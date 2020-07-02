#include "config.h"

#include "stm32h7xx_ll_utils.h"

#include "AggregatMotor.h"

#if USE_USBMIDI == 1
#include "UsbMidiAggregat.h"
#endif

#if USE_NETMIDI == 1
#include "EthernetInterface.h"
// #include "LWIPStack.h"
#include "nsapi_types.h"
#endif

#include "midimessage/midimessage.h"
#include "midimessage/simpleparser.h"

/************ TYPES ************/

typedef enum {
    SourceUsb,
    SourceMidi,
    SourceNet
} Source;

typedef enum {
    ConfigStateValid = 0x1337BEEF
} ConfigState;

typedef struct {
    ConfigState config_state;
    uint32_t device_id;
    // volatile uint8_t channel_offset;
    // volatile uint8_t controller_offset;
} __attribute__((packed)) Config;

/************ SIGNATURES ************/

void load_config();
void save_config();

void motors_init();
void motors_suspend();
void motors_resume();

void controller_init();
void controller_handle_msg(uint8_t * buffer, size_t length, Source source);

#if USE_USBMIDI == 1
void usbmidi_init();
void usbmidi_run();
void usbmidi_tx(uint8_t * buffer, size_t length);

// void usbmidi_event_callback();
void usbmidi_rx(uint8_t * buffer, uint8_t length, void * context); // parser callback handler
#else //USE_USBMIDI == 0
#define usbmidi_init()
#define usbmidi_run()
#define usbmidi_tx()
#endif //USE_USBMIDI

#if USE_MIDI == 1
void midi_init();
void midi_run();
void midi_tx(uint8_t * buffer, size_t length);

void midi_rx(uint8_t * buffer, uint8_t length, void * context); // parser callback handler
#else //USE_MIDI == 0
#define midi_init()
#define midi_tx(buffer, length)
#define midi_run()
#endif //USE_MIDI

#if USE_NETMIDI == 1
void netmidi_init();
void netmidi_run();
void netmidi_tx(uint8_t * buffer, size_t length);

void eth_status_changed(nsapi_event_t evt, intptr_t intptr);
void eth_ifup();
#else //USE_NETMIDI == 0
#define netmidi_init()
#define netmidi_run()
#define netmidi_tx(buffer, length)
#endif //USE_NETMIDI

/************ LOCAL VARIABLES ************/

// will be overwritten immediately by load_config()
Config config = {
    .device_id = 0, 
};

volatile bool motors_running = false;
AggregatMotor motors[MOTOR_COUNT] = {
  AggregatMotor(MOTOR_1_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC)
};

volatile int32_t channel_offset = CHANNEL_OFFSET;
volatile int32_t controller_offset = CONTROLLER_OFFSET;


#if USE_USBMIDI == 1
DigitalOut usbmidi_led(USB_CONNECTED_LED);
UsbMidiAggregat usbmidi(USB_POWER_PIN, false);
bool usb_to_midi = USBMIDI_FORWARD_TO_MIDI;
bool usb_to_net = USBMIDI_FORWARD_TO_NET;

MidiMessage::SimpleParser_t usbmidi_parser;
#endif

#if USE_MIDI == 1
BufferedSerial midi(MIDI_TX_PIN, MIDI_RX_PIN);
bool midi_to_usb = MIDI_FORWARD_TO_USB;
bool midi_to_net = MIDI_FORWARD_TO_NET;

MidiMessage::SimpleParser_t midi_parser;
// volatile State midi_state = StateStopped;
// Thread midi_thread;
#endif

#if USE_NETMIDI == 1
EthernetInterface eth;
Thread net_thread;
SocketAddress ip;
UDPSocket udp_sock;

bool eth_reconnect = false; 

DigitalOut net_led(NET_STATUS_LED);

bool net_to_midi = NETMIDI_FORWARD_TO_MIDI;
bool net_to_usb = NETMIDI_FORWARD_TO_USB;
char net_hostname[64] = "";
char net_servicename[64] = "";
// ns_mdns_t mdns_responder = NULL;
#endif

/************ INLINE FUNCTIONS ************/

inline float u7_to_pos(uint8_t value)
{
    // assert_param((value & 0x7F) == 0);

    return ((float)value) / 128.0;
}

inline float u14_to_pos(uint16_t value)
{
    // assert_param((value & 0x3FFF) == 0);

    return ((float)value) / 16383.0;
}

