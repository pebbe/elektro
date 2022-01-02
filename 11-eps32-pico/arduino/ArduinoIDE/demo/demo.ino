
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

#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <time.h>
#include "secret.h"

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

const char *dag[] = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" };
const char *maand[] = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "fritz.box");  // fritz.box  ntp.xs4all.nl  nl.pool.ntp.org

void setup()
{

#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial)
        ;
#endif

    // connect to WiFi
    PRINTF("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            PRINT(".");
        }
    PRINTLN(" CONNECTED");

    timeClient.begin();

    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    
           
}

void loop()
{
    timeClient.update();

    time_t now = timeClient.getEpochTime();
    struct tm* t = localtime(&now);
    PRINTF("%s %d %s %d, %d:%02d:%02d %s\n",
           dag[t->tm_wday],
           t->tm_mday, maand[t->tm_mon], 1900 + t->tm_year,
           t->tm_hour, t->tm_min, t->tm_sec,
           t->tm_isdst ? "CEST" : "CET");

    delay(1000);
}
