

 * [MicroPython documentation](https://docs.micropython.org/en/latest/)
 * [MicroPython Tutorial](https://www.youtube.com/playlist?list=PL6F17pWypPy_KSmpnR5CV8x78QhAiKBIl) (Youtube)

## Install

 * [download](https://micropython.org/download/)

```
esptool.py --port /dev/ttyUSB0 erase_flash
esptool.py --chip esp32 --port /dev/ttyUSB0 write_flash -z 0x1000 esp32-*.bin
```

## Editors

 * [mu-editor](https://codewith.mu/)
 * ~~[thonny](https://thonny.org/)~~
 
## Command line access + REPL

 * [rshell](https://github.com/dhylands/rshell)

In rshell:

```sh
cp *.py /pyboard
repl             # exit: Ctrl-X
```
