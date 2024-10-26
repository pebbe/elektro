#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_chip_info.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include <string.h>

#include "secret.h"

#define EXAMPLE_ESP_WIFI_SSID SECRET_SSID
#define EXAMPLE_ESP_WIFI_PASS SECRET_PASS
#define EXAMPLE_ESP_MAXIMUM_RETRY 20

#define LEDRED 25
#define LEDBLUE 26
#define LEDYELLOW 27

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about
 * two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *WIFITAG = "wifi station";
static const char *MQTTTAG = "mqtt client";
static const char *TIMETAG = "time";

static const char *TOPIC_UP = "esp32/demo11/up";
static const char *TOPIC_BLINK = "esp32/demo11/blink";
static const char *TOPIC_LEVEL = "esp32/demo11/level";

static int s_retry_num = 0;

static QueueHandle_t blinkQueue;
static QueueHandle_t timeQueue;

void blink(void *not_used) {
    int period = 1000 / portTICK_PERIOD_MS;
    bool blinkOn = true;
    int value;
    for (;;) {
        if (xQueueReceive(blinkQueue, &value, period) == pdTRUE) {
            if (value < 1)
                value = 1;
            // deciseconds -> milliseconds
            period = value * 100 / portTICK_PERIOD_MS;
        } else {
            blinkOn = !blinkOn;
            gpio_set_level(LEDRED, blinkOn ? 1 : 0);
            gpio_set_level(LEDBLUE, blinkOn ? 0 : 1);
        }
    }
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(WIFITAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(WIFITAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(WIFITAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL,
        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL,
        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta =
            {
                .ssid = EXAMPLE_ESP_WIFI_SSID,
                .password = EXAMPLE_ESP_WIFI_PASS,
                /* Setting a password implies station will connect to all
                 * security modes including WEP/WPA. However these modes are
                 * deprecated and not advisable to be used. Incase your Access
                 * point doesn't support WPA2, these mode can be enabled by
                 * commenting below line */
                .threshold.authmode = WIFI_AUTH_WPA2_PSK,

                .pmf_cfg = {.capable = true, .required = false},
            },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WIFITAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT)
     * or connection failed for the maximum number of re-tries (WIFI_FAIL_BIT).
     * The bits are set by wifi_event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we
     * can test which event actually happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(WIFITAG, "connected to ap SSID:%s", EXAMPLE_ESP_WIFI_SSID);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(WIFITAG, "Failed to connect to SSID:%s",
                 EXAMPLE_ESP_WIFI_SSID);
    } else {
        ESP_LOGE(WIFITAG, "UNEXPECTED EVENT");
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
        WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

static void mqtt_event_connected(void *handler_args, esp_event_base_t base,
                                 int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    esp_mqtt_client_publish(client, TOPIC_UP, "up!", 0, 1, 1);

    esp_mqtt_client_subscribe(client, TOPIC_BLINK, 1);
    esp_mqtt_client_subscribe(client, TOPIC_LEVEL, 1);

    int i;
    while (xQueueReceive(timeQueue, &i, 100) != pdTRUE)
        ;
    time_t now;
    time(&now);
    struct tm *t = localtime(&now);
    char s[30];
    strftime(s, sizeof(s), "%d-%m-%Y %H:%M:%S", t);
    esp_mqtt_client_publish(client, TOPIC_UP, s, 0, 1, 1);
}

static void mqtt_event_data(void *handler_args, esp_event_base_t base,
                            int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;
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
        esp_err_t ret = xQueueSendToBack(blinkQueue, (void *)&i, (TickType_t)0);
        if (ret != pdPASS) {
            ESP_ERROR_CHECK(ret);
        }
    } else if (!strcmp(topic, TOPIC_LEVEL)) {
        if (i < 0)
            i = 0;
        if (i > 255)
            i = 255;
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, i));
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
    }
}

void mqtt_start(void) {
    esp_mqtt_client_config_t mqtt_cfg = {.broker.address.uri =
                                             "mqtt://192.168.178.24:1883",
                                         .session.last_will.topic = TOPIC_UP,
                                         .session.last_will.msg = "----",
                                         .session.last_will.qos = 1,
                                         .session.last_will.retain = 1};

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_CONNECTED,
                                   mqtt_event_connected, NULL);
    esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt_event_data,
                                   NULL);
    esp_mqtt_client_start(client);
}

static void ledc_init(void) {
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                      .timer_num = LEDC_TIMER_0,
                                      .duty_resolution = LEDC_TIMER_8_BIT,
                                      .freq_hz =
                                          5000, // Set output frequency at 5 kHz
                                      .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {.speed_mode = LEDC_LOW_SPEED_MODE,
                                          .channel = LEDC_CHANNEL_0,
                                          .timer_sel = LEDC_TIMER_0,
                                          .intr_type = LEDC_INTR_DISABLE,
                                          .gpio_num = LEDYELLOW,
                                          .duty = 0, // Set duty to 0%
                                          .hpoint = 0};
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void app_main(void) {
    gpio_set_direction(LEDRED, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(LEDBLUE, GPIO_MODE_DEF_OUTPUT);
    // gpio_set_direction(LEDYELLOW, GPIO_MODE_DEF_OUTPUT);

    ledc_init();

    blinkQueue = xQueueCreate(10, sizeof(int));
    timeQueue = xQueueCreate(1, sizeof(int));

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    printf("==============\n");
    printf("espressif/demo\n");
    printf("==============\n");

    // Print some info
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ", CONFIG_IDF_TARGET,
           chip_info.cores, (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
    printf("silicon revision %d, ", chip_info.revision);
    /*
    printf("%dMB %s flash\n", size_flash_chip / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded"
                                                         : "external");
                                                         */
    printf("Minimum free heap size: %lu bytes\n",
           esp_get_minimum_free_heap_size());
    fflush(stdout);

    // Start WiFi
    ESP_LOGI(WIFITAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();

    BaseType_t bt = xTaskCreatePinnedToCore(blink, "blink", 1024, NULL, 1,
                                            NULL /* &blinkHandle */, 1);
    if (bt != pdPASS) {
        ESP_ERROR_CHECK(bt);
    }

    ESP_LOGI(MQTTTAG, "MQTT_START");
    mqtt_start();

    setenv("TZ", "CET-1CEST,M3.5.0/2,M10.5.0/3", 1);
    tzset();

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "nl.pool.ntp.org");
    esp_sntp_set_sync_interval(60000);
    esp_sntp_init();

    while (sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED)
        vTaskDelay(100);
    int i = 1;
    xQueueSendToBack(timeQueue, (void *)&i, (TickType_t)0);

    time_t now;
    char strftime_buf[64];
    struct tm timeinfo;
    for (;;) {
        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%a %e %b %Y  %H:%M:%S",
                 &timeinfo);
        ESP_LOGI(TIMETAG, "%s", strftime_buf);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
