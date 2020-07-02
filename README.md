# aggregat controller 02

https://gitlab.zhdk.ch/aggregat-02/a01
https://aggregat.zhdk.ch/


## Hardware

### dev (philip)

- Base board: [ST NUCLEO-H743ZI2](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html) (also see [@mbed](https://os.mbed.com/platforms/ST-Nucleo-H743ZI2/))
   *note* can possibly be scaled down to a (marginally) cheaper version
- MIDI-Module: [midi-uart-adapter](https://github.com/tschiemer/midi-uart-adapter), to be tested

## Software

- [Mbed Studio](https://os.mbed.com/studio/) as IDE

Make sure to install all dependencies:

```bash
git submodule update --init --recursive
```

### Problems

- as of 2020-07-02 the official USBMIDI of mbed-os has some (reported) bugs, please update these files:
  - [USBMIDI.h](https://github.com/tschiemer/mbed-os/blob/usbmidi-writeplus-fixes/drivers/USBMIDI.h)
  - [USBMIDI.cpp](https://github.com/tschiemer/mbed-os/blob/usbmidi-writeplus-fixes/drivers/source/usb/USBMIDI.cpp)
  - [MIDIMessage.h](https://github.com/tschiemer/mbed-os/blob/usbmidi-writeplus-fixes/drivers/internal/MIDIMessage.h)

## Configuration options

See file `config.h`
