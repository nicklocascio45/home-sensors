#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MOTION_SENSOR_PIN		6

// Logger tag
static const char *TAG = "app_main";

static volatile bool motion_detected = false;

static void IRAM_ATTR motion_sensor_isr_handler(void *arg)
{
	motion_detected = true;
}

void app_main(void)
{
    // Init pin
	gpio_config_t io_config = {
		.intr_type = GPIO_INTR_POSEDGE, // Trigger on rising edge
		.mode = GPIO_MODE_INPUT,
		.pin_bit_mask = (1ULL << MOTION_SENSOR_PIN),
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_DISABLE,
	};

	gpio_config(&io_config);

	// Install ISR service with default config
	gpio_install_isr_service(0);
	// Attach interrupt service routine
	gpio_isr_handler_add(MOTION_SENSOR_PIN, motion_sensor_isr_handler, NULL);

	ESP_LOGI(TAG, "PIR sensor initialized");

	while(1) {
		if (motion_detected) {
			motion_detected = false;
			ESP_LOGI(TAG, "MOTION!!");
		} else {
			ESP_LOGI(TAG, "Biz as usual");
		}

		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
}
