#include "config.h"

#include "mbed.h"

#include "AggregatMotor.h"

#if USE_USBMIDI == 1
#include "UsbMidiAggregat.h"
#endif

#if USE_NETMIDI == 1
#include "EthernetInterface.h"
// #include "ns_mdns_api.h"
#include "LWIPStack.h"
#include "lwip/apps/mdns.h"
#endif

#include "midimessage/midimessage.h"
#include "midimessage/simpleparser.h"

/************ TYPES ************/

// typedef enum {
//     StateStopped,
//     StateStarting,
//     StateStarted,
//     StateStopping
// } State;

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
    u8_t device_id;
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
void usbmidi_event_callback();
void usbmidi_tx(uint8_t * buffer, size_t length);
#else //USE_USBMIDI == 0
#define usbmidi_init()
#define usbmidi_tx()
#endif //USE_USBMIDI

#if USE_MIDI == 1
void midi_init();
void midi_run();
void midi_tx(uint8_t * buffer, size_t length);
void midi_rx(uint8_t * buffer, uint8_t length, void * context); // parser callback handler
// void midi_start();
// void midi_stop();
#else //USE_MIDI == 0
#define midi_init()
#define midi_tx(buffer, length)
#define midi_run()
// #define midi_start()
// #define midi_stop()
#endif //USE_MIDI

#if USE_NETMIDI == 1
void netmidi_init();
void netmidi_tx(uint8_t * buffer, size_t length);
#else //USE_NETMIDI == 0
#define netmidi_init()
#define netmidi_tx(buffer, length)
#endif //USE_NETMIDI

/************ LOCAL VARIABLES ************/

Config config = {
    .device_id = 1
};

volatile bool motors_running = false;
AggregatMotor motors[MOTOR_COUNT] = {
  AggregatMotor(MOTOR_1_PIN, MOTOR_REFRESH_RATE_HZ, MOTOR_PULSEWIDTH_MIN_USEC, MOTOR_PULSEWIDTH_MAX_USEC)
};

volatile int32_t channel_offset = CHANNEL_OFFSET;
volatile int32_t controller_offset = CONTROLLER_OFFSET;

#if USE_USBMIDI == 1
UsbMidiAggregat usbmidi(false);
bool usb_to_midi = USBMIDI_FORWARD_TO_MIDI;
bool usb_to_net = USBMIDI_FORWARD_TO_NET;
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
UDPSocket udp_sock;
bool net_to_midi = NETMIDI_FORWARD_TO_MIDI;
bool net_to_usb = NETMIDI_FORWARD_TO_USB;
char net_hostname[] = NET_HOSTNAME;
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

/************ FUNCTIONS ************/



void load_config()
{
    config.device_id = 1;
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
        }
        #endif
    }



    if (msg.type() == MIDIMessage::ControlChangeType){

        if (msg.channel() == channel_offset){
            int32_t motori = msg.controller() - controller_offset;

            if (0 <= motori && motori < MOTOR_COUNT){
                float pos = u7_to_pos(msg.value());
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
}


#if USE_USBMIDI == 1

void usbmidi_init()
{
    usbmidi.attach(usbmidi_event_callback);

    usbmidi.connect();
}

void usbmidi_event_callback()
{
    if (usbmidi.ready() == false){
        return;
    }

    if (usbmidi.readable()){
        MIDIMessage msg;

        usbmidi.read(&msg);


        if (usb_to_midi){
            midi_tx(msg.data, msg.length);
        }
        if (usb_to_net){
            netmidi_tx(msg.data, msg.length);
        }

        controller_handle_msg(msg.data, msg.length, SourceUsb);
    }
}

void usbmidi_tx(uint8_t * buffer, size_t length)
{
    if (usbmidi.ready() == false){
        return;
    }

    
    MIDIMessage msg;

    msg.from_raw(buffer, length);

    usbmidi.write(msg);
}
#endif //USE_USBMIDI == 1


#if USE_MIDI == 1

void midi_init()
{
    // setup MIDI UART according to specs
    midi.set_baud(31250); // 31.25kbaud
    midi.set_format(8, BufferedSerial::None, 1); // 8 data bits, no parity, 1 stop bit
    midi.set_flow_control(BufferedSerial::Disabled);

    midi.set_blocking(false);

    uint8_t buffer[128];

    MidiMessage::simpleparser_init(&midi_parser, true, (uint8_t*)&buffer, sizeof(buffer), midi_rx, NULL, NULL);
}

void midi_tx(uint8_t * buffer, size_t length)
{
    if (midi.writable() == false){
        return;
    }

    midi.write(buffer, length);
}


void midi_rx(uint8_t * buffer, uint8_t length, void * context)
{
    if (midi_to_usb){
        usbmidi_tx(buffer, length);
    }
    if (midi_to_net){
        netmidi_tx(buffer, length);
    }

    controller_handle_msg(buffer, length, SourceMidi);
}

// void midi_start()
// {
//     if (midi_state != StateStopped){
//         return;
//     }
//     midi_state = StateStarting;

//     midi_thread.start(midi_run);
// }

// void midi_stop()
// {
//     if (midi_state != StateStarted){
//         return;
//     }

//     midi_state = StateStopping;

//     do {
//         ThisThread::sleep_for(100);
//     } while( midi_state == StateStopped);

// }

void midi_run()
{
    uint8_t buffer[128];
    size_t length = 0;
    // midi_state = StateStarted;

    // while(midi_state == StateStarted) {
        if (midi.readable()){
            size_t rlen = midi.read(&buffer[length], sizeof(buffer) - length);

            if (rlen > 0){
                MidiMessage::simpleparser_receivedData(&midi_parser, buffer, (uint8_t)length);
            }
        }
    // }

    // midi_state = StateStopped;
}
    
#endif //USE_MIDI == 1

#if USE_NETMIDI == 1

void netmidi_init()
{
    // udp_sock.open()

    // mdns_resp_init();
    // mdns_responder = ns_mdns_server_start(NET_HOSTNAME, 0,0,0);


    sprintf(net_hostname, "%s-%d", NET_HOSTNAME, config.device_id);

    // LWIP & lwip = LWIP::get_instance();

    

    // mdns_resp_init();


    // mdns_resp_add_netif(net_hostname, );
    // mdns_resp_add_service();
}

void netmidi_deinit()
{
    // mdns_resp_remove_netif();
}

void netmidi_tx(uint8_t * buffer, size_t length)
{

}

#endif //USE_NETMIDI == 1

// main() runs in its own thread in the OS
int main()
{
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

    while (true) {
        midi_run();
    }
}

