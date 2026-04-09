# F1C100S [CoreMark](https://www.eembc.org/coremark/)

F1C100S performance on bare metal:
```
Allwinner F1C100S Coremark
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 19276
Total time (secs): 19.276000
Iterations/Sec   : 1296.949575
Iterations       : 25000
Compiler version : GCC10.3.1 20210824 (release)
Compiler flags   : -O3
Memory location  : STACK
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0xcc42
Correct operation validated. See README.md for run and reporting rules.
CoreMark 1.0 : 1296.949575 / GCC10.3.1 20210824 (release) -O3 / STACK
```
Comparison with other popular chips:

| Chip                                                                   | Freq (MHz) | CoreMark    |
|------------------------------------------------------------------------|------------|-------------|
| [F1C100S](./)                                                          | 576        | 1296        |
| [V3S](https://github.com/minilogic/v3s_nonos/tree/main/src/coremark)   | 1008       | 3102        |
| [iMXRT1062](https://www.pjrc.com/store/teensy40_pins.html)             | 600        | 2314        |
| [ESP32 S2](https://github.com/ochrin/coremark)                         | 80/160/240 | 157/315/472 |
| [ESP8266](https://github.com/ochrin/coremark)                          | 80         | 191         |
| [nRF52840](https://infocenter.nordicsemi.com/pdf/nRF52840_PS_v1.1.pdf) | 64         | 212         |
| [STM32F427](https://github.com/hacklabos/CoremarkPlatform)             | 180        | 258         |
| [STM32F103](https://github.com/hacklabos/CoremarkPlatform)             | 72/128     | 95/146      |
| [RP2040](https://github.com/nickfox-taterli/pico-coremark)             | 125        | 157         |
