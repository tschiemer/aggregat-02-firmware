#include "config.h"

#include "stm32h7xx_ll_utils.h"

#include "AggregatMotor.h"

#if USE_USBMIDI == 1
#include "UsbMidiAggregat.h"
#endif

#if USE_NETMIDI == 1
// low level interfaces needed to solve platform bug
#include "stm32h743xx.h"
#include "stm32xx_emac.h"

#include "EthernetInterface.h"
// #include "LWIPStack.h"
#include "nsapi_types.h"

// mdns responder
#include "minimr.h"
#endif

#include "midimessage/midimessage.h"
#include "midimessage/simpleparser.h"
#include "midimessage/packers.h"


/************ TYPES ************/

typedef enum {
    SourceUsb,
    SourceMidi,
    SourceNet
} Source;

/************ SIGNATURES ************/

void core_init();

void gpio_init();

void motors_init();
void motors_suspend();
void motors_resume();
void motors_run();

void motors_center();

// void controller_init();
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

void mdns_init();
void mdns_deinit();
int mdns_rr_callback(enum minimr_dns_rr_fun_type type, struct minimr_dns_rr * rr, ...);
#else //USE_NETMIDI == 0
#define netmidi_init()
#define netmidi_run()
#define netmidi_tx(buffer, length)
#endif //USE_NETMIDI

/************ LOCAL VARIABLES ************/

uint32_t hw_device_id = 0;

volatile bool btn_plus_1_request = false;
volatile bool btn_plus_2_request = false;

volatile bool motors_running = false;
volatile bool motors_center_request = false;

