#include <stdio.h>
#include <unistd.h>
#include <sys/lock.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "lvgl.h"

#include "display.h"
#include "pir.h"

#define LCD_H_RES						128
#define LCD_V_RES						64

#define LVGL_TICK_PERIOD_MS				5
#define LVGL_TASK_MAX_DELAY_MS 			500
#define LVGL_TASK_MIN_DELAY_MS 			1000 / CONFIG_FREERTOS_HZ
#define LVGL_TIMER_TASK_STACK_SIZE		(4 * 1024)
#define LVGL_TIMER_TASK_PRIORITY		2

#define MOTION_DISPLAY_TIME				3

// Logger tag
static const char *TAG = "display";

lv_display_t *display;

// To use LV_COLOR_FORMAT_I1, we need an extra buffer to hold the converted data
static uint8_t oled_buffer[LCD_H_RES * LCD_V_RES / 8];
// LVGL library is not thread-safe, use a mutex to protect it
static _lock_t lvgl_api_lock;

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t io_panel, esp_lcd_panel_io_event_data_t *edata, void *user_ctx)
{
	lv_display_t *disp = (lv_display_t *)user_ctx;
	lv_display_flush_ready(disp);
	return false;
}

static void lvgl_flush_callback(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    esp_lcd_panel_handle_t panel_handle = lv_display_get_user_data(disp);

    px_map += 8; // LVGL reservers 2 x 4 bytes in buffer, assumed to be used as a palette

    uint16_t hor_res = lv_display_get_physical_horizontal_resolution(disp);
    int x1 = area->x1;
    int x2 = area->x2;
    int y1 = area->y1;
    int y2 = area->y2;

    for (int y = y1; y < y2; y++) {
        for (int x = x1; x < x2; x++) {
			bool chroma_color = (px_map[(hor_res >> 3) * y + (x >> 3)] & 1 << (7 - x % 8));

			uint8_t *buf = oled_buffer + hor_res * (y >> 3) + (x);
			if (chroma_color) {
				(*buf) &= ~(1 << (y % 8));
			} else {
				(*buf) |= (1 << (y % 8));
			}
		}
	}

	// pass the draw buffer to the driver
	esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1, oled_buffer);
}

static void increase_lvgl_tick(void *arg)
{
	lv_tick_inc(LVGL_TICK_PERIOD_MS);
}

static void lvgl_timer_task(void *arg)
{
	uint32_t time_till_next_ms = 0;
	uint32_t time_threshold_ms = 2000 / CONFIG_FREERTOS_HZ;
	while (1) {
		_lock_acquire(&lvgl_api_lock);
		time_till_next_ms = lv_timer_handler();
		_lock_release(&lvgl_api_lock);

		if (time_till_next_ms == LV_NO_TIMER_READY) {
			time_till_next_ms = time_threshold_ms;
		}

		// in case of triggering a task watch dog time out
		time_till_next_ms = MAX(time_till_next_ms, time_threshold_ms);
		vTaskDelay(pdMS_TO_TICKS(time_till_next_ms));
	}
}

esp_err_t display_init(void)
{
    esp_err_t esp_ret;

    // Initialize I2C bus
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .i2c_port = 0, // make a constant, figure out why it's this value first though?
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .flags.enable_internal_pullup = true,
    };
    esp_ret = i2c_new_master_bus(&bus_config, &i2c_bus);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize I2C bus");
        return ESP_FAIL;
    }

    // Install panel IO
    esp_lcd_panel_io_handle_t io_handle = NULL;
    // make all hardcoded values constants, find in datasheet
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x3C,
        .scl_speed_hz = (400 * 1000),
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6,
    };
    esp_ret = esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install panel IO");
        return ESP_FAIL;
    }

    // Install SSD1306 panel driver
    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = LCD_V_RES,
    };
    panel_config.vendor_config = &ssd1306_config;
    esp_ret = esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install SSD1306 panel driver");
        return ESP_FAIL;
    }

    // Initialize panel
    esp_ret = esp_lcd_panel_reset(panel_handle);
    esp_ret = esp_lcd_panel_init(panel_handle);
    esp_ret = esp_lcd_panel_disp_on_off(panel_handle, true);
    ESP_LOGI(TAG, "Display initialized");

    // Initialize LVGL display
    lv_init();
    display = lv_display_create(LCD_H_RES, LCD_V_RES);
    lv_display_set_user_data(display, panel_handle);

    // Create and initialize draw buffer
    void *buf = NULL;
    size_t draw_buffer_sz = LCD_H_RES * LCD_V_RES / 8 + 8;
    buf = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_I1);
    lv_display_set_buffers(display, buf, NULL, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_FULL);
	ESP_LOGI(TAG, "Draw buffer initialized");

	// Set up lvgl flush callback
	lv_display_set_flush_cb(display, lvgl_flush_callback);

	// Set up flush ready notification
	const esp_lcd_panel_io_callbacks_t cbs = {
		.on_color_trans_done = notify_lvgl_flush_ready,
	};
	esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, display);

	// Set up timer
	const esp_timer_create_args_t lvgl_tick_timer_args = {
		.callback = &increase_lvgl_tick,
		.name = "lvgl_tick",
	};
	esp_timer_handle_t lvgl_tick_timer = NULL;
	esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
	esp_timer_start_periodic(lvgl_tick_timer, LVGL_TICK_PERIOD_MS * 1000);
	ESP_LOGI(TAG, "Callbacks registered and timer started");

	// Start timer task
	xTaskCreate(lvgl_timer_task, "lvgl_timer_task", LVGL_TIMER_TASK_STACK_SIZE, NULL, LVGL_TIMER_TASK_PRIORITY, NULL);

	return ESP_OK;
}

void display_task(void *pvParameters)
{
	EventGroupHandle_t motion_event_group = (EventGroupHandle_t)pvParameters;

	while(1) {
		EventBits_t motion_bits = xEventGroupWaitBits(motion_event_group,
													  MOTION_BIT,
													  pdTRUE,
													  pdFALSE,
													  pdMS_TO_TICKS(10000));

		if ((motion_bits & MOTION_BIT) != 0) {
			_lock_acquire(&lvgl_api_lock);
			lv_obj_t *scr = lv_display_get_screen_active(display);
			lv_obj_t *label = lv_label_create(scr);
			lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
			lv_label_set_text(label, "We got motion");
			lv_obj_set_width(label, lv_display_get_horizontal_resolution(display));
			lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
			_lock_release(&lvgl_api_lock);

			vTaskDelay(MOTION_DISPLAY_TIME * 1000 / portTICK_PERIOD_MS);

			_lock_acquire(&lvgl_api_lock);
			lv_label_set_text(label, "");
			_lock_release(&lvgl_api_lock);
		}
	}
}
