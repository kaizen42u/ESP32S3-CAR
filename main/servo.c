
#include "servo.h"

static const char *TAG = "servo";

servo_handle_t *servo_default_config(servo_handle_t *handle)
{
        memset(handle, 0, sizeof(servo_handle_t));
        handle->timer = LEDC_TIMER_0;
        handle->channel = LEDC_CHANNEL_0;
        handle->pin = 0;
        handle->duty_resolution = LEDC_TIMER_14_BIT; // 14bit is what we can have at most
        handle->freq_hz = 50.0F;                     // a normal servo is polling at 50 Hz (20ms)
        handle->angle_offset = 0.0F;
        handle->angle_to_duty_bias = (((1UL << handle->duty_resolution) - 1) / 20); // 1ms on time, total 20ms
        handle->angle_to_duty_weight = (handle->angle_to_duty_bias / 90);           // 90 degree = 1ms more on time
        return handle;
}

servo_handle_t *servo_init(servo_handle_t *handle, ledc_timer_t timer, ledc_channel_t channel, gpio_num_t pin)
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
            .duty = handle->angle_to_duty_bias, // output low on duty
            .hpoint = 0,                        // output high on 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

        return handle;
}

uint32_t angle_to_duty(servo_handle_t *handle, float angle)
{
        return handle->angle_to_duty_weight * (angle + handle->angle_offset) + handle->angle_to_duty_bias;
}

void servo_set_angle(servo_handle_t *handle, float angle)
{
        uint32_t new_duty = angle_to_duty(handle, angle);
        ESP_LOGV(TAG, "angle: %f : duty: %lu", angle, new_duty);
        // Set duty
        ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, handle->channel, new_duty));
        // Update duty to apply the new value
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, handle->channel));
}