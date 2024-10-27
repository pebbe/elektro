#include <ArduinoOTA.h>
#include <WiFi.h>
#include "secret.h"

#define LEDBLUE 26

char const *ssid = SECRET_SSID;
char const *password = SECRET_PASS;

void setup() {
  pinMode(LEDBLUE, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();

  digitalWrite(LEDBLUE, HIGH);
  delay(200);
  digitalWrite(LEDBLUE, LOW);
  delay(800);
}
