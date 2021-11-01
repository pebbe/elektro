//#define DEBUG

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

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 3 // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

#define BLUE   2
#define GREEN  6
#define YELLOW 4
#define RED    5

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
    // Initialize device.
    dht.begin();
    PRINTLN(F("DHTxx Unified Sensor Example"));
    // Print temperature sensor details.
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
    PRINTLN(F("------------------------------------"));
    PRINTLN(F("Temperature Sensor"));
    PRINT(F("Sensor Type: "));
    PRINTLN(sensor.name);
    PRINT(F("Driver Ver:  "));
    PRINTLN(sensor.version);
    PRINT(F("Unique ID:   "));
    PRINTLN(sensor.sensor_id);
    PRINT(F("Max Value:   "));
    PRINT(sensor.max_value);
    PRINTLN(F("°C"));
    PRINT(F("Min Value:   "));
    PRINT(sensor.min_value);
    PRINTLN(F("°C"));
    PRINT(F("Resolution:  "));
    PRINT(sensor.resolution);
    PRINTLN(F("°C"));
    PRINTLN(F("------------------------------------"));
    // Print humidity sensor details.
    dht.humidity().getSensor(&sensor);
    PRINTLN(F("Humidity Sensor"));
    PRINT(F("Sensor Type: "));
    PRINTLN(sensor.name);
    PRINT(F("Driver Ver:  "));
    PRINTLN(sensor.version);
    PRINT(F("Unique ID:   "));
    PRINTLN(sensor.sensor_id);
    PRINT(F("Max Value:   "));
    PRINT(sensor.max_value);
    PRINTLN(F("%"));
    PRINT(F("Min Value:   "));
    PRINT(sensor.min_value);
    PRINTLN(F("%"));
    PRINT(F("Resolution:  "));
    PRINT(sensor.resolution);
    PRINTLN(F("%"));
    PRINTLN(F("------------------------------------"));

    pinMode(BLUE,   OUTPUT);
    pinMode(GREEN,  OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(RED,    OUTPUT);

    digitalWrite(BLUE,   LOW);
    digitalWrite(GREEN,  LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(RED,    LOW);

    for (int i = 15; i < 27; i++)
    {
	show(i);
	delay(700);
    }
}

void loop()
{
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        PRINT(F("Error reading temperature!\t"));
        digitalWrite(RED, HIGH);
	delay(3800);
        digitalWrite(RED, LOW);
	delay(200);
    }
    else
    {
        PRINT(event.temperature);
        PRINTLN(F("°C"));
        delay(4000 - show(event.temperature));
    }
}
