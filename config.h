#include "mbed.h"

/************ FEATURES ************/

// MIDI channel (0-15) selection through 4 active high switches
#define USE_CHANNEL_SELECT      0

#if USE_CHANNEL_SELECT
#define CHANNEL_SELECT_PIN_1    PC_3   // LSB
#define CHANNEL_SELECT_PIN_2    PB_1
#define CHANNEL_SELECT_PIN_3    PC_2
#define CHANNEL_SELECT_PIN_4    PF_10    // MSB
#endif

// default value (iff USE_CHANNEL_SELECT == 0)
#define CHANNEL_SELECT_DEFAULT  0


// device id (0-15) selection (usb device name + hostname)
#define USE_DEVICE_ID_SELECT           0

#if USE_DEVICE_ID_SELECT
#define DEVICE_ID_SELECT_PIN_1         PF_1   // LSB
#define DEVICE_ID_SELECT_PIN_2         PF_2
#define DEVICE_ID_SELECT_PIN_3         PG_2
#define DEVICE_ID_SELECT_PIN_4         PG_3   // MSB
#endif

// default value (iff USE_DEVICE_ID_SELECT == 0)
#define DEVICE_ID_SELECT_DEFAULT       0

#define USE_FORWARDING_CONTROLS      0

#if USE_FORWARDING_CONTROLS
#define USB_TO_MIDI_PIN             PF_0
#define USB_TO_NET_PIN              PD_0
#define MIDI_TO_USB_PIN             PD_1
#define MIDI_TO_NET_PIN             PG_0
#define NET_TO_USB_PIN              PD_7
#define NET_TO_MIDI_PIN             PD_6
#endif //USE_FORWARDING_CONTROLS

#define USB_TO_MIDI_DEFAULT         true
#define USB_TO_NET_DEFAULT          true
#define MIDI_TO_USB_DEFAULT         true
#define MIDI_TO_NET_DEFAULT         true
#define NET_TO_MIDI_DEFAULT         true
#define NET_TO_USB_DEFAULT          true

// not used at this time
#define USE_CFG 0

#if USE_CFG
#define CFG_RSRV_1_PIN                  PD_5
#define CFG_RSRV_2_PIN                  PD_4
#endif //USE_CFG

#define USE_BUTTONS 0

#if USE_BUTTONS
#define BTN_CENTER_PIN                  PE_2
#define BTN_PLUS_1_PIN                  PE_4
#define BTN_PLUS_2_PIN                  PE_5
#endif //USE_BUTTONS

#define USE_STATUS_LEDS 1

#if USE_STATUS_LEDS
#define LED_PWR_PIN         PC_0
#define LED_MTR_PIN         PA_3
#define LED_USB_1_PIN       LED1 //PE_6
#define LED_USB_2_PIN       LED2 //PE_3
#define LED_MIDI_1_PIN      PF_8
#define LED_MIDI_2_PIN      PF_7
#define LED_NET_1_PIN       PF_9
#define LED_NET_2_PIN       PG_1
#endif //USE_STATUS_LEDS

// base offset of CC controls (if chosen badly can cause interference with standard CCs)
#define CC_CONTROLLER_OFFSET            102


// turn on/off remote core controls 
#define USE_CONTROLLER_LOGIC         1

// allow system resets with MIDI reset
#define USE_SYSTEM_RESET             1

#define USE_PITCHBEND_CONTROL        1
#define USE_NRPN_CONTROL             1

/************ MOTORS ************/

#define MOTOR_REFRESH_RATE_HZ       50

#define MOTOR_PULSEWIDTH_MIN_USEC   760
#define MOTOR_PULSEWIDTH_MAX_USEC   2240

#define MOTOR_PWR_AT_STARTUP 1

#define MOTOR_COUNT         2


// PWM out pins
#define MOTOR_1_PWM_PIN         PC_8
#define MOTOR_2_PWM_PIN         PC_9
#define MOTOR_3_PWM_PIN         PA_6
#define MOTOR_4_PWM_PIN         PB_5
#define MOTOR_5_PWM_PIN         PB_3
#define MOTOR_6_PWM_PIN         PD_14
#define MOTOR_7_PWM_PIN         PD_15
#define MOTOR_8_PWM_PIN         PC_8 //PE_9
#define MOTOR_9_PWM_PIN         PE_11
#define MOTOR_10_PWM_PIN        PE_14
#define MOTOR_11_PWM_PIN        PE_13
#define MOTOR_12_PWM_PIN        PB_6
#define MOTOR_13_PWM_PIN        PB_7
#define MOTOR_14_PWM_PIN        PA_0
#define MOTOR_15_PWM_PIN        PB_10
#define MOTOR_16_PWM_PIN        PB_11

// GPIO power on 
#define MOTOR_1_PWR_PIN         PC_6
#define MOTOR_2_PWR_PIN         PC_6
#define MOTOR_3_PWR_PIN         PC_7
#define MOTOR_4_PWR_PIN         PC_7
#define MOTOR_5_PWR_PIN         PB_3
#define MOTOR_6_PWR_PIN         PB_3
#define MOTOR_7_PWR_PIN         PB_4
#define MOTOR_8_PWR_PIN         PB_4
#define MOTOR_9_PWR_PIN         PF_5
#define MOTOR_10_PWR_PIN        PF_5
#define MOTOR_11_PWR_PIN        PC_6//PB_2
#define MOTOR_12_PWR_PIN        PC_6//PB_2
#define MOTOR_13_PWR_PIN        PC_6// PE_2
#define MOTOR_14_PWR_PIN        PC_6//PE_2
#define MOTOR_15_PWR_PIN        PE_0
#define MOTOR_16_PWR_PIN        PE_0


/************ USBMIDI ************/

#define USE_USBMIDI         1

#define USB_POWER_PIN       PA_9
// #define USB_CONNECTED_LED   PA_3 //A3 //LED1

/************ MIDI ************/

#define USE_MIDI    1

#define MIDI_TX_PIN PC_12// PC_10 //PC_12 //PD_5
#define MIDI_RX_PIN PD_2 //PC_11 //PD_2 //PD_6

#define MIDI_BAUD   31250

#define MIDI_BITS   8
#define MIDI_PARITY BufferedSerial::None
#define MIDI_STOP   1

/************ NETWORK BASED MIDI ************/

#define USE_NETMIDI             1

// #define NET_STATUS_LED          LED2

#define NET_HOSTNAME_FMT        ".%d.Aggregat.local"
// #define NET_SERVICENAME_FMT     ".%d.Aggregat???.local"

// required records: A or AAAA
#define MDNS_RR_COUNT           1


// #define 