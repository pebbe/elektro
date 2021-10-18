## Project 08 — Arduino Nano 33 IoT

### Resources

 * [Arduino Language Reference](https://www.arduino.cc/reference/en/)
 * [Arduino Nano 33 IoT](https://docs.arduino.cc/hardware/nano-33-iot)

### Display

VMA437 met chip SH1106

| display | Nano |
| ------- | ---- |
| GND     | GND  |
| VCC     | +3V3 |
| CLK (sck) | D4 |
| MOSI    | D5 |
| RES     | RESET |
| DC (a0) | D7   |
| CS      | D6   |


 * [u8g2](https://github.com/olikraus/u8g2)
     * [SH1106 128X64_NONAME](https://github.com/olikraus/u8g2/wiki/u8g2setupcpp#sh1106-128x64_noname)

```c
// SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7
U8G2_SH1106_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R2, 4, 5, 6, 7);
```
