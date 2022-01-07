// use only 1 core for demo purposes
#ifdef CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins
static const int led_pin[] = {25, 26, 27};

// Our task: blink an LED
void toggleLED(void *parameter) {
  int i = *((int *)parameter);
  for (;;) {
    digitalWrite(led_pin[i], HIGH);
    vTaskDelay((10 * i + 500) / portTICK_PERIOD_MS);
    digitalWrite(led_pin[i], LOW);
    vTaskDelay((10 * i + 500) / portTICK_PERIOD_MS);
  }
}

int params[] = {0, 1, 2};
char const * names[] = { "T0", "T1", "T2" };

void setup() {

  // Configure pins
  for (int i = 0; i < 3; i++)
    pinMode(led_pin[i], OUTPUT);

  // Tasks to run forever
  for (int i = 0; i < 3; i++)
    xTaskCreatePinnedToCore( // Use xTaskCreate() in vanilla FreeRTOS
      toggleLED,             // Function to be called
      names[i],              // Name of task
      1024,                  // Stack size (bytes in ESP32, words in FreeRTOS)
      &(params[i]),          // Parameter to pass to function
      1,                     // Task priority (0 to configMAX_PRIORITIES - 1)
      NULL,                  // Task handle
      app_cpu);              // Run on one core for demo purposes (ESP32 only)


  // If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler in
  // main after setting up your tasks.
}

void loop() {
  // put your main code here, to run repeatedly:

}
