
// TODO: code reorganiseren, opruimen, leesbaarder maken
// TODO: zie TODOs beneden

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

#include "secret.h";
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

unsigned int localPort = 2390;        // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;       // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];  //buffer to hold incoming and outgoing packets
// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;

unsigned long clockms = 0;
unsigned long epoch = 0;

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
        if (status == WL_CONNECTED) {
            u8g2.setFont(u8g2_font_open_iconic_all_2x_t);
            u8g2.drawStr(0, 40, "\xF7");
        }

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
            u8g2.drawStr(64 - w1 / 2, 32 + 16, s2);
            u8g2.drawStr(64 - w1 / 2 + w1 - w2, 32 + 16, s2 + i - 2);
        } else {
            u8g2.drawStr(64 - u8g2.getStrWidth(s) / 2, 32 + 16, s);
        }
        switch (opt) {
        case 1:
            // dag links
            u8g2.drawTriangle(0, 16, 16, 16, 8, 0);
            break;
        case 2:
            // nacht links
            u8g2.drawTriangle(0, 47, 16, 47, 8, 63);
            break;
        case 3:
            // dag rechts
            u8g2.drawTriangle(111, 47, 127, 47, 119, 63);
            break;
        case 4:
            // nacht rechts
            u8g2.drawTriangle(111, 16, 127, 16, 119, 0);
            break;
        }
    } while( u8g2.nextPage() );
}

void draw(char const *s) {
    draw(s, 0);
}

void doClock() {

    // TODO: wat als getClock mislukt?

    PRINTLN("doClock()");
    if (clockms == 0) {
        getClock();
    }
    unsigned long now = millis();
    if (now < clockms) {
        // wrap around
        getClock();
        now = millis();
    }
    unsigned long diff = (now - clockms) / 1000;
    if (diff > 86400) {
        // resync na een dag
        getClock();
        now = millis();
        diff = (now - clockms) / 1000;
    }

    now = epoch + diff;
    for (int i = 0; tzdata[i] != 0; i++) {
        if (tzdata[i] > now) {
            now += 3600 + 3600 * (i%2);
            break;
        }
    }

    // print the hour, minute and second:
    PRINT("The local time is ");       // UTC is the time at Greenwich Meridian (GMT)
    PRINT((now % 86400L) / 3600); // print the hour (86400 equals secs per day)
    int h = (now % (86400L / 2)) / 3600;
    if (h == 0) {
        h = 12;
    }
    String t = String(h);
    PRINT(':');
    t += ":";
    if (((now % 3600) / 60) < 10) {
        // In the first 10 minutes of each hour, we'll want a leading '0'
        PRINT('0');
        t += "0";
    }
    PRINT((now  % 3600) / 60); // print the minute (3600 equals secs per minute)
    t += String((now  % 3600) / 60);
    PRINT(':');
    if ((now % 60) < 10) {
        // In the first 10 seconds of each minute, we'll want a leading '0'
        PRINT('0');
    }
    PRINTLN(now % 60); // print the second
    draw(t.c_str());
}

void getClock() {

    connect();

    draw("?klok");
    PRINTLN("getClock()");
    Udp.begin(localPort);
    sendNTPpacket(timeServer); // send an NTP packet to a time server
    unsigned long now = millis();
    // wait to see if a reply is available
    delay(1000);
    if (Udp.parsePacket()) {
        PRINTLN("packet received");
        // We've received a packet, read the data from it
        Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

        //the timestamp starts at byte 40 of the received packet and is four bytes,
        // or two words, long. First, extract the two words:

        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years:
        epoch = secsSince1900 - seventyYears;
        PRINT("Unix time = ");
        PRINTLN(epoch);

        clockms = now;
    }
}

void doSun(int n) {

    // TODO: wat als getSun() mislukt?

    PRINT("doSun(");
    PRINT(n);
    PRINTLN(")");
    if (t1 == 0 && t2 == 0) {
        if (getSun()) {
            return;
        }
    }
    unsigned long now = millis();
    if (now < ms) {
        // wrap around
        if (getSun()) {
            return;
        }
        now = millis();
    }
    unsigned long diff = now - ms;
    if (diff > t2) {
        // volgende periode
        if (getSun()) {
            return;
        }
        now = millis();
        diff = now - ms;
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

int getSun() {

    connect();

    t1 = 0;
    t2 = 0;

    draw("?zon");
    PRINTLN("Starting connection to server...");
    if (!client.connect(server, 443)) {
        draw("*zon");
        PRINTLN("Connecting failed");
        return -1;
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

    if (s.length() < 11) {
        draw("*kort");
        PRINTLN("Too short");
        return -1;
    }

    day = s.substring(0, 1) == "1";
    t1 = 60000 * s.substring(2, 6).toInt();
    t2 = 60000 * s.substring(7, 11).toInt();

    return 0;
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
        draw(s.c_str());
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
    s += "\"";
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

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
    //Serial.println("1");
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    //Serial.println("2");
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;

    //Serial.println("3");

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123); //NTP requests are to port 123
    //Serial.println("4");
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    //Serial.println("5");
    Udp.endPacket();
    //Serial.println("6");
}

void connect() {

    // TODO: wat als er geen verbinding gemaakt kan worden?

    upcount = 20;

    status = WiFi.status();
    if (status == WL_CONNECTED) {
        return;
    }
    while (status != WL_CONNECTED) {
        draw("?con");
        PRINT("Attempting to connect to SSID: ");
        PRINTLN(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }
    PRINTLN("Connected to WiFi");
    printWifiStatus();
    delay(2000);
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
    status = WiFi.status();
    PRINT("Disconnect: ");
    PRINTLN(status);
}
