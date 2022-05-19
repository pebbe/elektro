// TODO: code reorganiseren, opruimen, leesbaarder maken

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

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>

#include <BME280I2C.h>
#include <Wire.h>
BME280I2C bme;    // Default : forced mode, standby time = 1000 ms
                  // Oversampling = pressure ×1, temperature ×1, humidity ×1, filter off,


#define LED 21

#include "secret.h"
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;
char server[] = "bisse.nl";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "nl.pool.ntp.org");  // fritz.box  nl.pool.ntp.org

WiFiSSLClient client;
//WiFiClient client;

WiFiClient plainClient;

PubSubClient mqttClient(plainClient);

int state = 0;
int state_max = 5;

long unsigned now;
long unsigned nowT = 0;
long unsigned nowH = 0;
long unsigned nowP = 0;

unsigned long t1 = 0;
unsigned long t2 = 0;
bool day = false;
unsigned sunboff = 0;
unsigned sunboffnext = 1;

#include "U8g2lib.h"
U8G2_SH1106_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R2, 4, 5, 6, 7);

unsigned int
tzdata[] = {
            1616893200,
            1635642000,
            1648342800,
            1667091600,
            1679792400,
            1698541200,
            1711846800,
            1729990800,
            1743296400,
            1761440400,
            1774746000,
            1792890000,
            1806195600,
            1824944400,
            1837645200,
            1856394000,
            1869094800,
            1887843600,
            1901149200,
            1919293200,
            1932598800,
            1950742800,
            1964048400,
            1982797200,
            1995498000,
            2014246800,
            2026947600,
            2045696400,
            2058397200,
            2077146000,
            2090451600,
            2108595600,
            2121901200,
            2140045200,
            2153350800,
            2172099600,
            2184800400,
            2203549200,
            2216250000,
            2234998800,
            2248304400,
            2266448400,
            2279754000,
            2297898000,
            2311203600,
            2329347600,
            2342653200,
            2361402000,
            2374102800,
            2392851600,
            2405552400,
            2424301200,
            2437606800,
            2455750800,
            2469056400,
            2487200400,
            2500506000,
            2519254800,
            0 };


void setup() {

#ifdef DEBUG
    Serial.begin(9600);
    while (!Serial)
        ;
#endif

    pinMode(LED, OUTPUT);

    u8g2.begin();
    u8g2.setContrast(100); // 0..255

    draw("?wifi");
    if (WiFi.status() == WL_NO_MODULE) {
        PRINTLN("Communication with WiFi module failed!");
        draw("*wifi");
        while (true);
    }

#ifdef DEBUG
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.print("Current Wifi firmware version: ");
        Serial.println(fv);
        Serial.print("Latest Wifi firmware version: ");
        Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
        Serial.println("Please upgrade the firmware");
    }
#endif

    connect();
    timeClient.begin();

    Wire.begin();
    draw("?sens");
    while(!bme.begin())  {
        PRINTLN("Could not find BME280 sensor!");
        delay(1000);
    }

#ifdef DEBUG
    switch(bme.chipModel())
        {
        case BME280::ChipModel_BME280:
            Serial.println("Found BME280 sensor! Success.");
            break;
        case BME280::ChipModel_BMP280:
            Serial.println("Found BMP280 sensor! No Humidity available.");
            break;
        default:
            Serial.println("Found UNKNOWN sensor! Error!");
        }
#endif


    mqttClient.setServer("192.168.178.24", 1883);
}

bool mqttConnect() {
    if (mqttClient.connected()) {
        return true;
    }
    if (mqttClient.connect("id08" /* , MQTT_USER, MQTT_PASS */)) {
        PRINTLN("MQTT connection established");
        // Establish the subscribe event
        //mqttClient.setCallback(subscribeReceive);
        return true;
    }
    PRINTLN("MQTT connection failed");
    return false;
}

void loop() {
    mqttClient.loop();

    switch (state) {
    case 0:
        doClock();
        break;
    case 1:
        doSun(1);
        break;
    case 2:
        doSun(2);
        break;
    case 3:
        doBME(1);
        break;
    case 4:
        doBME(2);
        break;
    case 5:
        doBME(3);
        break;
    }
    if (++state > state_max) {
        state = 0;
    }
    delay(6000);
}

