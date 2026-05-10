/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Waveshare ESP32-S3-LCD-1.47B — board-specific device factories.
 *
 * The 1.47" ST7789 SPI panel (172x320) sits on a physical 240-column
 * frame, so the visible window is offset by X=34 columns (Y=0).
 * This factory applies that gap after the generic driver creates the
 * panel handle, avoiding any need for a custom i80 or full-custom
 * implementation.
 *
 * Pin mapping (from Waveshare schematic / demo source):
 *   MOSI  : GPIO45   SCLK : GPIO40   CS   : GPIO42
 *   DC    : GPIO41   RST  : GPIO39   BL   : GPIO46 (LEDC PWM)
 *   SCL   : GPIO47   SDA  : GPIO48   (I2C — QMI8658 IMU)
 *   RGB   : GPIO38   (WS2812B single LED)
 *   BOOT  : GPIO0    BAT  : GPIO1 (ADC)
 *   SD    : CLK=14 CMD=15 D0=16 D1=18 D2=17 D3=21
 */

#include <string.h>
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

static const char *TAG = "WS_LCD147B_SETUP";

/* The ST7789 physical frame is 240 columns wide.
 * The 172-column visible area starts at column 34. */
#define LCD147B_LCD_OFFSET_X  34
#define LCD147B_LCD_OFFSET_Y  0

esp_err_t lcd_panel_factory_entry_t(esp_lcd_panel_io_handle_t io,
                                    const esp_lcd_panel_dev_config_t *panel_dev_config,
                                    esp_lcd_panel_handle_t *ret_panel)
{
    esp_lcd_panel_dev_config_t panel_dev_cfg = {0};
    memcpy(&panel_dev_cfg, panel_dev_config, sizeof(esp_lcd_panel_dev_config_t));

    esp_err_t ret = esp_lcd_new_panel_st7789(io, &panel_dev_cfg, ret_panel);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_new_panel_st7789 failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_lcd_panel_set_gap(*ret_panel, LCD147B_LCD_OFFSET_X, LCD147B_LCD_OFFSET_Y);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "esp_lcd_panel_set_gap failed: %s", esp_err_to_name(ret));
    }

    ESP_LOGI(TAG, "ST7789 panel ready (172x320, X offset %d)", LCD147B_LCD_OFFSET_X);
    return ESP_OK;
}
