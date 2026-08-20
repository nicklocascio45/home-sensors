#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

#include "pir.h"
#include "display.h"
#include "led.h"

// Logger tag
static const char *TAG = "app_main";

void app_main(void)
{
	// Set up shared event group
	EventGroupHandle_t motion_event_group;
	motion_event_group = xEventGroupCreate();

	esp_err_t esp_ret;

	// Set up PIR sensor
	esp_ret = pir_init(motion_event_group);
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Error setting up PIR sensor, aborting...");
		abort();
	}

	// Set up display
	esp_ret = display_init();
	if (esp_ret != ESP_OK) {
		ESP_LOGE(TAG, "Error setting up display, aborting...");
		abort();
	}

	// Start led task
	TaskHandle_t led_handle;
	xTaskCreate(led_task,
				"led_task",
				2048,
				(void *)motion_event_group,
				10,
				&led_handle);

	// Infinite loop now that everything is running
    while (1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