inline float s14_to_pos(int value)
{
    return u14_to_pos(value + 8192);
}


void do_nothing(){}

/************ FUNCTIONS ************/



void load_config()
{
    uint32_t uidxor = LL_GetUID_Word0() ^ LL_GetUID_Word1() ^ LL_GetUID_Word2();

    config.device_id = uidxor;


    srand(uidxor + time(NULL));

    // config.channel_offset = CHANNEL_OFFSET;
    // config.controller_offset = CONTROLLER_OFFSET;
}

void save_config()
{

}

void motors_init()
{
    for(int i = 0; i < MOTOR_COUNT; i++){

        // init to center position
        motors[i].set(0.5);

        // start suspended
        motors[i].suspend();
    }
}

void motors_suspend()
{
    for(int i = 0; i < MOTOR_COUNT; i++){
        motors[i].suspend();
    }

    motors_running = false;
}

void motors_resume()
{
    for(int i = 0; i < MOTOR_COUNT; i++){
        motors[i].resume();
    }
    motors_running = true;
}

void controller_init()
{

}

void controller_handle_msg(uint8_t * buffer, size_t length, Source source)
{
    printf("CMD (len %d) ", length);
    for(int i = 0; i < length; i++){
        printf("%02x", buffer[i]);
    }
    printf("\n");


    #if ENABLE_CONTROLLER_LOGIC == 1
    MIDIMessage msg;

    msg.from_raw(buffer, length);

    if (msg.length == 1){   // system real time messages

        // 0xFA = start, 0xFB = continue
        if (msg.data[0] == 0xFA || msg.data[0] == 0xFB){
            motors_resume();
        }

        // 0xFC = stop
        if (msg.data[0] == 0xFC){
            motors_suspend();
        }

        #if ENABLE_SYSTEM_RESET == 1
        // 0xFF = reset
        if (msg.data[0] == 0xFF){
            system_reset();
            printf("system_reset??\n");
        }
        #else
        printf("ENABLE_SYSTEM_RESET == 0\n");
        #endif
    }



    if (msg.type() == MIDIMessage::ControlChangeType){

        if (msg.channel() == channel_offset){
            int32_t motori = msg.controller() - controller_offset;

            if (0 <= motori && motori < MOTOR_COUNT){
                float pos = u7_to_pos(msg.value());
                printf("motor[%d] = %d\n", motori, (int)(pos*100));
                motors[motori] = pos;
            }
        }
    }

    // each channel controls a motor starting from channel_offset
    if (msg.type() == MIDIMessage::PitchWheelType){
        int32_t motori = msg.channel() - channel_offset;
        
        if (0 <= motori && motori < MOTOR_COUNT){
            float pos = s14_to_pos(msg.pitch());
            motors[motori] = pos;
        }
        
    }

    #endif //ENABLE_CONTROLLER_LOGIC == 1

    if (source == SourceUsb){
        if (usb_to_midi){
            midi_tx(buffer, length);
        }
        if (usb_to_net){
            netmidi_tx(buffer, length);
        }
    }
    if (source == SourceMidi){
        if (midi_to_usb){
            usbmidi_tx(buffer, length);
        }
        if (midi_to_net){
            netmidi_tx(buffer, length);
        }
    }
}


#if USE_USBMIDI == 1

void usbmidi_init()
{
    // usbmidi.attach(do_nothing);
    // [](){
    //     // do nothing (it's really sad, but if there is no callback attached, it doesn't work...)
    // });

    static uint8_t buffer[128];

    MidiMessage::simpleparser_init(&usbmidi_parser, true, (uint8_t*)&buffer, sizeof(buffer), usbmidi_rx, NULL, NULL);
}

void usbmidi_run()
{
    usbmidi_led = usbmidi.connected();

    if (usbmidi.connected() == false){

        if (usbmidi.just_reconnected()){
            // midi_tx((uint8_t*)"1\n", 2);
            usbmidi.connect();
        }

        return;
    } 

    // printf("usbmidi.ready() = %d\n", usbmidi.ready());
    if (!usbmidi.ready()){
        return;
    }

    
    if (usbmidi.readable()){
        printf("ubsmidi.readable() = %d\n", usbmidi.readable());    
        // mark activity
        // usbmidi_led = 0;

        MIDIMessage msg;

        usbmidi.read(msg);

        if (msg.length == 0){
            return;
        }

        // MIDIMessage can contain multiple messages, thus we have to parse them like a stream...
        MidiMessage::simpleparser_receivedData(&usbmidi_parser, msg.data, (uint8_t)msg.length);
    }
}

