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


## Pinout

Pin | Description
--- | ---
D11 (PB5) | Motor 1
D10 (PD14)| Motor 2
D9 (PD15) | Motor 3
D6 (PE9)  | Motor 4
D5 (PE9)  | Motor 5
D3 (PE13) | Motor 6
D36 (PB10)| Motor 7
D35 (PB11)| Motor 8
D33 (PB0) | Reserve
D32 (PA0) | Reserve

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
