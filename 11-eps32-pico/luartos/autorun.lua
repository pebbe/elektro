-- autorun.lua

time = require("time")

-- system info ----------

print("============")
print("luartos/demo")
print("============")

print("")
print("CPU: "..cpu.model())
print("Board: "..cpu.board())
print("Speed: "..cpu.speed().." MHz")
print("Temp: "..cpu.temperature().." ^C")
print("")

thread.start(function()
  while true do
    print(time.longtime())
    tmr.sleep(20)
  end
end)

-- blink ----------

pio.pin.setdir(pio.OUTPUT, pio.GPIO25)
pio.pin.setdir(pio.OUTPUT, pio.GPIO26)

led_on = false
function blink()
  led_on = not led_on
  if led_on then
    pio.pin.sethigh(pio.GPIO25)
    pio.pin.setlow(pio.GPIO26)
  else
    pio.pin.setlow(pio.GPIO25)
    pio.pin.sethigh(pio.GPIO26)
  end
end

t0 = tmr.attach(1000, blink)
t0:start()

-- PWM ----------

led = pwm.attach(pio.GPIO27, 5000, .5)
led:start()

-- MQTT ----------

topic_up = "esp32/demo11/up"
topic_blink = "esp32/demo11/blink"
topic_level = "esp32/demo11/level"

function handler(len, message, topic_len, topic_name)
  print("MQTT: "..topic_name.."="..message)
  value = tonumber(message)
  if topic_name == topic_blink then
    if value < 1 then
      value = 1
    end
    t0:detach()
    t0 = tmr.attach(value * 100, blink)
    t0:start()
  elseif topic_name == topic_level then
    if value < 0 then
      value = 0
    end
    if value > 255 then
      value = 255
    end
    led:setduty(value / 255)
  end
end

client = mqtt.client("id11", "192.168.178.24", 1883, false, nil, true)
client:connect("","")
while true do
  if client:connected() then
    break
  end
  tmr.sleepms(100)
end
client:subscribe(topic_blink, mqtt.QOS1, handler)
client:subscribe(topic_level, mqtt.QOS1, handler)
while true do
  -- wacht tot juiste tijd van NTP beschikbaar is
  if os.time() > 1641911053 then
    client:publish(topic_up, time.shorttime(), mqtt.QOS1, true)
    break
  end
  tmr.sleepms(100)
end