void usbmidi_rx(uint8_t * buffer, uint8_t length, void * context) // parser callback handler
{
    controller_handle_msg(buffer, length, SourceUsb);
}

void usbmidi_tx(uint8_t * buffer, size_t length)
{
    if (usbmidi.ready() == false){
        return;
    }

    usbmidi.write(buffer, length);
}

#endif //USE_USBMIDI == 1


#if USE_MIDI == 1

void midi_init()
{
    printf("midi_init()\n");

    // setup MIDI UART according to specs
    midi.set_baud(MIDI_BAUD); // 31.25kbaud
    midi.set_format(MIDI_BITS, MIDI_PARITY, MIDI_STOP); // 8 data bits, no parity, 1 stop bit
    midi.set_flow_control(BufferedSerial::Disabled);

    midi.set_blocking(false);

    static uint8_t buffer[128];

    MidiMessage::simpleparser_init(&midi_parser, true, (uint8_t*)&buffer, sizeof(buffer), midi_rx, NULL, NULL);
}

void midi_run()
{
    static uint8_t buffer[128];

    if (midi.readable()){

        size_t rlen = midi.read(buffer, sizeof(buffer));

        if (rlen > 0){
            // echo
            // midi_tx(buffer,rlen);
            // printf("midi rx %d\n", rlen);

            MidiMessage::simpleparser_receivedData(&midi_parser, buffer, (uint8_t)rlen);
        }
    }
}

void midi_rx(uint8_t * buffer, uint8_t length, void * context)
{
    controller_handle_msg(buffer, length, SourceMidi);
}

void midi_tx(uint8_t * buffer, size_t length)
{
    if (midi.writable() == false){
        return;
    }

    midi.write(buffer, length);
}
    
#endif //USE_MIDI == 1

#if USE_NETMIDI == 1

void netmidi_init()
{
    const char * mac = eth.get_mac_address();
    printf("mac %s\n", mac ? mac : "none");

    sprintf(net_hostname, NET_HOSTNAME_FMT, config.device_id);
    printf("hostname %s\n", net_hostname);

    sprintf(net_servicename, NET_SERVICENAME_FMT, config.device_id);
    printf("servicename %s\n", net_servicename);

    eth.add_event_listener(eth_status_changed);


    if (udp_sock.open(&eth)){
        printf("open error\n");
        return;
    }
    printf("opened\n");

    if (udp_sock.bind(7)){
        printf("bind error\n");
        return;
    }
    printf("bound to port\n");

    udp_sock.set_blocking(false);

    // eth_reconnect = true;
    

    net_thread.start(eth_ifup);
}

void eth_status_changed(nsapi_event_t evt, intptr_t intptr)
{
    if (evt != NSAPI_EVENT_CONNECTION_STATUS_CHANGE){
        return;
    }

    static bool eth_was_up = false;

    switch(eth.get_connection_status()){
        case NSAPI_STATUS_LOCAL_UP: 
            printf("eth local up\n");
            return;

        case NSAPI_STATUS_GLOBAL_UP:
            eth_was_up = true;
            printf("eth global up\n");
            return;

        case NSAPI_STATUS_DISCONNECTED: 
            printf("eth disconnected!\n");
            return;

        case NSAPI_STATUS_CONNECTING:
            printf("eth connecting\n");
            if (eth_was_up){
                eth_was_up = false;
                eth_reconnect = true;
                printf("should reconnect\n");
                // eth.disconnect();
            }
            return;

        case NSAPI_STATUS_ERROR_UNSUPPORTED:
            printf("eth unsupported\n");
            return;
    }
}

