#include "mbed.h"

/************ FEATURES ************/


#define USE_CHANNEL_SELECT      1
#define CHANNEL_SELECT_PIN_1    PC_11   // LSB
#define CHANNEL_SELECT_PIN_2    PC_10
#define CHANNEL_SELECT_PIN_3    PC_9
#define CHANNEL_SELECT_PIN_4    PC_8    // MSB

// default value (iff USE_CHANNEL_SELECT == 0)
#define CHANNEL_SELECT_DEFAULT  0


#define USE_DEVICE_ID_SELECT           1
#define DEVICE_ID_SELECT_PIN_1         A0   // LSB
#define DEVICE_ID_SELECT_PIN_2         A1
#define DEVICE_ID_SELECT_PIN_3         A2
#define DEVICE_ID_SELECT_PIN_4         A3   // MSB

// default value (iff USE_DEVICE_ID_SELECT == 0)
#define DEVICE_ID_SELECT_DEFAULT       1


// base offset of CC controls (if chosen badly can cause interference with standard CCs)
#define CC_CONTROLLER_OFFSET            102


// turn on/off remote core controls 
#define ENABLE_CONTROLLER_LOGIC         1

// allow system resets with MIDI reset
#define ENABLE_SYSTEM_RESET             1

#define ENABLE_PITCHBEND_CONTROL        1
#define ENABLE_NRPN_CONTROL             1


#define USE_FORWARDING_CONTROLS         1

#define USB_TO_MIDI_PIN             A0
#define USB_TO_NET_PIN              A0
#define MIDI_TO_USB_PIN             A0
#define MIDI_TO_NET_PIN             A0
#define NET_TO_USB_PIN              A0
#define NET_TO_MIDI_PIN             A0

#define USB_TO_MIDI_DEFAULT         true
#define USB_TO_NET_DEFAULT          false
#define MIDI_TO_USB_DEFAULT         true
#define MIDI_TO_NET_DEFAULT         true
#define NET_TO_MIDI_DEFAULT         true
#define NET_TO_USB_DEFAULT          false

/************ MOTORS ************/

#define MOTOR_REFRESH_RATE_HZ       50

#define MOTOR_PULSEWIDTH_MIN_USEC   760
#define MOTOR_PULSEWIDTH_MAX_USEC   2240

#define MOTOR_COUNT         8

#define MOTOR_1_PIN         D11
#define MOTOR_2_PIN         D10
#define MOTOR_3_PIN         D9
#define MOTOR_4_PIN         D6
#define MOTOR_5_PIN         D5
#define MOTOR_6_PIN         D3
#define MOTOR_7_PIN         PB_10
#define MOTOR_8_PIN         PB_11


/************ USBMIDI ************/

#define USE_USBMIDI 1

#define USB_POWER_PIN PA_9
#define USB_CONNECTED_LED A3 //LED1

/************ MIDI ************/

#define USE_MIDI 0

#define MIDI_TX_PIN PD_5
#define MIDI_RX_PIN PD_6

#define MIDI_BAUD   31250

#define MIDI_BITS   8
#define MIDI_PARITY BufferedSerial::None
#define MIDI_STOP   1

/************ NETWORK BASED MIDI ************/

#define USE_NETMIDI 1

#define NET_STATUS_LED LED2

#define NET_HOSTNAME_FMT ".%d.Aggregat.local"
#define NET_SERVICENAME_FMT ".%d.Aggregat???.local"

// required records: A or AAAA
#define MDNS_RR_COUNT 1


// #define 