void draw(char const *s, int opt) {
    u8g2.firstPage();
    do {
        /*
          if (status == WL_CONNECTED) {
          u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
          u8g2.drawStr(0, 40, "\xF7");
          }
        */

        u8g2.setFont(u8g2_font_logisoso34_tf);
        if (opt > 0 && opt < 5 || opt == 6) {
            // De spatie is te breed bij dit font, dus gebruik ik een punt in doSun() en bij humidity
            // Hier in de uitvoer wordt die punt overgeslagen
            int offset = (opt == 6) ? 1 : 2;
            char s2[10];
            strcpy(s2, s);
            int i = strlen(s2);
            int w1 = u8g2.getStrWidth(s2);
            s2[i-1-offset] = '\0';
            int w2 = u8g2.getStrWidth(s2 + i - offset);
            if (opt == 6) {
                w1 -= 6; // spatie nog iets smaller maken
            }
            u8g2.drawStr(64 - w1 / 2, 32 + 16, s2);
            u8g2.drawStr(64 - w1 / 2 + w1 - w2, 32 + 16, s2 + i - offset);
        } else if (opt == 5) {
            // bij temperatuur het graden-teken niet helemaal meerekenen voor het centreren
            String s1 = String(s).substring(0, strlen(s) - 1);
            u8g2.drawStr(64 - u8g2.getStrWidth(s1.c_str()) / 2 - 4, 32 + 16, s);
        } else {
            u8g2.drawStr(64 - u8g2.getStrWidth(s) / 2, 32 + 16, s);
        }
        switch (opt) {
        case 1:
            // dag links
            u8g2.drawTriangle(0, 12, 12, 12, 6, 0);
            break;
        case 2:
            // nacht links
            u8g2.drawTriangle(0, 51, 12, 51, 6, 63);
            break;
        case 3:
            // dag rechts
            u8g2.drawTriangle(115, 51, 127, 51, 121, 63);
            break;
        case 4:
            // nacht rechts
            u8g2.drawTriangle(115, 12, 127, 12, 121, 0);
            break;
        case 7:
            //u8g2.drawBox(44, 0, 40, 3);
            u8g2.drawBox(44, 60, 40, 3);
            break;
        }
    } while( u8g2.nextPage() );
}

void draw(char const *s) {
    draw(s, 0);
}

void doClock() {

    if (connect()) {
        timeClient.update();
    }

    now = timeClient.getEpochTime();

    for (int i = 0; tzdata[i] != 0; i++) {
        if (tzdata[i] > now) {
            now += 3600 + 3600 * (i%2);
            break;
        }
    }

    PRINT("locale tijd: ");
    int hr = (now % (86400L / 2)) / 3600;
    if (hr == 0) {
        hr = 12;
    }
    String t = String(hr);
    t += ":";
    PRINT(hr);
    PRINT(':');
    int mn = (now % 3600) / 60;
    if (mn < 10) {
        PRINT('0');
        t += "0";
    }
    PRINT(mn);
    t += String(mn);
#ifdef DEBUG
    PRINT(':');
    int sec = now % 60;
    if (sec < 10) {
        PRINT('0');
    }
    PRINTLN(sec);
#endif
    draw(t.c_str());
    digitalWrite(LED, (mn == 0 || mn == 30) ? HIGH : LOW);
}

void doSun(int n) {

    PRINT("doSun(");
    PRINT(n);
    PRINTLN(")");

    long unsigned now = timeClient.getEpochTime();

    if (now > t2) {
        if (!getSun()) {
            return;
        }
    }

    String s;
    int opt;
    if (n == 1) {
        opt = day ? 1 : 2;
        s = format(now - t1);
    } else {
        opt = day ? 3 : 4;
        s = format(t2 - now);
    }
    u8g2.setContrast(day ? 100 : 0); // 0..255
    draw(s.c_str(), opt);
    PRINTLN(s);
}

String format(unsigned long t) {
    t += 30; // + halve minuut
    t /= 60; // seconden -> minuten
    String s = String(t / 60);
    s += ".";
    unsigned long m = t % 60;
    if (m < 10) {
        s += "0";
    }
    s += String(m);
    return s;
}

