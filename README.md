# aggregat controller 02

## Hardware

### dev (philip)

- Base board: [ST NUCLEO-H743ZI2](https://www.st.com/en/evaluation-tools/nucleo-h743zi.html) (also see [@mbed](https://os.mbed.com/platforms/ST-Nucleo-H743ZI2/))
   *note* can possibly be scaled down to a cheaper version
- MIDI-Module: [midi-uart-adapter](https://github.com/tschiemer/midi-uart-adapter), to be tested

## Building

- [Mbed Studio](https://os.mbed.com/studio/) as IDE

Make sure to install all dependencies:

```bash
git submodule update --init --recursive
```

## Configuration options

See file `config.h`
