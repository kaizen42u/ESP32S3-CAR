
#pragma once

#include <string.h>
#include <math.h>

#include "driver/rmt_tx.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#include "led_strip_encoder.h"

typedef struct
{
        union
        {
                struct
                {
                        uint8_t g, r, b;
                };
                uint8_t pixels[3];
        };

} ws2812_rgb_t;

typedef struct
{
        uint16_t h;
        uint8_t s;
        uint8_t v;
} __packed ws2812_hsv_t;

typedef struct
{
        ws2812_rgb_t color;
        rmt_channel_handle_t led_chan;
        rmt_encoder_handle_t led_encoder;
        rmt_transmit_config_t tx_config;
        uint32_t resolution_hz;
        gpio_num_t pin;
} ws2812_handle_t;

ws2812_handle_t *ws2812_default_config(ws2812_handle_t *handle);
ws2812_handle_t *ws2812_init(ws2812_handle_t *handle);
ws2812_rgb_t *ws2812_hsv2rgb(ws2812_hsv_t *hsv, ws2812_rgb_t *rgb);
void ws2812_set_rgb(ws2812_handle_t *handle, ws2812_rgb_t *rgb);
void ws2812_set_hsv(ws2812_handle_t *handle, ws2812_hsv_t *hsv);
void ws2812_update(ws2812_handle_t *handle);