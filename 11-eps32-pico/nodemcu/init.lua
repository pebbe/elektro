secret = require("secret")
cfg = {}
cfg.ssid = secret.ssid
cfg.pwd = secret.password
cfg.auto = false
cfg.save = false
wifi.start()
wifi.mode(wifi.STATION)
wifi.sta.config(cfg)
wifi.sta.connect()

dag = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" }
maand = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" }
tz = {}
tz[0] = "CET"
tz[1] = "CEST"
tz[-1] = ""

time.settimezone("CET-1CEST,M3.5.0/2,M10.5.0/3")
wifi.sta.on("connected", function(event, info)
  time.initntp("fritz.box")
end)

mytimer = tmr.create()
mytimer:alarm(20000, tmr.ALARM_AUTO, function()
  t = time.getlocal()
  print(string.format("%s %d %s %d, %2d:%02d:%02d %s", dag[t["wday"]], t["day"], maand[t["mon"]], t["year"], t["hour"], t["min"], t["sec"], tz[t["dst"]]))

  m:connect("rpi-zero-2.fritz.box", 1883, 0, function(client)
    print("connected")
    -- Calling subscribe/publish only makes sense once the connection
    -- was successfully established. You can do that either here in the
    -- 'connect' callback or you need to otherwise make sure the
    -- connection was established (e.g. tracking connection status or in
    -- m:on("connect", function)).

    -- subscribe topic with qos = 0
    client:subscribe("esp32/demo11/#", 0, function(client) print("subscribe success") end)
    -- publish a message with data = hello, QoS = 0, retain = 0
    --client:publish("/topic", "hello", 0, 0, function(client) print("sent") end)
  end,
  function(client, reason)
    print("failed reason: " .. reason)
  end)

end)

gpio.config({gpio={25, 26, 27}, dir=gpio.OUT})
myChannel = ledc.newChannel({
  gpio=27,
  bits=ledc.TIMER_10_BIT,
  mode=ledc.LOW_SPEED,
  timer=ledc.TIMER_0,
  channel=ledc.CHANNEL_0,
  frequency=500,
  duty=100
});

led_on = false
mytimer2 = tmr.create()
mytimer2:alarm(300, tmr.ALARM_AUTO, function()
  led_on = not led_on
  if led_on then
    gpio.write(25, 1)
    gpio.write(26, 0)
  else
    gpio.write(25, 0)
    gpio.write(26, 1)
  end
end)

-- mytimer2:interval(250)

-- myChannel:setduty(1023)

m = mqtt.Client("id11", 120)
m:on("message", function(client, topic, data)
  print(topic .. ":" )
  if data ~= nil then
    print(data)
  end
end)

mytimer3 = tmr.create()
mytimer3:alarm(2000, tmr.ALARM_SINGLE,
  function()
    m:connect("rpi-zero-2.fritz.box", 1883, 0,
      function(client)
      	print("connected")
      	-- Calling subscribe/publish only makes sense once the connection
      	-- was successfully established. You can do that either here in the
      	-- 'connect' callback or you need to otherwise make sure the
      	-- connection was established (e.g. tracking connection status or in
      	-- m:on("connect", function)).

      	-- subscribe topic with qos = 0
      	client:subscribe("esp32/demo11/#", 0, function(client) print("subscribe success") end)
      	-- publish a message with data = hello, QoS = 0, retain = 0
      	--client:publish("/topic", "hello", 0, 0, function(client) print("sent") end)
      end,
      function(client, reason)
        print("failed reason: " .. reason)
      end)
  end)
