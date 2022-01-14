
#define DEBUG

#ifdef DEBUG
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#define PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define PRINT(x)
#define PRINTLN(x)
#define PRINTF(...)
#endif

#define LEDRED 25
#define LEDBLUE 26
#define LEDYELLOW 27

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <time.h>
#include "secret.h"

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

const char *dag[] = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" };
const char *maand[] = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fritz.box");  // fritz.box  ntp.xs4all.nl  nl.pool.ntp.org

void callback(char* topic, byte* payload, unsigned int length);
char const *flashmode(FlashMode_t mode);

WiFiClient plainClient;
const char *server = "rpi-zero-2.fritz.box";
PubSubClient mqttClient(server, 1883, callback, plainClient);

int blinkStep = 1;
int blinkDelay = 5;
bool blinkOn = false;
int clockStep = 1;
int clockDelay = 200;
int ledLevel = 64;
int ledLevelNow = -1;

void setup()
{
    pinMode(LEDRED, OUTPUT);
    pinMode(LEDBLUE, OUTPUT);
    pinMode(LEDYELLOW, OUTPUT);

#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial)
        ;
#endif

    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
    PRINTF("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
    PRINTF("This chip has %d cores\n", ESP.getChipCores());
    PRINTF("Chip ID: %lu\n", chipId);
    PRINTF("CPU frequency: %lu MHz\n", ESP.getCpuFreqMHz());
    PRINTF("Cycle count: %lu\n", ESP.getCycleCount());
    PRINTF("SDK Version: %s\n", ESP.getSdkVersion());
    PRINTLN("Internal RAM");
    PRINTF("    Heap Size: %lu\n", ESP.getHeapSize());
    PRINTF("    Free Heap: %lu\n", ESP.getFreeHeap());
    PRINTF("    Min Free Heap: %lu\n", ESP.getMinFreeHeap());
    PRINTF("    Max Alloc Heap: %lu\n", ESP.getMaxAllocHeap());
    PRINTLN("Flash Chip");
    PRINTF("    Size: %lu\n", ESP.getFlashChipSize());
    PRINTF("    Speed: %lu\n", ESP.getFlashChipSpeed());
    PRINTF("    Mode: %s\n", flashmode(ESP.getFlashChipMode()));
    PRINTF("Sketch Size: %lu\n", ESP.getSketchSize());
    PRINTF("Free Sketch Space: %lu\n", ESP.getFreeSketchSpace());

    // connect to WiFi
    PRINTF("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        PRINT(".");
    }
    PRINTLN(" CONNECTED");
    delay(500);

    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    timeClient.begin();

    char const *topic = "esp32/demo11/up";

    mqttClient.connect("id11", NULL, NULL, topic, 1, true, "----");
    timeClient.update();
    time_t now = timeClient.getEpochTime();
    struct tm* t = localtime(&now);
    char s[100];
    sprintf(s, "%02d-%02d-%04d %02d:%02d:%02d", t->tm_mday, t->tm_mon+1, 1900 + t->tm_year, t->tm_hour, t->tm_min, t->tm_sec);
    delay(500);
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

char const *flashmode(FlashMode_t mode)
{
    switch (mode) {
    case FM_QIO:
        return "QIO";
    case FM_QOUT:
        return "QOUT";
    case FM_DIO:
        return "DIO";
    case FM_DOUT:
        return "DOUT";
    case FM_FAST_READ:
        return "FAST_READ";
    case FM_SLOW_READ:
        return "SLOW_READ";
    }
    return "UNKNOWN";
}
