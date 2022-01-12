
https://nodemcu.readthedocs.io/en/dev-esp32/

## Installatie van firmware

zie: https://nodemcu.readthedocs.io/en/dev-esp32/build/

Gebruik een virtuele environment voor Python 2

```sh
git clone --branch dev-esp32 --recurse-submodules https://github.com/nodemcu/nodemcu-firmware.git nodemcu-firmware-esp32
cd nodemcu-firmware-esp32
pip install Pyserial
pip install -r sdk/esp32-esp-idf/requirements.txt
make menuconfig
make
make flash
```

Zie ook:

```
make help
```

Zie welke modules je hebt gekozen:

```
grep 'CONFIG_NODEMCU_CMODULE.*=y' sdkconfig
```

## Tools

 * [nodemcu-tool](https://www.npmjs.com/package/nodemcu-tool) — terminal, upload/download bestanden
 * [ChiliPeppr](http://chilipeppr.com/esp32) — web-interface

In de directory van je project, zet defaults voor poort en baudrate:

```
nodemcu-tool init
```

Uploaden Lua-bestanden:

```
nodemcu-tool upload *.lua
```

Open terminal:

```
nodemcu-tool terminal
```
