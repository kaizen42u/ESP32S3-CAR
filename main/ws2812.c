
#include "ws2812.h"

static const char *TAG = "ws2812";

ws2812_handle_t *ws2812_default_config(ws2812_handle_t *handle)
{
        memset(handle, 0, sizeof(ws2812_handle_t));
        handle->pin = 48;
        handle->resolution_hz = 10000000; // 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
        return handle;
}

ws2812_handle_t *ws2812_init(ws2812_handle_t *handle)
{
        ESP_LOGI(TAG, "Create RMT TX channel");
        rmt_tx_channel_config_t tx_chan_config = {
            .clk_src = RMT_CLK_SRC_DEFAULT, // select source clock
            .gpio_num = handle->pin,
            .mem_block_symbols = 64, // increase the block size can make the LED less flickering
            .resolution_hz = handle->resolution_hz,
            .trans_queue_depth = 4, // set the number of transactions that can be pending in the background
        };
        ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &handle->led_chan));

        ESP_LOGI(TAG, "Install led strip encoder");
        led_strip_encoder_config_t encoder_config = {
            .resolution = handle->resolution_hz,
        };
        ESP_ERROR_CHECK(rmt_new_led_strip_encoder(&encoder_config, &handle->led_encoder));

        ESP_LOGI(TAG, "Enable RMT TX channel");
        ESP_ERROR_CHECK(rmt_enable(handle->led_chan));

        rmt_transmit_config_t tx_config = {
            .loop_count = 0, // no transfer loop
        };
        handle->tx_config = tx_config;
        return handle;
}

ws2812_rgb_t *ws2812_hsv2rgb(ws2812_hsv_t *hsv, ws2812_rgb_t *rgb)
{
        if (hsv->h >= 360)
                hsv->h -= 360;
        if (hsv->s > 100)
                hsv->s = 100;
        if (hsv->v > 100)
                hsv->v = 100;
        uint32_t rgb_max = hsv->v * 2.55f;
        uint32_t rgb_min = rgb_max * (100 - hsv->s) / 100.0f;

        uint32_t i = hsv->h / 60;
        uint32_t diff = fmodf(hsv->h, 60);

        // RGB adjustment amount by hue
        uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

        switch (i)
        {
        case 0:
                rgb->r = rgb_max;
                rgb->g = rgb_min + rgb_adj;
                rgb->b = rgb_min;
                break;
        case 1:
                rgb->r = rgb_max - rgb_adj;
                rgb->g = rgb_max;
                rgb->b = rgb_min;
                break;
        case 2:
                rgb->r = rgb_min;
                rgb->g = rgb_max;
                rgb->b = rgb_min + rgb_adj;
                break;
        case 3:
                rgb->r = rgb_min;
                rgb->g = rgb_max - rgb_adj;
                rgb->b = rgb_max;
                break;
        case 4:
                rgb->r = rgb_min + rgb_adj;
                rgb->g = rgb_min;
                rgb->b = rgb_max;
                break;
        default:
                rgb->r = rgb_max;
                rgb->g = rgb_min;
                rgb->b = rgb_max - rgb_adj;
                break;
        }
        return rgb;
}

void ws2812_set_rgb(ws2812_handle_t *handle, ws2812_rgb_t *rgb)
{
        handle->color = *rgb;
}

void ws2812_set_hsv(ws2812_handle_t *handle, ws2812_hsv_t *hsv)
{
        ws2812_rgb_t rgb = {.r = 0, .g = 0, .b = 0};
        ws2812_hsv2rgb(hsv, &rgb);
        ws2812_set_rgb(handle, &rgb);
}

void ws2812_update(ws2812_handle_t *handle)
{
        ESP_ERROR_CHECK(rmt_transmit(handle->led_chan, handle->led_encoder, handle->color.pixels, sizeof(handle->color.pixels), &handle->tx_config));
}