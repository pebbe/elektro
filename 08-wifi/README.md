## Project 08 — Wifi

### Resources

 * [Arduino Nano 33 IoT](https://docs.arduino.cc/hardware/nano-33-iot)

### Display

 * [u8glib](https://github.com/olikraus/u8glib/wiki)

VMA437 met chip SH1106

| display | Nano |
| ------- | ---- |
| GND     | GND  |
| VCC     | +3V3 |
| CLK (sck) | D4 |
| MOSI (sda) | D5 |
| RES     | RESET |
| DC (a0) | D7   |
| CS (cs) | D6   |


```c
// SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7
U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7);
```