AggregatMotor motors[MOTOR_COUNT] = {
  #if MOTOR_COUNT > 0
  AggregatMotor(MOTOR_1_PWR_PIN, MOTOR_1_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 1
  AggregatMotor(MOTOR_2_PWR_PIN, MOTOR_2_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 2
  AggregatMotor(MOTOR_3_PWR_PIN, MOTOR_3_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 3
  AggregatMotor(MOTOR_4_PWR_PIN, MOTOR_4_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 4
  AggregatMotor(MOTOR_5_PWR_PIN, MOTOR_5_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 5
  AggregatMotor(MOTOR_6_PWR_PIN, MOTOR_6_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 6
  AggregatMotor(MOTOR_7_PWR_PIN, MOTOR_7_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 7
  AggregatMotor(MOTOR_8_PWR_PIN, MOTOR_8_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 8
  AggregatMotor(MOTOR_9_PWR_PIN, MOTOR_9_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 9
  AggregatMotor(MOTOR_10_PWR_PIN, MOTOR_10_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 10
  AggregatMotor(MOTOR_11_PWR_PIN, MOTOR_11_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 11
  AggregatMotor(MOTOR_12_PWR_PIN, MOTOR_12_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 12
  AggregatMotor(MOTOR_13_PWR_PIN, MOTOR_13_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 13
  AggregatMotor(MOTOR_14_PWR_PIN, MOTOR_14_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 14
  AggregatMotor(MOTOR_15_PWR_PIN, MOTOR_15_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 15
  AggregatMotor(MOTOR_16_PWR_PIN, MOTOR_16_PWM_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
};

#if USE_STATUS_LEDS
DigitalOut led_pwr(LED_PWR_PIN, 1);
DigitalOut led_motors(LED_MTR_PIN, 0);
#endif

#if USE_CHANNEL_SELECT 
BusIn channel_select_bus(CHANNEL_SELECT_PIN_1, CHANNEL_SELECT_PIN_2, CHANNEL_SELECT_PIN_3, CHANNEL_SELECT_PIN_4);
#define get_channel() channel_select_bus.read()
#else
#define get_channel() CHANNEL_SELECT_DEFAULT
#endif //USE_CHANNEL_SELECT

#if USE_DEVICE_ID_SELECT
BusIn device_id_bus(DEVICE_ID_SELECT_PIN_1, DEVICE_ID_SELECT_PIN_2, DEVICE_ID_SELECT_PIN_3, DEVICE_ID_SELECT_PIN_4);
#define get_device_id()     device_id_bus.read()
#else
#define get_device_id()     DEVICE_ID_SELECT_DEFAULT
#endif //USE_ID_SELECT

#if USE_CFG
DigitalIn cfg1(CFG_RSRV_1_PIN);
DigitalIn cfg2(CFG_RSRV_2_PIN);
#endif //USE_CFG

#if USE_BUTTONS
InterruptIn btn_center(BTN_CENTER_PIN);
InterruptIn btn_plus1(BTN_PLUS_1_PIN);
InterruptIn btn_plus2(BTN_PLUS_2_PIN);

volatile bool btn_center_touched = false;
volatile bool btn_plus1_touched = false;
volatile bool btn_plus2_touched = false;
#endif


#if USE_USBMIDI
UsbMidiAggregat usbmidi(USB_POWER_PIN, false);
MidiMessage::SimpleParser_t usbmidi_parser;
#endif //USE_USBMIDI


#if USE_USBMIDI && USE_STATUS_LEDS
DigitalOut usb_led1(LED_USB_1_PIN);
DigitalOut usb_led2(LED_USB_2_PIN);
#endif

#if USE_USBMIDI && USE_FORWARDING_CONTROLS
DigitalIn usb_to_midi_pin(USB_TO_MIDI_PIN);
DigitalIn usb_to_net_pin(USB_TO_NET_PIN);

#define usb_to_midi()   usb_to_midi_pin.read()
#define usb_to_net()    usb_to_net_pin.read()
#else
#define usb_to_midi()   USB_TO_MIDI_DEFAULT    
#define usb_to_net()    USB_TO_NET_DEFAULT
#endif //USE_USBMIDI && USE_FORWARDING_CONTROLS



#if USE_MIDI
BufferedSerial midi(MIDI_TX_PIN, MIDI_RX_PIN);

MidiMessage::SimpleParser_t midi_parser;
#endif //USE_USBMIDI

#if USE_MIDI && USE_FORWARDING_CONTROLS
DigitalIn midi_to_usb_pin(MIDI_TO_USB_PIN);
DigitalIn midi_to_net_pin(MIDI_TO_NET_PIN);
#define midi_to_usb()   midi_to_usb_pin.read()
#define midi_to_net()   midi_to_net_pin.read()
#else
#define midi_to_usb()   MIDI_TO_USB_DEFAULT
#define midi_to_net()   MIDI_TO_NET_DEFAULT
#endif // USE_MIDI && USE_FORWARDING_CONTROLS

#if USE_MIDI && USE_STATUS_LEDS
DigitalOut midi_led1(LED_MIDI_1_PIN);
DigitalOut midi_led2(LED_MIDI_2_PIN);
#endif


#if USE_NETMIDI 
EthernetInterface eth;
Thread net_thread;
SocketAddress ip;
UDPSocket udp_sock;

bool eth_reconnect = false; 

// DigitalOut net_led(LED_NET_1_PIN);

minimr_dns_rr_a RR_A = {
    .type = MINIMR_DNS_TYPE_A,
    .cache_class = MINIMR_DNS_CACHEFLUSH | MINIMR_DNS_CLASS_IN,
    .ttl = 120,
    .fun = mdns_rr_callback
};
minimr_dns_rr_aaaa RR_AAAA = {
    .type = MINIMR_DNS_TYPE_AAAA,
    .cache_class = MINIMR_DNS_CACHEFLUSH | MINIMR_DNS_CLASS_IN,
    .ttl = 120,
    .fun = mdns_rr_callback
};
struct minimr_dns_rr * mdns_records[MDNS_RR_COUNT];
SocketAddress mdns_addr;

UDPSocket mdns_sock;
#endif

#if USE_NETMIDI && USE_FORWARDING_CONTROLS
DigitalIn net_to_usb_pin(NET_TO_USB_PIN);
DigitalIn net_to_midi_pin(NET_TO_MIDI_PIN);

#define net_to_usb()    net_to_usb_pin.read()
#define net_to_midi()   net_to_midi_pin.read()
#else
#define net_to_usb()    NET_TO_USB_DEFAULT
#define net_to_midi()   NET_TO_MIDI_DEFAULT
#endif // USE_NETMIDI && USE_FORWARDING_CONTROLS

#if USE_NETMIDI && USE_STATUS_LEDS
DigitalOut net_led1(LED_NET_1_PIN);
DigitalOut net_led2(LED_NET_2_PIN);
#endif

/************ INLINE FUNCTIONS ************/

inline float u7_to_pos(uint8_t value)
{
    return ((float)value) / 128.0;
}

inline float u14_to_pos(uint16_t value)
{
    return ((float)value) / 16383.0;
}

inline float s14_to_pos(int value)
{
    return u14_to_pos(value + 8192);
}


void do_nothing(){}

/************ FUNCTIONS ************/



void core_init()
{
    hw_device_id = LL_GetUID_Word0() ^ LL_GetUID_Word1() ^ LL_GetUID_Word2();

    srand(hw_device_id + time(NULL));
}

void gpio_init()
{

    #if USE_DEVICE_ID_SELECT
    // device_id_bus.mode(PullNone);
    #endif

    #if USE_CHANNEL_SELECT
    // channel_select_bus.mode(PullNone);
    #endif

    #if USE_BUTTONS
    // btn_center.mode(PullDown);
    btn_center.rise([](){
        // motors_center_request = true;

        usb_led2 = !usb_led2;
    });
    // btn_center.mode(PullUp);
    btn_plus1.rise([](){
        midi_led2 = !midi_led2;
    });
    // btn_center.mode(PullDefault);
    btn_plus2.rise([](){
        net_led2 = !net_led2;
    });
    #endif //USE_BUTTONS

    printf("device_id %d\n", get_device_id());
    printf("channel %d\n", get_channel());

    printf("usb -> midi %d\n", usb_to_midi());
    printf("usb -> net %d\n", usb_to_net());
    printf("midi -> usb %d\n", midi_to_usb());
    printf("midi -> net %d\n", midi_to_net());
    printf("net -> usb %d\n", net_to_usb());
    printf("net -> midi %d\n", net_to_midi());

    #if USE_CFG
    printf("cfg1 %d\n", cfg1.read());
    printf("cfg2 %d\n", cfg2.read());
    #endif
}

void motors_init()
{
    for(int i = 0; i < MOTOR_COUNT; i++){

        // init to center position
        // motors[i] = 0.5;

        // start suspended (well, they are initialized in suspended mode)
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

void motors_run()
{
    if (motors_center_request){
        motors_center();
        motors_center_request = false;
    }

    for(int i = 0; i < MOTOR_COUNT; i++){
        motors[i].run();
    }
}

void motors_center()
{
    for(int i = 0; i < MOTOR_COUNT; i++){
        motors[i] = 0.5;
    }
}


void controller_handle_msg(uint8_t * buffer, size_t length, Source source)
{
    // printf("CMD (len %d) ", length);
    // for(int i = 0; i < length; i++){
    //     printf("%02x", buffer[i]);
    // }
    // printf("\n");


    #if USE_CONTROLLER_LOGIC == 1

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

        #if USE_SYSTEM_RESET == 1
        // 0xFF = reset
        if (msg.data[0] == 0xFF){
            system_reset();
            printf("system_reset??\n");
        }
        #else
        printf("USE_SYSTEM_RESET == 0\n");
        #endif
    }


    // position control
    if (msg.type() == MIDIMessage::ControlChangeType){
        if (msg.channel() == get_channel()){
            int32_t motori = msg.controller() - CC_CONTROLLER_OFFSET;

            if (0 <= motori && motori < MOTOR_COUNT){
                float pos = u7_to_pos(msg.value());
                // printf("motor[%d] = %d\n", motori, (int)(pos*100));
                motors[motori] = pos;
            }
        }
    }

    #if USE_PITCHBEND_CONTROL
    // each channel controls a motor starting from channel_offset
    if (msg.type() == MIDIMessage::PitchWheelType){
        int32_t motori = msg.channel();
        
        if (0 <= motori && motori < MOTOR_COUNT){
            float pos = s14_to_pos(msg.pitch());
            motors[motori] = pos;
        }
        
    }
    #endif //USE_PITCHBEND_CONTROL

    // power control
    if (msg.type() == MIDIMessage::NoteOnType || msg.type() == MIDIMessage::NoteOffType){
        if (msg.channel() == get_channel()){
            int32_t motori = msg.controller() - CC_CONTROLLER_OFFSET;

            if (0 <= motori && motori < MOTOR_COUNT){
                // turn on or off
                motors[motori] = msg.type() == MIDIMessage::NoteOnType;
            }
        }
    }


    #endif //USE_CONTROLLER_LOGIC == 1

    #if USE_USBMIDI
    if (source == SourceUsb){
        if (usb_to_midi()){
            midi_tx(buffer, length);
        }
        if (usb_to_net()){
            netmidi_tx(buffer, length);
        }
    }
    #endif
    #if USE_MIDI
    if (source == SourceMidi){
        if (midi_to_usb()){
            usbmidi_tx(buffer, length);
        }
        if (midi_to_net()){
            netmidi_tx(buffer, length);
        }
    }
    #endif
    #if USE_NETMIDI
    if (source == SourceNet){
        if (net_to_usb()){
            usbmidi_tx(buffer, length);
        }
        if (net_to_midi()){
            netmidi_tx(buffer, length);
        }
    }
    #endif
}

#if USE_NRPN_CONTROL
void controller_handle_nrpn(uint8_t channel, MidiMessage::NRpnType_t type, MidiMessage::NRpnAction_t action,  uint16_t controller, uint16_t value, Source source)
{
    #if USE_CONTROLLER_LOGIC == 1
    if (type == MidiMessage::NRpnTypeNRPN && channel == get_channel() && action == MidiMessage::NRpnActionValue){
        int32_t motori = controller - CC_CONTROLLER_OFFSET;
        // printf("controller %d\n", motori);

        if (0 <= motori && motori < MOTOR_COUNT){
            float pos = u14_to_pos(value);
            // printf("motor[%d] = %d\n", motori, (int)(pos*100));
            motors[motori] = pos;
        }
    }
    #endif //USE_CONTROLLER_LOGIC == 1
}
#endif //USE_NRPN_CONTROL


#if USE_USBMIDI == 1

void usbmidi_init()
{
    // usbmidi.attach(do_nothing);
    // [](){
    //     // do nothing (it's really sad, but if there is no callback attached, it doesn't work...)
    // });

    static uint8_t buffer[128];

    MidiMessage::simpleparser_init(
        &usbmidi_parser,
        true, // enable running status
        (uint8_t*)&buffer, sizeof(buffer),
        [](uint8_t * buffer, uint16_t length, void * context){
            controller_handle_msg(buffer, length, SourceUsb);
        },
        #if USE_NRPN_CONTROL
        [](uint8_t channel, MidiMessage::NRpnType_t type, MidiMessage::NRpnAction_t action,  uint16_t controller, uint16_t value, void * context){
            controller_handle_nrpn(channel, type, action, controller, value, SourceUsb);
        },
        #else
        NULL,
        #endif
        NULL,
        NULL
    );
}

void usbmidi_run()
{
    usb_led1 = usbmidi.connected();

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
        // printf("ubsmidi.readable() = %d\n", usbmidi.readable());    
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

    MidiMessage::simpleparser_init(
        &midi_parser,
        true, // enable running status
        (uint8_t*)&buffer, sizeof(buffer),
        [](uint8_t * buffer, uint16_t length, void * context){
            controller_handle_msg(buffer, length, SourceMidi);
        },
        #if USE_NRPN_CONTROL
        [](uint8_t channel, MidiMessage::NRpnType_t type, MidiMessage::NRpnAction_t action,  uint16_t controller, uint16_t value, void * context){
            controller_handle_nrpn(channel, type, action, controller, value, SourceMidi);
        },
        #else
        NULL,
        #endif
        NULL,
        NULL
    );
}

void midi_run()
{
    static uint8_t buffer[128];

    if (midi.readable()){

        size_t rlen = midi.read(buffer, sizeof(buffer));

        if (rlen > 0){
            MidiMessage::simpleparser_receivedData(&midi_parser, buffer, (uint8_t)rlen);
        }
    }
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
    // low-level enable multicast pass all 
    WRITE_REG(ETH->MACPFR, READ_REG(ETH->MACPFR) | (1 << 4));

    const char * mac = eth.get_mac_address();
    printf("mac %s\n", mac ? mac : "none");


    eth.add_event_listener(eth_status_changed);

    
    if (udp_sock.open(&eth)){
        printf("udp_sock: open error\n");
        return;
    }
    if (udp_sock.bind(7)){
        printf("udp_sock: bind error\n");
        return;
    }
    udp_sock.set_blocking(false);

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
                mdns_deinit();
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
    uint8_t s3 = (((hw_device_id >> 24) ^ (hw_device_id >> 16)) % 254) + 1;
    uint8_t s4 = (hw_device_id >> 8) ^ hw_device_id;
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

void mdns_init()
{
    memset(mdns_records, 0, sizeof(mdns_records));

    if (ip.get_ip_version() == NSAPI_IPv4){
        memcpy(RR_A.ipv4, ip.get_ip_bytes(), sizeof(RR_A.ipv4));

        sprintf((char*)RR_A.name, NET_HOSTNAME_FMT, get_device_id());
        printf("A.name %s\n", (char*)RR_A.name);

        minimr_dns_normalize_name(RR_A.name, &RR_A.name_length);

        mdns_records[0] = (struct minimr_dns_rr *)&RR_A;

        mdns_addr.set_ip_address("224.0.0.251");
        mdns_addr.set_port(5353);
    } else {
        memcpy(RR_AAAA.ipv6, ip.get_ip_bytes(), sizeof(RR_AAAA.ipv6));

        sprintf((char*)RR_AAAA.name, NET_HOSTNAME_FMT, get_device_id());
        printf("AAAA.name %s\n", (char*)RR_AAAA.name);

        minimr_dns_normalize_name(RR_AAAA.name, &RR_AAAA.name_length);


        mdns_records[0] = (struct minimr_dns_rr *)&RR_AAAA;

        mdns_addr.set_ip_address("ff02::fb");
        mdns_addr.set_port(5353);
    }


    if (mdns_sock.open(&eth)){
        printf("mdns_sock: open error\n");
        return;
    }
    if (mdns_sock.bind(5353)){
        printf("mdns_sock: bind error\n");
        return;
    }
    mdns_sock.set_blocking(false);

    if (mdns_sock.join_multicast_group(mdns_addr)){
        printf("mdns_sock: failed to join multicast grp\n");
        return;
    }
    
}

void mdns_deinit()
{
    if (mdns_sock.leave_multicast_group(mdns_addr)){
        printf("mdns_sock: failed leave multicast group\n");
    }

    if (mdns_sock.close()){
        printf("mdns_sock: failed close\n");
    }
}

int mdns_rr_callback(enum minimr_dns_rr_fun_type type, struct minimr_dns_rr * rr, ...)
{
    if (type == minimr_dns_rr_fun_type_respond_to){
        // always respond
        return MINIMR_RESPOND;
    }

    // it isn't necessarily safe to put this here

    va_list args;
    va_start(args, rr);

    uint8_t * outmsg = va_arg(args, uint8_t *);
    uint16_t * outmsglen = va_arg(args, uint16_t *);
    uint16_t outmsgmaxlen = va_arg(args, int); // uint16_t will be promoted to int
    uint16_t * nrr = va_arg(args, uint16_t *);

    va_end(args);


    if (type == minimr_dns_rr_fun_type_get_rr){
        if ((rr->type == MINIMR_DNS_TYPE_A && outmsgmaxlen < MINIMR_DNS_RR_A_SIZE(rr->name_length)) ||
            (rr->type == MINIMR_DNS_TYPE_AAAA && outmsgmaxlen < MINIMR_DNS_RR_AAAA_SIZE(rr->name_length))) {
            return MINIMR_NOT_OK;
        }

        uint16_t l = *outmsglen;

        // helper macros to write all the standard fields of the record
        // you can naturally do this manually and customize it ;)

        MINIMR_DNS_RR_WRITE_COMMON(outmsg, l, rr->name, rr->name_length, rr->type, rr->cache_class, rr->ttl);

        if (rr->type == MINIMR_DNS_TYPE_A) {
            MINIMR_DNS_RR_WRITE_A_BODY(outmsg, l, ((minimr_dns_rr_a*)rr)->ipv4);
        }
        else if (rr->type == MINIMR_DNS_TYPE_AAAA) {
            MINIMR_DNS_RR_WRITE_AAAA_BODY(outmsg, l, ((minimr_dns_rr_aaaa*)rr)->ipv6);
        }
        else {
            return MINIMR_NOT_OK;
        }

        *outmsglen = l;
        *nrr = 1;
    }

    return MINIMR_OK;
}


void eth_ifup()
{
    printf("eth_ifup()\n");

    eth_reconnect = true;

    uint8_t mdns_in[1024];
    uint8_t mdns_out[1024];
    uint16_t mdns_inlen = 0;
    uint16_t mdns_outlen = 0;

    int res = 0;

    while (1){

        if (eth.get_connection_status() != NSAPI_STATUS_GLOBAL_UP){

            if (eth_reconnect == false){
                wait_us(1000000);
                continue;
            }

            if (eth_connect() == false){
                return;
            }

            eth_reconnect = false;

            
            mdns_init();

            // only announcing twice
            res = minimr_announce(mdns_records, MDNS_RR_COUNT, mdns_out, &mdns_outlen, sizeof(mdns_out), NULL);

            if (res != MINIMR_OK){
                printf("minimr_announce: failed %d\n", res);
                continue;
            }

            if (mdns_outlen > 0){
                // mdns_sock.set_blocking(false);
                mdns_sock.sendto(mdns_addr, mdns_out, mdns_outlen);
                wait_us(1000000);
                mdns_sock.sendto(mdns_addr, mdns_out, mdns_outlen);
                mdns_sock.set_blocking(false);
            }
        }

        
        // check if there is an mdns message and react to it
        SocketAddress addr;
        nsapi_size_or_error_t sockres = mdns_sock.recvfrom(&addr, mdns_in, sizeof(mdns_in));

        if (sockres > 0){
            // printf("mdns rx %d %s\n", res, addr.get_ip_address());

            struct minimr_dns_query_stat qstats[MDNS_RR_COUNT];
            
            uint8_t unicast_requested = 0;

            res = minimr_handle_queries(mdns_in, res, qstats, MDNS_RR_COUNT, mdns_records, MDNS_RR_COUNT, mdns_out, &mdns_outlen, sizeof(mdns_out), &unicast_requested);

            if (res != MINIMR_OK){
                // printf("minimr_handle: failed %d\n", res);
                continue;
            } else {
                printf("mdns responding\n");
            }

            if (mdns_outlen > 0){
                if (unicast_requested){
                    mdns_sock.sendto(addr, mdns_out, mdns_outlen);
                } else {
                    mdns_sock.sendto(mdns_addr, mdns_out, mdns_outlen);
                }
            }

        } else if (sockres < 0) {
            if (sockres != NSAPI_ERROR_WOULD_BLOCK){
                printf("mdns_sock.recv err %d\n", sockres);
            }
        } // mdns responder
    }

}

void netmidi_run()
{
    net_led1 = eth.get_connection_status() == NSAPI_STATUS_GLOBAL_UP;

    if (eth.get_connection_status() != NSAPI_STATUS_GLOBAL_UP){
        return;
    }


    uint8_t data[256];
    SocketAddress addr;

    nsapi_size_or_error_t res = udp_sock.recvfrom(&addr, data, sizeof(data));

    if (res > 0){
        data[res] = '\0';
        printf("udp rx (%d) from %s: %s\n", res, addr.get_ip_address(), data);
        
        // echo back
        udp_sock.sendto(addr, data, res);
    } else if (res < 0) {
        if (res != NSAPI_ERROR_WOULD_BLOCK){
            printf("rx err %d\n", res);
        }
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

    core_init();

    // initialize gpios
    gpio_init();

    // initialize motors
    motors_init();

    // any com interface initialization
    usbmidi_init();
    midi_init();
    netmidi_init();

    // start motors
    // motors_resume();

    while (true) {
        usbmidi_run();
        midi_run();
        netmidi_run();
        motors_run();
    }
}

