-- init.lua

secret = require("secret")

dag = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" }
maand = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" }

tz = {}
tz[0] = "CET"
tz[1] = "CEST"
tz[-1] = ""

time.settimezone("CET-1CEST,M3.5.0/2,M10.5.0/3")

-- LEDs

gpio.config({gpio={25, 26, 27}, dir=gpio.OUT})

led_channel = ledc.newChannel({
  gpio=27,
  bits=ledc.TIMER_10_BIT,
  mode=ledc.LOW_SPEED,
  timer=ledc.TIMER_0,
  channel=ledc.CHANNEL_0,
  frequency=500,
  duty=1023
});

led_on = false
led_timer = tmr.create()
led_timer:alarm(1000, tmr.ALARM_AUTO, function()
  led_on = not led_on
  if led_on then
    gpio.write(25, 1)
    gpio.write(26, 0)
  else
    gpio.write(25, 0)
    gpio.write(26, 1)
  end
end)

-- Systeeminfo

print('============')
print('nodemcu/demo')
print('============')

print(_VERSION)
print("Heap: "..node.heap())

-- WiFi

cfg = {}
cfg.ssid = secret.ssid
cfg.pwd = secret.password
cfg.auto = false
cfg.save = false
wifi.start()
wifi.mode(wifi.STATION)
wifi.sta.config(cfg)
wifi.sta.on("connected", function(event, info)
  time.initntp("fritz.box")
end)
wifi.sta.connect()

-- MQTT

topic_up = "esp32/demo11/up"
topic_blink = "esp32/demo11/blink"
topic_level = "esp32/demo11/level"

mqtt_client = mqtt.Client("id11", 120)
mqtt_client:lwt(topic_up, "----", 1, 1)
mqtt_client:on("connect", function(client)
  local t = time.getlocal()
  local s = string.format("%02d-%02d-%04d %02d:%02d:%02d", t["day"], t["mon"], t["year"], t["hour"], t["min"], t["sec"])
  client:subscribe(topic_blink, 1)
  client:subscribe(topic_level, 1)
  client:publish(topic_up, s, 1, 1)
end)
mqtt_client:on("message", function(client, topic, data)
  local value = tonumber(data)
  if topic == topic_blink then
    if value < 1 then value = 1 end
    led_timer:interval(value * 100)
  elseif topic == topic_level then
    if value < 0 then value = 0 end
    if value > 255 then value = 255 end
    led_channel:setduty(value * 4)
  end
end)

-- wacht tot time.get() de juiste tijd geeft voordat MQTT gestart wordt

setup_timer = tmr.create()
setup_timer:alarm(100, tmr.ALARM_SEMI, function()
  if time.get() > 100000000 then
    mqtt_client:connect("rpi-zero-2.fritz.box", 1883)
    setup_timer:unregister()
  else
    setup_timer:start()
  end
end)

-- local time

time_timer = tmr.create()
time_timer:alarm(20000, tmr.ALARM_AUTO, function()
  local t = time.getlocal()
  print(string.format("%s %d %s %d, %2d:%02d:%02d %s", dag[t["wday"]], t["day"], maand[t["mon"]], t["year"], t["hour"], t["min"], t["sec"], tz[t["dst"]]))
end)

-- end
