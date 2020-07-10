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

typedef enum {
    ConfigStateValid = 0x1337BEEF
} ConfigState;

typedef struct {
    ConfigState config_state;
    uint32_t device_id;
    // volatile uint8_t channel_offset;
} __attribute__((packed)) Config;

/************ SIGNATURES ************/

void load_config();
void save_config();

void gpio_init();

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

void mdns_configure();
int mdns_rr_callback(enum minimr_dns_rr_fun_type type, struct minimr_dns_rr * rr, ...);
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
    #if MOTOR_COUNT > 0
  AggregatMotor(MOTOR_1_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 1
  AggregatMotor(MOTOR_2_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 2
  AggregatMotor(MOTOR_3_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 3
  AggregatMotor(MOTOR_4_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 4
  AggregatMotor(MOTOR_5_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 5
  AggregatMotor(MOTOR_6_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 6
  AggregatMotor(MOTOR_7_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC),
  #endif
  #if MOTOR_COUNT > 7
  AggregatMotor(MOTOR_8_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC)
  #endif
};

#if USE_CHANNEL_SELECT 
BusIn channel_select_bus(CHANNEL_SELECT_1_PIN, CHANNEL_SELECT_2_PIN, CHANNEL_SELECT_3_PIN, CHANNEL_SELECT_4_PIN);
#define get_channel() channel_select_bus.read()
#else
#define get_channel() CHANNEL_OFFSET
#endif


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
}

void save_config()
{

}

void gpio_init()
{
    #if USE_CHANNEL_SELECT
    channel_select_bus.mode(PullUp);
    #endif
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
    // printf("CMD (len %d) ", length);
    // for(int i = 0; i < length; i++){
    //     printf("%02x", buffer[i]);
    // }
    // printf("\n");


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

        // TO BE DEFINED
        // printf("channel %d\n",get_channel());

        // example
        if (msg.channel() == get_channel()){
            int32_t motori = msg.controller() - CONTROLLER_OFFSET;
            // printf("controller %d\n", motori);

            if (0 <= motori && motori < MOTOR_COUNT){
                float pos = u7_to_pos(msg.value());
                // printf("motor[%d] = %d\n", motori, (int)(pos*100));
                motors[motori] = pos;
            }
        }
    }

    // each channel controls a motor starting from channel_offset
    if (msg.type() == MIDIMessage::PitchWheelType){
        int32_t motori = msg.channel();
        
        if (0 <= motori && motori < MOTOR_COUNT){
            float pos = s14_to_pos(msg.pitch());
            motors[motori] = pos;
        }
        
    }

    #endif //ENABLE_CONTROLLER_LOGIC == 1

    #if USE_USBMIDI
    if (source == SourceUsb){
        if (usb_to_midi){
            midi_tx(buffer, length);
        }
        if (usb_to_net){
            netmidi_tx(buffer, length);
        }
    }
    #endif
    #if USE_MIDI
    if (source == SourceMidi){
        if (midi_to_usb){
            usbmidi_tx(buffer, length);
        }
        if (midi_to_net){
            netmidi_tx(buffer, length);
        }
    }
    #endif
    #if USE_NETMIDI
    if (source == SourceNet){
        if (net_to_usb){
            usbmidi_tx(buffer, length);
        }
        if (net_to_midi){
            netmidi_tx(buffer, length);
        }
    }
    #endif
}

void controller_handle_nrpn(uint8_t channel, MidiMessage::NRpnType_t type, MidiMessage::NRpnAction_t action,  uint16_t controller, uint16_t value, Source source)
{
    
    #if ENABLE_CONTROLLER_LOGIC == 1
    if (type == MidiMessage::NRpnTypeNRPN && channel == get_channel() && action == MidiMessage::NRpnActionValue){
        int32_t motori = controller - CONTROLLER_OFFSET;
        // printf("controller %d\n", motori);

        if (0 <= motori && motori < MOTOR_COUNT){
            float pos = u14_to_pos(value);
            // printf("motor[%d] = %d\n", motori, (int)(pos*100));
            motors[motori] = pos;
        }
    }
    #endif //ENABLE_CONTROLLER_LOGIC == 1

    uint8_t buffer[12];
    uint8_t length = 0;

    if (type == MidiMessage::NRpnTypeNRPN){
        if (action == MidiMessage::NRpnActionValue){
            length = MidiMessage::packNrpnValue(buffer, channel, controller, value, false);
        } else if (action == MidiMessage::NRpnActionIncrement){
            length = MidiMessage::packNrpnIncrement(buffer, channel, controller, value, false);
        } else if (action == MidiMessage::NRpnActionDecrement){
            length = MidiMessage::packNrpnDecrement(buffer, channel, controller, value, false);
        }
    } else { // RPN
        if (action == MidiMessage::NRpnActionValue){
            length = MidiMessage::packRpnValue(buffer, channel, controller, value, false);
        } else if (action == MidiMessage::NRpnActionIncrement){
            length = MidiMessage::packRpnIncrement(buffer, channel, controller, value, false);
        } else if (action == MidiMessage::NRpnActionDecrement){
            length = MidiMessage::packRpnDecrement(buffer, channel, controller, value, false);
        }
    }
    
    if (length == 0){
        return;
    }

    #if USE_USBMIDI
    if (source == SourceUsb){
        if (usb_to_midi){
            midi_tx(buffer, length);
        }
        if (usb_to_net){
            netmidi_tx(buffer, length);
        }
    }
    #endif
    #if USE_MIDI
    if (source == SourceMidi){
        if (midi_to_usb){
            usbmidi_tx(buffer, length);
        }
        if (midi_to_net){
            netmidi_tx(buffer, length);
        }
    }
    #endif
    #if USE_NETMIDI
    if (source == SourceNet){
        if (net_to_usb){
            usbmidi_tx(buffer, length);
        }
        if (net_to_midi){
            netmidi_tx(buffer, length);
        }
    }
    #endif
}


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
        [](uint8_t channel, MidiMessage::NRpnType_t type, MidiMessage::NRpnAction_t action,  uint16_t controller, uint16_t value, void * context){
            controller_handle_nrpn(channel, type, action, controller, value, SourceUsb);
        },
        NULL,
        NULL
    );
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
        [](uint8_t channel, MidiMessage::NRpnType_t type, MidiMessage::NRpnAction_t action,  uint16_t controller, uint16_t value, void * context){
            controller_handle_nrpn(channel, type, action, controller, value, SourceMidi);
        },
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
            // echo
            // midi_tx(buffer,rlen);
            // printf("midi rx %d\n", rlen);

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
    // force initialization of interface/mac etc (it's really ridiculous how they designed this)
    eth.set_blocking(false);
    wait_us(10); // just a little bit of waiting to avoid potential problem of non-blocking setting not yet taking effect
    eth.connect();
    eth.disconnect();
    eth.set_blocking(true);
    
    // low-level: set MAC filter to pass multicast
    STM32_EMAC &emac = STM32_EMAC::get_instance();
    ETH_MACFilterConfigTypeDef pFilterConfig;
    HAL_ETH_GetMACFilterConfig(&emac.EthHandle, &pFilterConfig);
    pFilterConfig.PassAllMulticast = ENABLE;
    HAL_ETH_SetMACFilterConfig(&emac.EthHandle, &pFilterConfig);


    const char * mac = eth.get_mac_address();
    printf("mac %s\n", mac ? mac : "none");

    sprintf((char*)RR_A.name, NET_HOSTNAME_FMT, config.device_id);
    printf("hostname %s\n", (char*)&RR_A.name[1]);

    sprintf((char*)RR_AAAA.name, NET_SERVICENAME_FMT, config.device_id);
    printf("servicename %s\n", (char*)&RR_AAAA.name[1]);


    eth.add_event_listener(eth_status_changed);

    // Aggregat-02-245631f.local

    // mDNS initialization
    minimr_dns_normalize_name(RR_A.name, &RR_A.name_length);
    minimr_dns_normalize_name(RR_AAAA.name, &RR_AAAA.name_length);

    memset(mdns_records, 0, sizeof(mdns_records));

    if (mdns_sock.open(&eth)){
        printf("mdns_sock: open error\n");
        return;
    }
    if (mdns_sock.bind(5353)){
        printf("mdns_sock: bind error\n");
        return;
    }
    mdns_sock.set_blocking(false);


    
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
                mdns_sock.leave_multicast_group(mdns_addr);
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

void mdns_configure()
{

    if (ip.get_ip_version() == NSAPI_IPv4){
        memcpy(RR_A.ipv4, ip.get_ip_bytes(), sizeof(RR_A.ipv4));

        mdns_records[0] = (struct minimr_dns_rr *)&RR_A;

        mdns_addr.set_ip_address("224.0.0.251");
        mdns_addr.set_port(5353);
    } else {
        memcpy(RR_AAAA.ipv6, ip.get_ip_bytes(), sizeof(RR_AAAA.ipv6));

        mdns_records[0] = (struct minimr_dns_rr *)&RR_AAAA;

        mdns_addr.set_ip_address("ff02::fb");
        mdns_addr.set_port(5353);
    }

    if (mdns_sock.join_multicast_group(mdns_addr)){
        printf("mdns_sock: failed to join multicast grp\n");
        return;
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

            
            mdns_configure();

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
    net_led = eth.get_connection_status() == NSAPI_STATUS_GLOBAL_UP;

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

    load_config();

    // initialize gpios
    gpio_init();

    // initialize motors
    motors_init();

    // initialize controller
    controller_init();

    // any com interface initialization
    usbmidi_init();
    midi_init();
    netmidi_init();

    motors_resume();

    while (true) {
        usbmidi_run();
        midi_run();
        netmidi_run();
    }
}

