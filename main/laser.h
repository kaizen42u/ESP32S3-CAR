
#pragma once

#include <string.h>

#include "driver/ledc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

// All the resources that are used for laser PWM control
typedef struct
{
        ledc_timer_t timer;               // LEDC timer, 0 - 3
        ledc_channel_t channel;           // LEDC channel, 0 - 7
        gpio_num_t pin;                   // Output GPIO
        ledc_timer_bit_t duty_resolution; // PWM resolution, 1 - 14 bits
        uint32_t freq_hz;                 // PWM frequency
        uint32_t duty_max;                // Top value of the timer, based on the resolution
} laser_handle_t;

// Loads default settings of the laser PWM control
laser_handle_t *laser_default_config(laser_handle_t *handle);

// Creates the resources for laser PWM control
laser_handle_t *laser_init(laser_handle_t *handle, ledc_timer_t timer, ledc_channel_t channel, gpio_num_t pin);

// Sets PWM duty cycle, value normalized form 0 - 100
void laser_set_duty_cycle(laser_handle_t *handle, float duty_cycle);