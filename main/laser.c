
#include "laser.h"

static const char *TAG = "laser";

laser_handle_t *laser_default_config(laser_handle_t *handle)
{
        memset(handle, 0, sizeof(laser_handle_t));
        handle->timer = LEDC_TIMER_1;
        handle->channel = LEDC_CHANNEL_4;
        handle->pin = 0;
        handle->duty_resolution = LEDC_TIMER_10_BIT;
        handle->freq_hz = 10000.0F;
        handle->duty_max = (1UL << handle->duty_resolution) - 1;
        return handle;
}

laser_handle_t *laser_init(laser_handle_t *handle, ledc_timer_t timer, ledc_channel_t channel, gpio_num_t pin)
{
        handle->timer = timer;
        handle->channel = channel;
        handle->pin = pin;

        ESP_LOGI(TAG, "GPIO[%d]| LEDC timer: %d| LEDC channel: %d", handle->pin, handle->timer, handle->channel);

        // Configure the pins
        gpio_config_t io_conf = {
            .pin_bit_mask = (1UL << handle->pin), // we just need to set one pin
            .mode = GPIO_MODE_OUTPUT,             // output only
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&io_conf));

        // Prepare and then apply the LEDC PWM timer configuration
        ledc_timer_config_t ledc_timer = {
            .speed_mode = LEDC_LOW_SPEED_MODE, // only low speed mode on ESP32-S3
            .duty_resolution = handle->duty_resolution,
            .timer_num = handle->timer,
            .freq_hz = handle->freq_hz,
            .clk_cfg = LEDC_AUTO_CLK,
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        // Prepare and then apply the LEDC PWM channel configuration
        ledc_channel_config_t ledc_channel = {
            .gpio_num = handle->pin,
            .speed_mode = LEDC_LOW_SPEED_MODE,
            .channel = handle->channel,
            .intr_type = LEDC_INTR_DISABLE,
            .timer_sel = handle->timer,
            .duty = 0,   // output low on duty
            .hpoint = 0, // output high on 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

        return handle;
}

void laser_set_duty_cycle(laser_handle_t *handle, float duty_cycle)
{
        uint32_t new_duty = duty_cycle * handle->duty_max / 100.0f;
        // ESP_LOGV(TAG, "duty: %lu", new_duty);
        //  Set duty
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, handle->channel, new_duty));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, handle->channel));
}