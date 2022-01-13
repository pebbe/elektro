https://mongoose-os.com/docs/mongoose-os/quickstart/setup.md

```sh
mos clone https://github.com/mongoose-os-apps/demo-js demo
cd demo
```

In `mos.yml` waarde voor `"mqtt.server"` aanpassen.

In `mos.yml` toevoegen onder `libs`:
```yaml
  - location: https://github.com/mongoose-os-libs/pwm
```


```sh
mos build --platform esp32
mos flash
mos wifi SSID PASSWORD
mos config-set \
  mqtt.enable=true \
  mqtt.max_qos=1 \
  mqtt.server=rpi-zero-2.fritz.box:1883 \
  mqtt.will_message=---- \
  mqtt.will_retain=true \
  mqtt.will_topic=esp32/demo11/up \
  sntp.server=fritz.box \
  sys.tz_spec=CET-1CEST,M3.5.0/2,M10.5.0/3
mos console
```

## FAIL

In MongooseOS zou je met JavaScript moeten kunnen werken, maar de
implementatie is te gebrekkig. De keywords `var` en `case` zijn niet
geïmplementeerd. Er is geen conversie tussen strings en
getallen. Je kunt niet de inhoud van twee string-variabelen met
elkaar vergelijken. De implementatie van subscribe in MQTT geeft als
message een string zonder einde: blijkbaar verwacht de library dat de
string met een nul-byte aan het einde verzonden worden. Als het een
getal is verzonden als string is er geen mogelijkheid de ontvangen
data weer om te zetten naar een getal.

Je kunt ook in C/C++ werken, maar dan moet je na elke bewerking de
boel op de server laten compileren, en het resultaat flashen. Dan kun
je beter gewoon ArduinoIDE of Espressif gebruiken.
