//#define DEBUG

#ifdef DEBUG
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#define WRITE(x) Serial.write(x)
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

#define BLUE 2
#define GREEN 6
#define RED 4
#define WHITE 5

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// See guide for details on sensor wiring and usage:
//   https://learn.adafruit.com/dht/overview

DHT_Unified dht(DHTPIN, DHTTYPE);

byte data[8] = {
    0b0001,
    0b0011,
    0b0010,
    0b0110,
    0b0100,
    0b1100,
    0b1000,
    0};

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

    pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(RED, OUTPUT);
    pinMode(WHITE, OUTPUT);

    for (int i = 0; i < 8; i++)
    {
        digitalWrite(BLUE, data[i] & 0b0001 ? HIGH : LOW);
        digitalWrite(GREEN, data[i] & 0b0010 ? HIGH : LOW);
        digitalWrite(RED, data[i] & 0b0100 ? HIGH : LOW);
        digitalWrite(WHITE, data[i] & 0b1000 ? HIGH : LOW);
        delay(600);
    }
}

void loop()
{
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature))
    {
        PRINT(F("Error reading temperature!\t"));
        digitalWrite(BLUE, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(RED, HIGH);
        digitalWrite(WHITE, LOW);
        delay(200);
        digitalWrite(RED, LOW);
    }
    else
    {
        PRINT(event.temperature);
        PRINT(F("°C\t"));
        int i = event.temperature / 2.0 - 7.0;
        PRINTLN(i);
        if (i < 0)
        {
            i = 0;
        }
        else if (i > 6)
        {
            i = 6;
        }
        digitalWrite(BLUE, data[i] & 0b0001 ? HIGH : LOW);
        digitalWrite(GREEN, data[i] & 0b0010 ? HIGH : LOW);
        digitalWrite(RED, data[i] & 0b0100 ? HIGH : LOW);
        digitalWrite(WHITE, data[i] & 0b1000 ? HIGH : LOW);
    }
    delay(10000);
}