bool getSun() {

    if (!connect()) {
        return false;
    }

    if (sunboff) {
        draw("*zon");
        PRINT("getSun() back-off: ");
        PRINTLN(sunboff);
        sunboff--;
        return false;
    }

    t1 = 0;
    t2 = 0;

    draw("?zon");
    PRINT("Starting connection to server ");
    PRINTLN(server);
    if (!client.connect(server, 443)) {
        draw("*zon");
        PRINTLN("Connecting failed");
        backoffSun();
        return false;
    }

    PRINTLN("connected to server");
    client.println("GET /data/sun.txt HTTP/1.1");
    client.println("Host: bisse.nl");
    client.println("Connection: close");
    client.println();

    delay(1000);
    String s;
    int nl = 0;
    while (client.available()) {
        char c = client.read();
        if (nl < 2) {
            if (c == '\n') {
                nl++;
            } else if (c == '\r') {
            } else {
                nl = 0;
            }
        } else {
            s += c;
        }
        WRITE(c);
    }
    PRINT("Data: ");
    PRINTLN(s);
    PRINT("Data size: ");
    PRINTLN(s.length());

    if (!client.connected()) {
        PRINTLN("disconnecting from server");
        client.stop();
    }

    if (s.length() < 260) {
        draw("*kort");
        PRINTLN("Too short");
        backoffSun();
        return false;
    }

    if (s.substring(0, 3) != "SUN") {
        draw("*zon");
        PRINTLN("Invalid response");
        backoffSun();
        return false;
    }

    long unsigned now = timeClient.getEpochTime();
    for (int i = 1; i < 16; i++) {
        unsigned int value = s.substring(4 + i * 16, 19 + i * 16).toInt();
        if (value > now) {
            t2 = value;
            t1 = s.substring(-12 + i * 16, 3 + i * 16).toInt();
            day = (i%2) == 1 ? true : false;
            sunboffnext = 1;
            return true;
        }
    }

    draw("*zon");
    PRINTLN("No value");
    backoffSun();
    return false;
}

void backoffSun() {
    sunboff = sunboffnext;
    if (sunboffnext < 1024) {
        sunboffnext *= 2;
    }
    draw("*zon");
    PRINT("getSun() failed, back-off: ");
    PRINTLN(sunboff);
    delay(6000);
}

void doBME(int n) {
    PRINT("doBMH(");
    PRINT(n);
    PRINTLN(")");

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    float temp(NAN), hum(NAN), pres(NAN);
    bme.read(pres, temp, hum, tempUnit, presUnit);

    PRINT("Temp: ");
    PRINT(temp);
    PRINT("°C");
    PRINT("\tHumidity: ");
    PRINT(hum);
    PRINT("% RH");
    PRINT("\tPressure: ");
    PRINT(pres);
    PRINTLN("Pa");

    if (n == 1) {
        int t = int(temp * 10.0 + .5);
        String s = String(t / 10);
        s += ".";
        s += String(t % 10);
        s += "\xB0";
        draw(s.c_str(), 5);
        if (now - nowT > 600) {
            nowT = now;
            pub("boven/temp", t);
        }
        return;
    }

    if (n == 2) {
        /*
          int h = int(hum * 10.0 + .5);
          String s = String(h / 10);
          s += ".";
          s += String(h % 10);
        */
        String s = String(int(hum + .5));
        s += ".%";
        draw(s.c_str(), 6);
        if (now - nowH > 600) {
            nowH = now;
            pub("boven/hum", int(hum * 10.0 + .5));
        }
        return;
    }

    String s = String(int(pres * .01 + .5));
    draw(s.c_str(), 7);
    if (now - nowP > 600) {
        nowP = now;
        pub("boven/pres", int(pres * .1 + .5));
    }
}

void pub(char const *s, int i) {
    if (!mqttConnect()) {
        return;
    }

    String m = String(i);
    PRINT("MQTT pub ");
    PRINT(s);
    PRINT(" ");
    PRINT(m);
    PRINT(" ... ");
    if(mqttClient.publish(s, m.c_str(), true)) {
        PRINTLN("OK");
    } else {
        PRINTLN("FAIL");
    }
}

void printWifiStatus() {
    long rssi = WiFi.RSSI();
    String s = String(rssi);
    draw(s.c_str());

#ifdef DEBUG
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    Serial.print("signal strength (RSSI): ");
    Serial.print(rssi);
    Serial.println(" dBm");
#endif
}

bool connect() {

    status = WiFi.status();
    if (status == WL_CONNECTED) {
        return true;
    }
    for (int i = 0; i < 9; i++) {
        String s = "?c";
        s += String(i + 1);
        draw(s.c_str());
        PRINT("Attempting to connect to SSID: ");
        PRINTLN(ssid);
        status = WiFi.begin(ssid, pass);
        if (status == WL_CONNECTED) {
            break;
        }
        delay(10000);
    }
    if (status != WL_CONNECTED) {
        draw("*con");
        PRINT("Connecting to SSID failed");
        return false;
    }
    PRINTLN("Connected to WiFi");
    printWifiStatus();
    delay(2000);
    return true;
}
