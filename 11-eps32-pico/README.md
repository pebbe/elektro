
## Project 11 — ESP32: vergelijking van platforms

```
ESP32 Chip model = ESP32-PICO-D4 Rev 1
This chip has 2 cores
Chip ID: 8437860
```

https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-pico-kit.html

### Platforms

 * Arduino — C++
 * Espressif — C
     * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-pico-kit.html
 * NodeMCU — Lua
     * https://nodemcu.readthedocs.io/en/dev-esp32/
     * https://nodemcu.readthedocs.io/
     * http://chilipeppr.com/esp32
 * Lua RTOS — Lua
     * https://github.com/whitecatboard/Lua-RTOS-ESP32/wiki
 * MicroPython — Python
     * https://docs.micropython.org/
     * https://docs.micropython.org/en/latest/esp32/tutorial/
     * https://thonny.org/
     * https://codewith.mu/
 * Mongoose OS — JavaScript, C
     * https://mongoose-os.com/

### Taken

Voor elk platform, iets wat volgende dingen tegelijk doet:

 * Bij opstarten: systeeminfo naar seriële poort sturen
 * Tijd ophalen via NTP, omzetten naar locale tijd, en naar seriële
   poort sturen
 * Twee externe LEDs om de beurt knipperen
 * Een externe LED feller en zwakker laten branden met PWM
 * Berichten versturen via mqtt
 * Reageren op berichten ontvangen via mqtt: knippersnelheid van LEDs aanpassen


Timezone Amsterdam: `CET-1CEST,M3.5.0/2,M10.5.0/3`
