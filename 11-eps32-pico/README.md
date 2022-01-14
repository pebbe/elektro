
## Project 11 — ESP32: vergelijking van platforms

ESP32 Pico gekocht om de verschillende manieren waarop je die kunt
programmeren te testen. De Pico is smal genoeg om op een enkel
breadboard te gebruiken. Voor grotere ESP32's heb je twee breadboards
naast elkaar nodig.

### Tests

 * Systeeminformatie
 * WiFi
 * NTP
 * localtime Europe/Amsterdam: `CET-1CEST,M3.5.0/2,M10.5.0/3`
 * MQTT
 * Knipperende LEDs instelbaar via MQTT
 * LED met PWM instelbaar via MQTT

#### 1. Arduino IDE

#### 2. Espressif

#### 3. Arduino IDE met RTOS

#### 4. MicroPython

#### 5. NodeMCU

#### 6. Lua RTOS

#### 7. Mongoose OS

#### 8. Zephyr

#### 9. CircuitPython

### Conclusies

Arduino IDE voor eindproducten

NodeMCU voor ontwikkeling / tests

### Vervolg

Tests met

 * Verschillende soorten displays
 * Verschillende soorten sensors




----------------------------------------------------------------


```
ESP32 Chip model = ESP32-PICO-D4 Rev 1
This chip has 2 cores
Chip ID: 8437860
```

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-pico-kit.html

### Platforms

 * Arduino — C++
 * Espressif — C
 * NodeMCU — Lua
     * https://nodemcu.readthedocs.io/en/dev-esp32/
     * http://chilipeppr.com/esp32
 * Lua RTOS — Lua
     * https://github.com/whitecatboard/Lua-RTOS-ESP32/wiki
 * CircuitPython
     * Niet voor ESP32? (wel voor ESP32-S2)
     * https://circuitpython.org/
 * MicroPython — Python
     * https://docs.micropython.org/
     * https://docs.micropython.org/en/latest/esp32/tutorial/
     * https://thonny.org/
     * https://codewith.mu/
 * Mongoose OS — JavaScript, C
     * https://mongoose-os.com/
 * Zephyr
     * https://www.zephyrproject.org/
