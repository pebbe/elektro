
## Firmware installeren

```
cd .../nodemcu-firmware-esp32
make menuconfig    # Component config -> NodeMCU modules
# zie selectie van modules:
grep 'CONFIG_NODEMCU_CMODULE.*=y' sdkconfig
make
idf.py -p /dev/ttyUSB0 flash
```

## Upload script

Zet defaults voor poort en baudrate:

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


Meer tools voor uploaden van script:

https://nodemcu.readthedocs.io/en/release/upload/

NodeMCU-Tool is de enige tool die lijkt te werken.
