#!/bin/bash

while :
do
    set `free -m | fgrep Mem`
    total=$2
    free=$4
    avail=$7
    mosquitto_pub -r -t raspi/time -m "`date '+%H:%M'`"
    sleep 2
    mosquitto_pub -r -t raspi/mem/free -m "${free}Mi ($[ ($free * 100 + 50) / $total ]%)"
    sleep 2
    mosquitto_pub -r -t raspi/mem/avail -m "${avail}Mi ($[ ($avail * 100 + 50) / $total ]%)"
    sleep 2
    mosquitto_pub -r -t raspi/uptime -m "`uptime | perl -p -e 's/.* up (.*),\s+\S+\s+user.*/$1/'`"
    sleep 2
    mosquitto_pub -r -t raspi/load -m "`uptime | perl -p -e 's/.*\s+(\S+\s+\S+\s+\S+)/$1/'`"
    sleep 2
    mosquitto_pub -r -t raspi/temp/cpu -m "`perl -p -e 's!.*!sprintf("%.1f°C", $& / 1000.0)!e' /sys/class/thermal/thermal_zone0/temp`"
    sleep 2
    mosquitto_pub -r -t raspi/temp/gpu -m "`vcgencmd measure_temp | perl -p -e 's/.*?([0-9][0-9.]*).*/$1°C/'`"
    sleep 48
done
