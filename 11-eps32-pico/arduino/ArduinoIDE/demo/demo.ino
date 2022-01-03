
#define DEBUG

#ifdef DEBUG
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#define PRINTF(...) Serial.printf(__VA_ARGS__)
#define WRITE(x) Serial.write(x)
#else
#define PRINT(x)
#define PRINTLN(x)
#define PRINTF(...)
#define WRITE(x)
#endif

#define LEDRED 25
#define LEDBLUE 26
#define LEDYELLOW 27

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <analogWrite.h>
#include <time.h>
#include "secret.h"

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

const char *dag[] = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" };
const char *maand[] = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fritz.box");  // fritz.box  ntp.xs4all.nl  nl.pool.ntp.org

void callback(char* topic, byte* payload, unsigned int length);

WiFiClient plainClient;
const char *server = "rpi-zero-2.fritz.box";
PubSubClient mqttClient(server, 1883, callback, plainClient);

int blinkStep = 1;
int blinkDelay = 5;
bool blinkOn = false;
int clockStep = 1;
int clockDelay = 10;
int ledLevel = 64;
int ledLevelNow = -1;

void setup()
{

#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial)
        ;
#endif

    pinMode(LEDRED, OUTPUT);
    pinMode(LEDBLUE, OUTPUT);
    pinMode(LEDYELLOW, OUTPUT);

    // connect to WiFi
    PRINTF("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            PRINT(".");
        }
    PRINTLN(" CONNECTED");

    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    timeClient.begin();

    char const *topic = "esp32/demo11/up";

    mqttClient.connect("id11", NULL, NULL, topic, 1, true, "----");
    timeClient.update();
    time_t now = timeClient.getEpochTime();
    struct tm* t = localtime(&now);
    char s[100];
    sprintf(s, "%02d-%02d-%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon+1, 1900 + t->tm_year, t->tm_hour, t->tm_min, t->tm_sec);
    mqttClient.publish(topic, s, true);
    mqttClient.subscribe("esp32/demo11/blink");
    mqttClient.subscribe("esp32/demo11/level");
}

void loop()
{

    mqttClient.loop();

    if (--clockStep <= 0) {
        clockStep = clockDelay;

        timeClient.update();

        time_t now = timeClient.getEpochTime();
        struct tm* t = localtime(&now);
        PRINTF("%s %d %s %d, %d:%02d:%02d %s\n",
               dag[t->tm_wday],
               t->tm_mday, maand[t->tm_mon], 1900 + t->tm_year,
               t->tm_hour, t->tm_min, t->tm_sec,
               t->tm_isdst ? "CEST" : "CET");

    }

    if (--blinkStep <= 0) {
        blinkStep = blinkDelay;
        blinkOn = !blinkOn;
        digitalWrite(LEDRED, blinkOn ? HIGH : LOW);
        digitalWrite(LEDBLUE, blinkOn ? LOW : HIGH);
    }

    if (ledLevel != ledLevelNow) {
        ledLevelNow = ledLevel;
        analogWrite(LEDYELLOW, ledLevel);
        // voor native zie: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
    }

    delay(100);
}

void callback(char* topic, byte* payload, unsigned int length)
{
    char s[length+1];
    for (int i = 0; i < length; i++)
        s[i] = payload[i];
    s[length] = 0;
    PRINT(topic);
    PRINT(": ");
    PRINTLN(s);

    int v = atoi(s);
    if (!strcmp(topic, "esp32/demo11/blink")) {
        if (v < 1)
            v = 1;
        if (v > 1000)
            v = 1000;
        blinkDelay = v;
        blinkStep = 1;
    } else if (!strcmp(topic, "esp32/demo11/level")) {
        if (v < 0)
            v = 0;
        if (v > 255)
            v = 255;
        ledLevel = v;
    }

}
