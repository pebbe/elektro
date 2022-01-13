//load('api_aws.js');
//load('api_azure.js');
//load('api_config.js');
//load('api_dash.js');
//load('api_events.js');
//load('api_gcp.js');
load('api_gpio.js');
//load('api_mqtt.js');
//load('api_shadow.js');
load('api_timer.js');
//load('api_sys.js');
//load('api_watson.js');
load('api_pwm.js');

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


Timer.set(10000, 0, function() {
  Timer.del(mytimer);
  mytimer = blink(500);
}, null);
