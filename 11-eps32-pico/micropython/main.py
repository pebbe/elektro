import machine, network, ntptime, sys, uos, utime, _thread
from umqttsimple import MQTTClient
from mytime import longtime, shorttime
import secret

topic_up = 'esp32/demo11/up'
topic_blink = 'esp32/demo11/blink'
topic_level = 'esp32/demo11/level'

blink_steps = 10
blink_step = 0
blink_on = False

def timer():
    i = 3
    while True:
        i -= 1
        if i == 0:
            i = 3
            ntptime.settime()
        sys.stdout.write(longtime() + '\n')
        utime.sleep(20)

def blinker():
    global blink_step, blink_steps, blink_on
    while True:
        blink_step += 1
        if blink_step >= blink_steps:
            blink_step = 0
            blink_on = not blink_on
            if blink_on:
                led_red.on()
                led_blue.off()
            else:
                led_red.off()
                led_blue.on()
        utime.sleep(.1)

def sub_cb(topic, msg):
    global pwm_yellow, blink_step, blink_steps
    try:
        t = topic.decode()
        value = int(msg.decode())
    except:
        return
    if t == topic_level:
        value = (value + 1) * 4 - 1
        if value < 0:
            value = 0
        if value > 1023:
            value = 1023
        pwm_yellow.duty(value)
    elif t == topic_blink:
        if value < 0:
            value = 1
        blink_steps = value
        blink_step = 0

sys.stdout.write('''
================
micropython/demo
================
''')

sys.stdout.write(uos.uname().machine + ' / ' + uos.uname().version + '\n')

led_red = machine.Pin(25, machine.Pin.OUT)
led_blue = machine.Pin(26, machine.Pin.OUT)
_thread.start_new_thread(blinker, ())

led_yellow = machine.Pin(27, machine.Pin.OUT)
pwm_yellow = machine.PWM(led_yellow)
utime.sleep(.5)
pwm_yellow.freq(500)
pwm_yellow.duty(100)

sys.stdout.write('Connecting to {} '.format(secret.ssid))
wifi = network.WLAN(network.STA_IF)
wifi.active(True)
wifi.connect(secret.ssid, secret.password)
while not wifi.isconnected():
    sys.stdout.write('.')
    utime.sleep(.1)
sys.stdout.write(' OK\n')

ntptime.host = 'fritz.box'
ntptime.settime()
_thread.start_new_thread(timer, ())

client = MQTTClient('id11', 'rpi-zero-2.fritz.box', port=1883)
client.set_last_will(topic_up, '----', retain=True, qos=1)
client.set_callback(sub_cb)
client.connect()
client.subscribe(topic_blink)
client.subscribe(topic_level)
client.publish(topic_up, shorttime(), retain=True, qos=1)

while True:
    client.wait_msg()
