
Download `wcc` van
https://github.com/whitecatboard/Lua-RTOS-ESP32#method-1-get-a-precompiled-firmware

```
./wcc -p /dev/ttyUSB0 -f -ffs

./wcc -p /dev/ttyUSB0 -ls .
./wcc -p /dev/ttyUSB0 -down config.lua config.lua
./wcc -p /dev/ttyUSB0 -down autorun.lua autorun.lua
edit autorun.lua
./wcc -p /dev/ttyUSB0 -up autorun.lua autorun.lua

./wcc -p /dev/ttyUSB0 -ls examples/lua
mkdir tmp
./wcc -p /dev/ttyUSB0 -down examples/lua/mqtt.lua tmp/mqtt.lua
```

Let op:

Je kunt `wcc` niet gebruiken als een ander programma (zoals een monitor)
verbonden is aan de poort.


https://github.com/whitecatboard/Lua-RTOS-ESP32/wiki

https://github.com/whitecatboard/Lua-RTOS-ESP32/wiki/Firmware-Espressif-Systems-ESP32-PICO-KIT

Let op:

 * `tmr.delay()` blokkeert in eigen thread
 * `tmr.sleep()` geeft tijd aan andere threads
 * als `autorun.lua` het einde bereikt worden alle threads gestopt

Beperkingen:

 * mqtt zonder last will & testament
 * geen tijdzones
