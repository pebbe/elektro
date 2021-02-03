// een sketch voor de ATtiny85, iets interessanter dan Blink

#define LOWER 20

int pin[3]   = { 4, 0, 1 };    // fysieke pinnen met pwm: 3, 5, 6
int level[3] = { 0, 0, 0 };
int count[3] = { 2, 4, 8 };

void setup() {
  for (int i = 0; i < 3; i++) {
    pinMode(pin[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 3; i++) {
    int set = 0;
    if (count[i]-- == 0) {
      count[i] = random(5, 500);
      level[i] = random(level[i] < LOWER ? LOWER : level[i], 256);
      set = 1;
    } else if (level[i] > 0) {
      level[i] = (6 * level[i]) / 7;    // exponentionele uitdoving
      set = 1;
    }
    if (set) {
      analogWrite(pin[i], level[i]);
    }
  }
  delay(20);
}