bool eth_connect()
{
    // printf("eth.status = %d\n", eth.get_connection_status());
    if (eth.get_connection_status() != NSAPI_STATUS_DISCONNECTED){
        eth.disconnect();
    }

    eth.set_default_parameters();

    // to force dhcp
    eth.set_network("0.0.0.0", "0.0.0.0", "0.0.0.0");

    int res;
    
    do {
        res = eth.connect();
    } while( res != NSAPI_ERROR_OK && res != NSAPI_ERROR_DHCP_FAILURE );

    // if (res == NSAPI_ERROR_NO_CONNECTION){
    //     return false;
    // }

    if (res == NSAPI_ERROR_OK) {
    
        // Show the network address
        eth.get_ip_address(&ip);
        printf("%s: %s\n", ip.get_ip_version() == NSAPI_IPv4 ? "ipv4" : "ipv6", ip.get_ip_address());

        return true;
    }

    if (res != NSAPI_ERROR_DHCP_FAILURE){
        printf("connect failed: %d\n", res);
        return false;
    }

    printf("dhcp timeout\n");


    res = eth.get_ipv6_link_local_address(&ip);

    if (res == NSAPI_ERROR_OK){
        printf("ipv6 link-local: %s\n", ip.get_ip_address());
        return true;
    }

    if (res == NSAPI_ERROR_UNSUPPORTED){
        printf("ipv6 link-local not supported\n");
    } else {
        printf("ipv6 link-local fail: %d\n", res);
    }

    // has to disconnect to set manual address
    eth.disconnect();

    // setup ipv4 link-local address
    uint8_t s3 = (((config.device_id >> 24) ^ (config.device_id >> 16)) % 254) + 1;
    uint8_t s4 = (config.device_id >> 8) ^ config.device_id;
    char ll[20] = "";
    sprintf(ll, "169.254.%hhu.%hhu", s3, s4);
    ip.set_ip_address(ll);


    printf("Using ipv4 link-local: %s/16\n", ll);
    eth.set_network(ip, "255.255.0.0", "0.0.0.0");

    if (eth.connect()){
        printf("link-local connect fail!\n");
        return false;
    } 

    return true;
}

void eth_ifup()
{
    printf("eth_ifup()\n");

    eth_reconnect = true;


    while (1){

        // if (exclusive_source == ExclusiveSourceNone){
        //     ThisThread::sleep_for(1000);
        //     continue;
        // }

        // if (exclusive_source_mutex.trylock() == false){
        //     continue;
        // }

        if (eth_reconnect == false){
            ThisThread::sleep_for(1000);
            continue;
        }


        if (eth_connect() == false){
            // exclusive_source_mutex.unlock();
            return;
        }
        
        // exclusive_source = ExclusiveSourceNet;
        // exclusive_source_mutex.unlock();

        eth_reconnect = false;

        // SocketAddress addr;

        // uint8_t hello[] = "hello!";

        // addr.set_ip_address("169.254.108.82");
        // addr.set_port(6666);

        // int r = udp_sock.sendto(addr, hello, sizeof(hello));
        // if (r < 0){
        //     printf("tx err %d\n", r);
        //     continue;
        // } else {
        //     printf("sent hello\n");
        // }
    }

}

void netmidi_run()
{
    net_led = eth.get_connection_status() == NSAPI_STATUS_GLOBAL_UP;

    if (eth.get_connection_status() != NSAPI_STATUS_GLOBAL_UP){
        return;
    }

    SocketAddress addr;

    addr.set_ip_address("169.254.108.82");
    addr.set_port(6666);

    uint8_t data[256];

    nsapi_size_or_error_t res = udp_sock.recv(data, sizeof(data));

    if (res > 0){
        data[res] = '\0';
        printf("udp rx (%d) %s", res, data);
        
        // echo back
        udp_sock.sendto(addr, data, res);
    }
}

void netmidi_tx(uint8_t * buffer, size_t length)
{
    //TODO 
}

#endif //USE_NETMIDI == 1

// main() runs in its own thread in the OS
int main()
{
    printf("\nRESTART\n");
    // midi_tx((uint8_t*)"RESTART\n",8);

    load_config();
    
    // initialize motors
    motors_init();

    // initialize controller
    controller_init();

    // any com interface initialization
    usbmidi_init();
    midi_init();
    netmidi_init();

    // start up handlers
    // midi_start();

    motors_resume();

    // Thread thr;

    // thr.start([](){

    //     while(1){
    //         printf("midi_tx\n");

    //         static uint8_t controller = 1;
    //         static uint8_t value = 127;

    //         value = (value + 1) % 128;

    //         uint8_t cc[3] = {0xB1, controller, value};

    //         usbmidi_tx(cc, 3);

    //         wait_us(1000000);
    //     }

    // });

    while (true) {
        usbmidi_run();
        midi_run();
        netmidi_run();


// if (usbmidi.ready()){
//             static uint8_t controller = 1;
//             static uint8_t value = 127;

//             value = (value + 1) % 128;

//             uint8_t cc[3] = {0xB1, controller, value};

//             usbmidi_tx(cc, 3);

//             wait_us(1000000);
// }
    }
}

