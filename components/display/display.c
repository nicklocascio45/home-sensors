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

// Logger tag
static const char *TAG = "display";

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
        .height = 128,
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
    lv_display_t *display = lv_display_create(64, 128);
    lv_display_set_user_data(display, panel_handle);
    
    // Create and initialize draw buffer
    void *buf = NULL;
    size_t draw_buffer_sz = 64 * 128 / 8 + 8;
    buf = heap_caps_calloc(1, draw_buffer_sz, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(buf);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_I1);
    lv_display_set_buffers(display, buf, NULL, draw_buffer_sz, LV_DISPLAY_RENDER_MODE_FULL);

    // Set up lvgl callback

    return ESP_OK;
}
