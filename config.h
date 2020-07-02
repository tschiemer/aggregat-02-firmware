#include "mbed.h"

/************ FEATURES ************/

// default value 
#define CHANNEL_OFFSET      0
#define CONTROLLER_OFFSET   0

#define ENABLE_SYSTEM_RESET 1

#define ENABLE_CONTROLLER_LOGIC   1

/************ MOTORS ************/

// #define MOTOR_REFRESH_RATE_HZ       60

// #define MOTOR_PULSEWIDTH_MIN_USEC   556
// #define MOTOR_PULSEWIDTH_MAX_USEC   2410

#define MOTOR_REFRESH_RATE_HZ 50

#define MOTOR_PULSEWIDTH_MIN_USEC   0
#define MOTOR_PULSEWIDTH_MAX_USEC   20000000

#define MOTOR_COUNT         1

#define MOTOR_1_PIN         PWM_OUT // PA_15
#define MOTOR_2_PIN         LED2
#define MOTOR_3_PIN         LED3
#define MOTOR_4_PIN         LED4


/************ USBMIDI ************/

#define USE_USBMIDI 1


#define USB_POWER_PIN PA_9
#define USB_CONNECTED_LED LED1

// default value
#define USBMIDI_FORWARD_TO_MIDI true
#define USBMIDI_FORWARD_TO_NET  false

/************ MIDI ************/

#define USE_MIDI 1

#define MIDI_TX_PIN PD_5
#define MIDI_RX_PIN PD_6

#define MIDI_BAUD   31250
// #define MIDI_BAUD   9600

#define MIDI_BITS   8
#define MIDI_PARITY BufferedSerial::None
#define MIDI_STOP   1

// default value
#define MIDI_FORWARD_TO_USB true
#define MIDI_FORWARD_TO_NET true


/************ NETWORK BASED MIDI ************/

#define USE_NETMIDI 1

#define NET_STATUS_LED LED2

// default value
#define NETMIDI_FORWARD_TO_MIDI true
#define NETMIDI_FORWARD_TO_USB  false

#define NET_HOSTNAME_FMT "Aggregat-02-%08x"
#define NET_SERVICENAME_FMT "Aggregat 02 (%08x)"


// #define NET_HOSTNAME_MAXLEN 32

// #define LWIP_MDNS_RESPONDER 1

// #define 