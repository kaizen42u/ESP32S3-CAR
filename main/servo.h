
#pragma once

#include <string.h>

#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

typedef struct
{
        ledc_timer_t timer;
        ledc_channel_t channel;
        gpio_num_t pin;
        ledc_timer_bit_t duty_resolution;
        uint32_t freq_hz;
        float angle_offset;

        uint32_t angle_to_duty_bias;
        float angle_to_duty_weight;
} servo_handle_t;

servo_handle_t *servo_default_config(servo_handle_t *handle);
servo_handle_t *servo_init(servo_handle_t *handle, ledc_timer_t timer, ledc_channel_t channel, gpio_num_t pin);
void servo_set_angle(servo_handle_t *handle, float angle);