#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include "pir.h"
#include "led.h"

// Logger tag
static const char *TAG = "pir";

static void IRAM_ATTR pir_isr_handler(void *arg)
{
	BaseType_t xHigherPriorityTaskWoken, xResult;
	EventGroupHandle_t motion_event_group = (EventGroupHandle_t)arg;

	xHigherPriorityTaskWoken = pdFALSE;
	xResult = xEventGroupSetBitsFromISR(motion_event_group, MOTION_BIT, &xHigherPriorityTaskWoken);

    if( xResult != pdFAIL ) {
        // If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch should be requested
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}

esp_err_t pir_init(EventGroupHandle_t motion_event_group)
{
    // Set up PIR pin config
	gpio_config_t pir_config = {
		.intr_type = GPIO_INTR_POSEDGE, // Trigger on rising edge
		.mode = GPIO_MODE_INPUT,
		.pin_bit_mask = (1ULL << PIR_PIN),
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en = GPIO_PULLUP_DISABLE,
	};
	gpio_config(&pir_config);

	// Install ISR service with default config
	gpio_install_isr_service(0);

	// Attach interrupt service routine
	gpio_isr_handler_add(PIR_PIN, pir_isr_handler, motion_event_group);

	ESP_LOGI(TAG, "PIR sensor initialized");

	return ESP_OK;
}
