#include "mbed.h"

/************ FEATURES ************/

// default value 
#define CHANNEL_OFFSET      0
#define CONTROLLER_OFFSET   0

#define ENABLE_SYSTEM_RESET 1

/************ MOTORS ************/

#define MOTOR_REFRESH_RATE_HZ       60

#define MOTOR_PULSEWIDTH_MIN_USEC   556
#define MOTOR_PULSEWIDTH_MAX_USEC   2410

#define MOTOR_COUNT         1

#define MOTOR_1_PIN         LED1
#define MOTOR_2_PIN         LED2
#define MOTOR_3_PIN         LED3
#define MOTOR_4_PIN         LED4


/************ USBMIDI ************/

#define USE_USBMIDI 1

// default value
#define USBMIDI_FORWARD_TO_MIDI true
#define USBMIDI_FORWARD_TO_NET  true

/************ MIDI ************/

#define USE_MIDI 1

#define MIDI_TX_PIN USBTX
#define MIDI_RX_PIN USBRX

// default value
#define MIDI_FORWARD_TO_USB true
#define MIDI_FORWARD_TO_NET true


/************ NETWORK BASED MIDI ************/

#define USE_NETMIDI 1

// default value
#define NETMIDI_FORWARD_TO_MIDI true
#define NETMIDI_FORWARD_TO_USB  true

#define NET_HOSTNAME "Aggregat-02"
// #define NET_HOSTNAME_MAXLEN 32

// #define LWIP_MDNS_RESPONDER 1

// #define 