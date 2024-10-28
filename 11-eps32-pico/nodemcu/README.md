
## Firmware installeren

```
cd .../nodemcu-firmware-esp32
make menuconfig    # Component config -> NodeMCU modules
make
idf.py -p /dev/ttyUSB0 flash
```

Zie selectie van modules:

```
grep 'CONFIG_NODEMCU_CMODULE.*=y' sdkconfig
```

Nodig voor dit project:

```
CONFIG_NODEMCU_CMODULE_FILE=y
CONFIG_NODEMCU_CMODULE_GPIO=y
CONFIG_NODEMCU_CMODULE_I2C=y
CONFIG_NODEMCU_CMODULE_LEDC=y
CONFIG_NODEMCU_CMODULE_MQTT=y
CONFIG_NODEMCU_CMODULE_NET=y
CONFIG_NODEMCU_CMODULE_NODE=y
CONFIG_NODEMCU_CMODULE_OW=y
CONFIG_NODEMCU_CMODULE_PIPE=y
CONFIG_NODEMCU_CMODULE_TMR=y
CONFIG_NODEMCU_CMODULE_WIFI=y
CONFIG_NODEMCU_CMODULE_TIME=y
CONFIG_NODEMCU_CMODULE_UART=y
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
