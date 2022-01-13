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
mos config-set mqtt.enable=true mqtt.server=rpi-zero-2.fritz.box:1883 sntp.server=fritz.box
mos console
```
