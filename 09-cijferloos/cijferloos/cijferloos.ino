
#define TINY

#ifdef TINY
#define DHTPIN 4
#define BLUE   3
#define GREEN  2
#define YELLOW 1
#define RED    0
#else
#define DHTPIN 3
#define BLUE   2
#define GREEN  6
#define YELLOW 4
#define RED    5
#define DEBUG
#endif

#ifdef DEBUG
#define PRINT(x)   Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#define WRITE(x)   Serial.write(x)
#else
#define PRINT(x)
#define PRINTLN(x)
#define WRITE(x)
#endif

#include <Arduino.h>

#include <dht.h>

dht DHT;

byte leds[4] = { BLUE, GREEN, YELLOW, RED };

int show(int temp) {
    PRINT(F("show("));
    PRINT(temp);
    PRINT(F(")\t"));
    temp -= 15;
    if (temp < 0) {
        temp = 0;
    } else if (temp > 11) {
        temp = 11;
    }
    int led = leds[temp / 3];
    int blinks = (temp % 3) + 1;
    PRINT(temp / 3);
    PRINT("\t");
    PRINTLN(blinks);
    for (int i = 0; i < blinks; i++) {
        digitalWrite(led, HIGH);
        delay(300);
        digitalWrite(led, LOW);
        delay(300);
    }
    return blinks * 600;
}

void setup()
{
#ifdef DEBUG
    Serial.begin(9600);
    while (!Serial)
        ;
#endif
    pinMode(BLUE,   OUTPUT);
    pinMode(GREEN,  OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(RED,    OUTPUT);

    digitalWrite(BLUE,   LOW);
    digitalWrite(GREEN,  LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED,    LOW);

    for (int i = 0; i < 4; i++) {
        digitalWrite(leds[i], HIGH);
        delay(500);
        digitalWrite(leds[i], LOW);
    }
    delay(1000);
}

void loop()
{
    if (DHT.read11(DHTPIN) == DHTLIB_OK) {
        PRINT(DHT.temperature);
        PRINTLN(F("°C"));
        delay(4000 - show(DHT.temperature));
    } else {
        PRINT(F("Error reading temperature"));
        digitalWrite(RED, HIGH);
        delay(3800);
        digitalWrite(RED, LOW);
        delay(200);
    }
}
