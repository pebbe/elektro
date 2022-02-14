
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

#include "WiFi.h"
#include <time.h>
#include "esp_event.h"
#include "esp_sntp.h"
#include "mqtt_client.h"

#include "secret.h"

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

const char *TOPIC_UP = "esp32/demo11/up";
const char *TOPIC_BLINK = "esp32/demo11/blink";
const char *TOPIC_LEVEL = "esp32/demo11/level";

const char *dag[] = { "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" };
const char *maand[] = { "januari", "februari", "maart", "april", "mei", "juni", "juli", "augustus", "september", "oktober", "november", "december" };

static QueueHandle_t blinkQueue;
static QueueHandle_t timeQueue;

void blink(void *not_used) {
    int period = 1000;
    bool blinkOn = true;
    int value;
    for (;;) {
        if (xQueueReceive(blinkQueue, &value, period) == pdTRUE) {
            if (value < 1)
                value = 1;
            // deciseconds -> milliseconds
            period = value * 100;
        } else {
            blinkOn = !blinkOn;
            digitalWrite(LEDRED, blinkOn ? HIGH : LOW);
            digitalWrite(LEDBLUE, blinkOn ? LOW : HIGH);
        }
    }
}

static void mqtt_event_connected(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;

    esp_mqtt_client_publish(client, TOPIC_UP, "up!", 0, 1, 1);

    esp_mqtt_client_subscribe(client, TOPIC_BLINK, 1);
    esp_mqtt_client_subscribe(client, TOPIC_LEVEL, 1);

    int i;
    while (xQueueReceive(timeQueue, &i, 100) != pdTRUE)
        ;
    time_t now;
    time(&now);
    struct tm* t = localtime(&now);
    char s[30];
    strftime(s, sizeof(s), "%d-%m-%Y %H:%M:%S", t);
    esp_mqtt_client_publish(client, TOPIC_UP, s, 0, 1, 1);
}

static void mqtt_event_data(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    int tn = event->topic_len;
    int dn = event->data_len;
    char topic[tn + 1];
    char data[dn + 1];

    strncpy(topic, event->topic, tn);
    strncpy(data, event->data, dn);
    topic[tn] = '\0';
    data[dn] = '\0';
    int i = atoi(data);
    if (!strcmp(topic, TOPIC_BLINK)) {
        if (i < 1)
            i = 1;
        xQueueSendToBack(blinkQueue, (void *) &i, (TickType_t) 0);
    } else if (!strcmp(topic, TOPIC_LEVEL)) {
        if (i < 0)
            i = 0;
        if (i > 255)
            i = 255;
        analogWrite(LEDYELLOW, i);
    }
}

esp_mqtt_client_config_t mqtt_cfg;
void mqtt_start(void) {
    mqtt_cfg.uri = "mqtt://rpi-zero-2.fritz.box:1883";
    mqtt_cfg.lwt_topic = TOPIC_UP;
    mqtt_cfg.lwt_msg = "----";
    mqtt_cfg.lwt_qos = 1;
    mqtt_cfg.lwt_retain = 1;

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED, mqtt_event_connected, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt_event_data, NULL);
    esp_mqtt_client_start(client);
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

void setup()
{
    pinMode(LEDRED, OUTPUT);
    pinMode(LEDBLUE, OUTPUT);
    pinMode(LEDYELLOW, OUTPUT); // niet nodig?

    blinkQueue = xQueueCreate(10, sizeof(int));
    timeQueue = xQueueCreate(1, sizeof(int));

#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial)
        ;
#endif

    PRINTLN("=============");
    PRINTLN("arduino/demo2");
    PRINTLN("=============");

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
        vTaskDelay(100);
        PRINT(".");
    }
    PRINTLN(" CONNECTED");

    if (xTaskCreatePinnedToCore(blink, "blink", 1024, NULL, 1, NULL, 1) != pdPASS)
        PRINTLN("Creating blink task failed!");

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    char fbox[] = "fritz.box";
    sntp_setservername(0, fbox);
    sntp_set_sync_interval((uint32_t) 60000);
    sntp_init();

    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED)
        vTaskDelay(10);
    int i = 1;
    xQueueSendToBack(timeQueue, (void *) &i, (TickType_t) 0);

    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);

    mqtt_start();
}

void loop()
{
    time_t now;
    time(&now);
    struct tm* t = localtime(&now);
    PRINTF("%s %d %s %d, %d:%02d:%02d %s\n",
           dag[t->tm_wday],
           t->tm_mday, maand[t->tm_mon], 1900 + t->tm_year,
           t->tm_hour, t->tm_min, t->tm_sec,
           t->tm_isdst ? "CEST" : "CET");

    vTaskDelay(20000 / portTICK_PERIOD_MS);
}
