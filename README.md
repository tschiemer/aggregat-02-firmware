# aggregat controller 02

Firmware for a NUCLEO-H743ZI2 board to allow for MIDI based control (DIN5, USB-MIDI and (future) RTPMIDI)
of 16 PWM controlled servo motors.

Created for setups of custom made, motorized AGGREGAT loudspeakers at ICST.

https://aggregat.zhdk.ch/

https://gitlab.zhdk.ch/aggregat-02/a01-firmware
https://gitlab.zhdk.ch/aggregat-02/a01-pcb-iocontrols

## Todos

- [ ] Sytem testing & revision
- [x] Ethernet-based networking
  - [x] DHCP or link-local addressing
  - [ ] [mDNS / DNS-SD](https://github.com/tschiemer/minimr) based host/service discovery
     - [x] ~~using hardware id as part of hostname~~
     - [ ] ~~dynamic, simple hostname~~
     - [ ] *Proposed* Use Channel (or specific) setting as part of hostname, ex   ch4.aggregat.local or 1.aggregat.local
     - [ ] service???
  - [ ] Control interface TBD
    1. *Proposed* Raw UDP (quasi simple) port (?): 1234 (?)
    2. [OSC](https://github.com/MariadeAnton/oscpack) (relatively simple)
    3. *Postpone (when components are made by pt)* RTPMIDI (complex)


## Installation / Flashing

1. Connect the USB cable (programming end / opposite of ethernet plug) to computer.
2. Device should show up as mass storage device (ie external hard disk)
3. Drag & Drop firmware file into external hard disk
   -> big LED should flash red-yellow and then stay yellow - *voilà*

## Known Problems & Limitations

- as of 2020-07-02 ethernet interferes with the USB Device interface causing a permanent USB fail, but only if both are connected *at the same time*. It's a problem of the underlying system which might get resolve d in the future.

## MIDI Control

**IMPORTANT NOTE: MIDI channels and controller number etc might have a range of 1-16, 1-128 respectively, but actual data being sent it 0-15, 0-127 respectively. So keep this in mind when reading this documentation and when trying to configure any device.**

Also see demo Max/MSP in `max`.

Please understand the following basic settings/definitions:

- `<midi-channel>` is defined through 4-Pin DIP-Switch (binary setting of values 0 - 15, ie MIDI Channels 1 - 16)

- `<cc-offset> := 102`


### General Control

System real-time messages do not use a channel, thus they affect *all* connected devices (also see https://www.midi.org/specifications-old/item/table-1-summary-of-midi-message)

- `RESET` ( 0b11111111 )

  resets all devices (ie all devices restart)

- `STOP` ( 0b11111100 )

  suspend all motor functions

- `START` ( 0b11111101 ) or `CONTINUE` ( 0b11111011 )

  continue (unsuspend) all motor functions

### Motor 0 - (N-1) where N := the number of motors

Turn on/off power of motor M (0 - (N-1)):

    Note on/off <note := M> <velocity (any)>

Set motor M position to P (0 - 127):

    CC <midi-channel> <controller := <cc-offset> + M> <value := P>

or when using NRPN, then P (0 - 16383):

    NRPN <midi-channel> <controller := M> <value := P>

So, to set the 9th motor M := 8 to center position P := 127/2 or 16383/2 on device listening to *MIDI* channel 4 you would do..

    CC 3 110 64

or

    NRPN 3 8 8192

## Hardware Controls

![iocontrols board](https://gitlab.zhdk.ch/aggregat-02/a01-firmware/-/raw/master/docs/iocontrols-labelled.png)


| What | Function |
| --- | --- |
| Button left corner | Hard reset |
| 4-pin DIP Switch left | MIDI channel select (binary MSB-LSB) |
| 4-pin DIP Switch right | Device id select (binary MSB-LSB) |
| 8-pin DIP Switch: Pin 1 | Forward USBMIDI to MIDI? |
| 8-pin DIP Switch: Pin 2 | Forward USBMIDI to RTPMIDI? |
| 8-pin DIP Switch: Pin 3 | Forward MIDI to USBMIDI? |
| 8-pin DIP Switch: Pin 4 | Forward MIDI to RTPMIDI? |
| 8-pin DIP Switch: Pin 5 | Forward RTPMIDI to USBMIDI? |
| 8-pin DIP Switch: Pin 6 | Forward RTPMIDI to MIDI? |
| 8-pin DIP Switch: Pin 7 | HIGH -> Suspend motors on startup, LOW -> run motors |
| 8-pin DIP Switch: Pin 8 | Enable motor power control (MIDI commands are processed) *currently feature not implemented in hardware* |
| Three buttons left | Set all motors to center position |
| Three buttons middle | *not implemented* |
| Three buttons right | Suspend / Resume all motors |
| |
| Green LED left bottom | System has power |
| Red LED left top | Motors are either all suspended or at least one motor's power is off |
| LED


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

## Hardware

- Base board: [ST NUCLEO-H743ZI2](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html) (also see [@mbed](https://os.mbed.com/platforms/ST-Nucleo-H743ZI2/))
   *note* can possibly be scaled down to a (marginally) cheaper version
- IOControls: https://gitlab.zhdk.ch/aggregat-02/a01-pcb-iocontrols
- Aggregat core board: https://gitlab.zhdk.ch/aggregat-02/a01-pcb-core

### Bill of Materials

For BOM of custom PCBs see respective repository.

Number | Item | Cost per (10/100) Unit(s)  | Cost
--- | --- | --- | ---
1 | [ST NUCLEO-H743ZI2](https://www.digikey.ch/product-detail/en/stmicroelectronics/NUCLEO-H743ZI2/497-19452-ND/10130892) (Base Board) | 25.65 | 25.65
1 | [IOControls board](https://gitlab.zhdk.ch/aggregat-02/a01-pcb-iocontrols) | ~20.00 | 20.00
1 | [UART - MIDI adapter board](https://github.com/tschiemer/midi-uart-adapter) | 10.0 | 10.00
| |
1 | [USB 2.0 B Connector](https://www.digikey.ch/product-detail/en/amphenol-icc-commercial-products/MUSB-D511-00/MUSB-D511-00-ND/2567127) | 9.56 | 9.56
3 | [DIN 5-pol (MIDI) Connector](https://www.digikey.ch/product-detail/en/cui-devices/SD-50LS/CP-1235-ND/500833) | 4.17 | 12.52
1 | [AC-DC Converter 5V 14A](https://www.digikey.ch/product-detail/en/mean-well-usa-inc/LRS-75-5/1866-3364-ND/7705056) | 17.25 | 17.25
1 | [USB Power Adapter](https://www.digikey.ch/product-detail/en/cui-inc/SWI3-5-E-I38/102-4482-ND/7784533) | 4.94 | 4.94
3 | [30 pin ribbon cable connector](https://www.digikey.ch/product-detail/en/SFH210-PPPC-D15-ID-BK/S9292-ND/2095301) (IOControls) | 1.06 | 3.18
1 | [34 pin ribbon cable connector](https://www.digikey.ch/product-detail/en/sullins-connector-solutions/SFH210-PPPC-D17-ID-BK/S9293-ND/2095302) | 0.94 | 0.94
1 | [20 pin ribbon cable connector](https://www.digikey.ch/product-detail/en/sullins-connector-solutions/SFH210-PPPC-D10-ID-BK/S9289-ND/2095298) | 0.72 | 0.72
2 | [34 wire 28 AWG ribbon cable (example)](https://www.digikey.ch/product-detail/en/cicoil/310J101-34/CI34N-10-ND/3524331) (IOControls) | 0.40 | 0.80
| | Headers: [1](https://www.digikey.ch/product-detail/en/PREC040SABN-RC/S1022EC-40-ND/2775014) and/or [2](https://www.digikey.ch/product-detail/en/PRPC032DABN-RC/S2021EC-32-ND/2775895) | | < 5.00
8 | [4 pin motor connector](https://www.digikey.ch/product-detail/en/0469930410/WM16259-ND/5116019) | 0.57 | <
1 | [Power plug w/ switch](https://www.digikey.ch/product-detail/en/schurter-inc/DC11-0001-001/486-1073-ND/641579) | 13.08 | 13.08

Total (material) cost ~ 120.-

### Pin Assignment

![Pin assignment nucleo LHS](https://gitlab.zhdk.ch/aggregat-02/a01-firmware/-/raw/master/docs/pin-assignment-nucleo-lhs.png)
![Pin assignment nucleo RHS](https://gitlab.zhdk.ch/aggregat-02/a01-firmware/-/raw/master/docs/pin-assignment-nucleo-rhs.png)


Pin | Function | Description | Connector
--- | --- | --- | ---
PC10 (6) | DBG SERIAL | TX | CN1 (USB)
PC11 (8) | DBG SERIAL | RX
| |
PC12 (10) | UART | MIDI TX | CN7
PD2 (12) | UART | MIDI RX
| |
PC8 (2) | PWM3/3 | Motor 1 (spkr 1) Control | CN8
PC9 (4) | PWM3/4 | Motor 2 (spkr 1) Control
PC6 (1) | GPIO OUT | Motor 1/2 Power | CN7
PA15 (9) | GPIO OUT | Motor 3/4 Power
PC7 (11) | GPIO OUT | Motor 5/6 Power
PA6 (12) | PWM3/1 | Motor 3 (spkr 2) Control
PB5 (14) | PWM3/2 | Motor 4 (spkr 2) Control
PB3 (15) | PWM2/2 | Motor 5 (spkr 3) Control
PD14 (16) | PWM4/3 | Motor 6 (spkr 3) Control
PD15 (18) | PWM4/4 | Motor 7 (spkr 4) Control
PB4 (19) | GPIO OUT | Motor 7/8 Power
PE9 (4) | PWM1/1 | Motor 8 (spkr 4) Control | CN10
PE11 (6) | PWM1/2 | Motor 9 (spkr 5) Control
PE14 (8) | PWM1/4 | Motor 10 (spkr 5) Control
PF5 (9) | GPIO OUT | Motor 9/10 Power
PE13 (10) | PWM1/3 | Motor 11 (spkr 6) Control
PB6 (14) | PWM4/1 | Motor 12 (spkr 6) Control
PB2 (15) | GPIO OUT | Motor 11/12 Power
PB7 (16) | PWM4/2 | Motor 13 (spkr 7) Control
PD11 (23) | GPIO OUT | Motor 13/14 Power
PA0 (29) | PWM2/1 | Motor 14 (spkr 7) Control
PB10 (32) | PWM2/3 | Motor 15 (spkr 8) Control
PE0 (33) | GPIO OUT | Motor 15/16 Power
PB11 (34) | PWM2/4 | Motor 16 (spkr 8) Control

#### iocontrols

NUCLEO connectors CN9 (+CN8) only.

##### CN1 (iocontrols connector)

Pin | Function | Description | Nucleo Connector
--- | --- | --- | ---
3V3 (7) | | | CN 8
GND (11) |
NRST (5) |

##### CN2 (iocontrols connector)

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

##### CN3 (iocontrols connector)

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



## Questions?

- Peter Färber, peter.faerber (@) zhdk.ch
- Philip Tschiemer, [filou.se](https://filou.se)


## License

Copyfright (c) 2020 Institute for Computer Music and Sound Technology (ICST), ZHdK Zurich, <https://www.zhdk.ch/forschung/icst>

GNU Lesser General Public License v3.0

Also see `LICENSE.txt` file.
