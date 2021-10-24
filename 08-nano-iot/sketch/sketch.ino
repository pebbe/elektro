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

#define LED 21

#include "secret.h";
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

unsigned long clockms = 0;
unsigned long epoch = 0;
unsigned clockboff = 0;
unsigned clockboffnext = 1;

int status = WL_IDLE_STATUS;
int upcount = 0;
char server[] = "urd2.let.rug.nl";
WiFiSSLClient client;

int state = 0;
int state_max = 4;

unsigned long ms = 0;
unsigned long t1 = 0;
unsigned long t2 = 0;
bool day = false;
unsigned sunboff = 0;
unsigned sunboffnext = 1;

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN  3
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

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

    dht.begin();
}

void loop() {
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
        doDHT(1);
        break;
    case 4:
        doDHT(2);
        break;
    }
    disconnect();
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
        if (opt > 0 && opt < 5) {
            // De spatie is te breed bij dit font, dus gebruik ik een punt in doSun()
            // Hier in de uitvoer wordt die punt overgeslagen
            char s2[10];
            strcpy(s2, s);
            int i = strlen(s2);
            int w1 = u8g2.getStrWidth(s2);
            s2[i-3] = '\0';
            int w2 = u8g2.getStrWidth(s2 + i - 2);
            // w1 -= 4; // spatie nog iets smaller maken
            u8g2.drawStr(64 - w1 / 2, 32 + 16, s2);
            u8g2.drawStr(64 - w1 / 2 + w1 - w2, 32 + 16, s2 + i - 2);
        } else if (opt == 5) {
            // bij temperatuur het graden-teken niet meerekenen voor het centreren
            String s1 = String(s).substring(0, strlen(s) - 1);
            u8g2.drawStr(64 - u8g2.getStrWidth(s1.c_str()) / 2, 32 + 16, s);
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
        }
    } while( u8g2.nextPage() );
}

void draw(char const *s) {
    draw(s, 0);
}

void doClock() {

    PRINTLN("doClock()");
    if (clockms == 0) {
        getClock();
        if (clockms == 0) {
            draw("*klok");
            return;
        }
    }
    unsigned long now = millis();
    if (now < clockms) {
        // wrap around
        clockms = 0;
        doClock();
        return;
    }
    unsigned long diff = (now - clockms) / 1000;
    if (diff > 43200) {
        // resync na 12 uur
        clockms = 0;
        doClock();
        return;
    }

    now = epoch + diff;
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
}

void getClock() {

    if (!connect()) {
        return;
    }

    if (clockboff) {
        PRINT("getClock() back-off: ");
        PRINTLN(clockboff);
        clockboff--;
        return;
    }

    draw("?klok");
    PRINTLN("getClock()");

    WiFiUDP ntpUDP;
    NTPClient timeClient(ntpUDP, "fritz.box"); // "nl.pool.ntp.org");
    timeClient.begin();

    unsigned long ms1, ms2;
    ms1 = millis();
    if (timeClient.forceUpdate()) {
        epoch = timeClient.getEpochTime();
        ms2 = millis();
        clockms = ms1 / 2 + ms2 / 2;
        clockboffnext = 1;
        PRINTLN(timeClient.getFormattedTime());
    } else {
        clockboff = clockboffnext;
        if (clockboffnext < 64) {
            clockboffnext *= 2;
        }
        draw("*klok");
        PRINT("timeClient.update() failed, back-off: ");
        PRINTLN(clockboff);
        delay(6000);
    }
    timeClient.end();
}

void doSun(int n) {

    PRINT("doSun(");
    PRINT(n);
    PRINTLN(")");

    if (t1 == 0 && t2 == 0) {
        if (!getSun()) {
            return;
        }
    }
    unsigned long now = millis();
    if (now < ms) {
        // wrap around
        t1 = 0;
        t2 = 0;
        doSun(n);
        return;
    }
    unsigned long diff = now - ms;
    if (diff > t2) {
        // volgende periode
        t1 = 0;
        t2 = 0;
        doSun(n);
        return;
    }

    String s;
    int opt;
    if (n == 1) {
        opt = day ? 1 : 2;
        s = format(t1 + diff);
    } else {
        opt = day ? 3 : 4;
        s = format(t2 - diff);
    }
    PRINTLN(s);
    draw(s.c_str(), opt);
}

String format(unsigned long t) {
    t += 30000; // + halve minuut
    t /= 60000; // millis -> minuten
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
    client.println("GET /~kleiweg/sun/bin/ HTTP/1.1");
    client.println("Host: urd2.let.rug.nl");
    client.println("Connection: close");
    client.println();

    ms = millis();

    delay(1000);
    String s;
    int nl = 0;
    while (client.available()) {
        char c = client.read();
        if (c == '\n') {
            nl++;
        } else if (c == '\r') {
        } else if (nl == 1) {
            nl = 0;
        } else if (nl > 1) {
            s += c;
        }
        WRITE(c);
    }
    PRINT("Data: ");
    PRINTLN(s);
    if (!client.connected()) {
        PRINTLN("disconnecting from server");
        client.stop();
    }

    if (s.length() < 15) {
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

    day = s.substring(4, 5) == "1";
    t1 = 60000 * s.substring(6, 10).toInt();
    t2 = 60000 * s.substring(11, 15).toInt();

    if (t1 == 0 && t2 == 0) {
        backoffSun();
        return false;
    }

    sunboffnext = 1;
    return true;
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

void doDHT(int n) {
    PRINT("doDHT(");
    PRINT(n);
    PRINTLN(")");
    sensors_event_t event;
    if (n == 1) {
        dht.temperature().getEvent(&event);
        PRINT(event.temperature);
        PRINTLN("°");
        int t = int(event.temperature * 10.0 + 0.5);
        String s = String(t / 10);
        s += ".";
        s += String(t % 10);
        s += "\xB0";
        draw(s.c_str(), 5);
        return;
    }
    dht.humidity().getEvent(&event);
    PRINT(event.relative_humidity);
    PRINTLN("%");
    String s = String(int(event.relative_humidity + 0.5));
    s += "%";
    draw(s.c_str());
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

    upcount = 10;

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
    digitalWrite(LED, HIGH);
    PRINTLN("Connected to WiFi");
    printWifiStatus();
    delay(2000);
    return true;
}

void disconnect() {
    if (upcount > 0) {
        upcount--;
        return;
    }
    if (status != WL_CONNECTED) {
        return;
    }
    WiFi.end();
    digitalWrite(LED, LOW);
    status = WiFi.status();
    PRINT("Disconnect: ");
    PRINTLN(status);
}
