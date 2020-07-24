# aggregat controller 02

https://gitlab.zhdk.ch/aggregat-02/a01

https://aggregat.zhdk.ch/

## Todos

- [x] USBMIDI connectivity
- [x] Classical MIDI (over UART) connectivity
- [x] Ethernet-based networking
  - [x] DHCP or link-local addressing
  - [ ] [mDNS / DNS-SD](https://github.com/tschiemer/minimr) based host/service discovery
     - [x] ~~using hardware id as part of hostname~~
     - [ ] ~~dynamic, simple hostname~~
     - [ ] *Proposed* Use Channel (or specific) setting as part of hostname, ex ch4.aggregat.local or 1.aggregat.local
     - [ ] service???
  - [ ] Control interface TBD
    1. *Proposed* Raw UDP (quasi simple) port (?): 1234 (?)
    2. [OSC](https://github.com/MariadeAnton/oscpack) (relatively simple)
    3. *Postpone (when components are made by pt)* RTPMIDI (complex)
- [ ] **Control Logic** TBD
  - [ ] Configuration using DIP Switch (controller/channel offset) TBD
- [ ] **Motor Control**
- [ ] **Calibration** necessary? how?
- [ ] Hardware: Modules, custom PCB-hat?, case/protection


### Hardware Controls

Possible configuration settings (for evaluation), settable using dip-switch:

Name | Pins | Description | Use?
--- | --- | --- | ---
UsbLed | 1 | Is USBMIDI connected? Blink on activity? |
EthLed | 1 | Is Ethernet connected? Blink on activity? |
Channel | 4 | Which MIDI Channel to use (1-16) |
HostId | 2-4 | Which ID is used as part of hostname? |
Forward X to Y | 9 | Enable forwarding of messages from USB/MIDI/NET to USB/MIDI/NET (incl. echo) |


## Pin Assignment

Pin | Function | Description | Connector
--- | --- | --- | ---
PD8 | DBG SERIAL | TX | CN1 (USB)
PD9 | DBG SERIAL | RX
 |
PD2 | UART | MIDI RX | CN7
PC12 | UART | MIDI TX
 |
D11 (PB5) | PWM | Motor 1 | CN7
D10 (PD14)| PWM | Motor 2
D9 (PD15) | PWM | Motor 3
D6 (PE9)  | PWM | Motor 4 | CN10
D5 (PE9)  | PWM | Motor 5
D3 (PE13) | PWM | Motor 6
D36 (PB10)| PWM | Motor 7
D35 (PB11)| PWM | Motor 8
D33 (PB0) | PWM | *Reserve*
D32 (PA0) | PWM | *Reserve*
 |

### iocontrols

NUCLEO connectors CN9 (+CN8) only.

#### CN1 (iocontrols connector)

 Pin | Function | Description | Nucleo Connector
 --- | --- | --- | ---
3V3 (7) | | | CN 8
GND (11) |
NRST (5) |

#### CN2 (iocontrols connector)

 Pin | Function | Description | Nucleo Connector
 --- | --- | --- | ---
PA3 (1) | GPIO OUT | LED Motor | CN9 (lhs)
PC0 (3) | GPIO OUT | LED Power
PC3 (5) | GPIO IN | MIDI Channel Select 4 (MSB)
PB1 (7) | GPIO IN | MIDI Channel Select 3
PC2 (9) | GPIO IN | MIDI Channel Select 2
PF10 (11) | GPIO IN | MIDI Channel Select 1 (LSB)
PG2 (14) | GPIO IN | Hostname ID 4 (MSB) | CN8 (rhs)
PG3 (16) | GPIO IN | Hostname ID 3
PF2 (17) | GPIO IN | Hostname ID 2 | CN9 (lhs)
PF1 (19) | GPIO IN | Hostname ID 1 (LSB)
PF0 (21) | GPIO IN | USB to MIDI
GND (23) | GND
PD0 (25) | GPIO IN | USB to NET
PD1 (27) | GPIO IN | MIDI to USB
PG0 (29) | GPIO IN | MIDI to NET

#### CN3 (iocontrols connector)

Pin | Function | Description | Nucleo Connector
--- | --- | --- | ---
PD7 (2) | GPIO IN | NET to USB | CN9 (rhs) | CN3
PD6 (4) | GPIO IN | NET to USB
PD5 (6) | GPIO IN | CFG1 (RSRV)
PD4 (8) | GPIO IN | CFG2 (RSRV)
PD3 (10) | TBD | TBD
GND (12) | GND
PE2 (14) | GPIO IN | Button Center
PE4 (16) | GPIO IN | Button Plus1 (RSRV)
PE5 (18) | GPIO IN | Button Plus2 (RSRV)
PE6 (20) | GPIO OUT | LED 11 (USB1)
PE3 (22) | GPIO OUT | LED 12 (USB2)
PF8 (24) | GPIO OUT | LED 21 (MIDI1)
PF7 (26) | GPIO OUT | LED 22 (MIDI2)
PF9 (28) | GPIO OUT | LED 31 (NET1)
PG1 (30) | GPIO OUT | LED 32 (NET2)

## Installation / Flashing

1. Connect the USB cable (programming end / opposite of ethernet plug) to computer.
2. Device should show up as mass storage device (ie external hard disk)
3. Drag & Drop firmware file into external hard disk
   -> big LED should flash red-yellow and then stay yellow - *voilà*

## Known Problems & Limitations

- as of 2020-07-02 ethernet interferes with the USB Device interface causing a permanent USB fail, but only if both are connected *at the same time*. It's a problem of the underlying system which might get resolve d in the future.

## Hardware

### dev (philip)

- Base board: [ST NUCLEO-H743ZI2](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html) (also see [@mbed](https://os.mbed.com/platforms/ST-Nucleo-H743ZI2/))
   *note* can possibly be scaled down to a (marginally) cheaper version
- MIDI-Module: [midi-uart-adapter](https://github.com/tschiemer/midi-uart-adapter)

## Software

Use [Mbed Studio](https://os.mbed.com/studio/) as IDE.

In Mbed Studio workspace run (on command line):

```bash
git clone git@gitlab.zhdk.ch:aggregat-02/a01.git
cd a01
git submodule update --init --recursive
```

As of 2020-07-02 the official USBMIDI of mbed-os has some (reported) bugs, please update these files:
  - [USBMIDI.h](https://github.com/tschiemer/mbed-os/blob/usbmidi-writeplus-fixes/drivers/USBMIDI.h)
  - [USBMIDI.cpp](https://github.com/tschiemer/mbed-os/blob/usbmidi-writeplus-fixes/drivers/source/usb/USBMIDI.cpp)
  - [MIDIMessage.h](https://github.com/tschiemer/mbed-os/blob/usbmidi-writeplus-fixes/drivers/internal/MIDIMessage.h)


### Configuration options

See file `config.h`

## Questions?

- Peter Färber, peter.faerber@zhdk.ch
- Philip Tschiemer, philip.tschiemer@gmail.com

## Copyfright 2020 ICST / ZHdK
