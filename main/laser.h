
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
        uint32_t duty_max;
} laser_handle_t;

laser_handle_t *laser_default_config(laser_handle_t *handle);
laser_handle_t *laser_init(laser_handle_t *handle, ledc_timer_t timer, ledc_channel_t channel, gpio_num_t pin);
void laser_set_duty_cycle(laser_handle_t *handle, float duty_cycle);