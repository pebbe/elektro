load('api_gpio.js');
load('api_log.js');
load('api_math.js');
load('api_mqtt.js');
load('api_pwm.js');
load('api_timer.js');

function blink(ms) {
  let id = Timer.set(ms, Timer.REPEAT, function() {
    led_on = !led_on;
    GPIO.write(25, led_on);
    GPIO.write(26, !led_on);
  }, null);
  return id;
}


GPIO.set_mode(25, GPIO.MODE_OUTPUT);
GPIO.set_mode(26, GPIO.MODE_OUTPUT);
GPIO.set_mode(27, GPIO.MODE_OUTPUT);

let led_on = false;
let mytimer = blink(1000);

PWM.set(27, 5000, 0.25);

// MQTT

function handleMQTT(conn, topic, msg) {
  Log.info(topic);
  Log.info(msg);
  let value = Math.round(msg);
  if (topic == 'esp32/demo11/blink') {
    if (value < 1) {
      value = 1;
    }
    Timer.del(mytimer);
    mytimer = blink(value * 100);
  } else if (topic == 'esp32/demo11/level') {
    if (value < 0) {
      value = 0;
    } else if (value > 255) {
      value = 255;
    }
    PWM.set(27, 5000, value / 255);
  }
}

function setupMQTT() {
  Log.info('setupMQTT')
  MQTT.pub('esp32/demo11/up', 'up!', 1, true);
  MQTT.sub('esp32/demo11/blink', handleMQTT);
  MQTT.sub('esp32/demo11/level', handleMQTT);
}

let t1 = Timer.set(10, Timer.REPEAT, function() {
  if (MQTT.isConnected() && Timer.now() > 100000000) {
    Timer.del(t1);
    setupMQTT();
  }
}, null);


// Print time each 20 sec

function printTime() {
  Log.info(Timer.fmt('%A %e %B %Y, %H:%M:%S %Z', Timer.now()));
}

printTime();
Timer.set(20000, Timer.REPEAT, printTime, null);
