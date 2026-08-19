#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "led.h"

#define BLINK_COUNT				3
#define BLINK_DURATION_SEC		0.5

// Logger tag
static const char *TAG = "led";

void blink_light(void)
{
	for (int i = 0; i < BLINK_COUNT; i++) {
		gpio_set_level(LED_PIN, 1);
		vTaskDelay(BLINK_DURATION_SEC * 1000 / portTICK_PERIOD_MS);
		gpio_set_level(LED_PIN, 0);
		vTaskDelay(BLINK_DURATION_SEC * 1000 / portTICK_PERIOD_MS);
	}
}

void led_task(void *pvParameters)
{
    EventGroupHandle_t motion_event_group = (EventGroupHandle_t)pvParameters;

    // Reset LED pin
    gpio_reset_pin(LED_PIN);

    // Set LED GPIO direction
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Start LED on low
    gpio_set_level(LED_PIN, 0);

    // Enter event loop
    while (1) {
        EventBits_t motion_bits = xEventGroupWaitBits(motion_event_group,
													  MOTION_BIT,
													  pdTRUE,
													  pdFALSE,
													  pdMS_TO_TICKS(10000));

		if ((motion_bits & MOTION_BIT) != 0) {
			ESP_LOGI(TAG, "Motion detected!");
			blink_light();
		}
	}
}
