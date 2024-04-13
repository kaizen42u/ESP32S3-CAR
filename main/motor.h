
#pragma once

#include <string.h>

#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "driver/pulse_cnt.h"
#include "hal/mcpwm_ll.h"

#include "logging.h"
#include "mathop.h"

typedef enum
{
        MOTOR_DIRECTION_DEFAULT,
        MOTOR_DIRECTION_REVERSED
} motor_direction_t;

typedef struct
{
        gpio_num_t ch_A_pin;
        gpio_num_t ch_B_pin;
        mcpwm_unit_t mcpwm_unit;
        mcpwm_timer_t mcpwm_timer;
        mcpwm_io_signals_t mcpwm_ch_A_signal;
        mcpwm_io_signals_t mcpwm_ch_B_signal;
        mcpwm_config_t mcpwm_config;
        // uint32_t mcpwm_group_frequency;
        // uint32_t mcpwm_timer_frequency;
        motor_direction_t direction;

        gpio_num_t pcnt_pin;
        pcnt_unit_config_t pcnt_unit_config;
        pcnt_unit_handle_t pcnt_unit_handle;
        pcnt_channel_handle_t pcnt_channel_handle;
        pcnt_glitch_filter_config_t pcnt_glitch_filter_config;
} motor_handle_t;

motor_handle_t *dc_motor_default_config(motor_handle_t *handle);
void dc_motor_init(motor_handle_t *handle);

void dc_motor_set_duty(motor_handle_t *handle, float duty_cycle);
void dc_motor_set_direction(motor_handle_t *handle, motor_direction_t direction);
void dc_motor_forward(motor_handle_t *handle, float duty_cycle);
void dc_motor_backward(motor_handle_t *handle, float duty_cycle);
void dc_motor_brake(motor_handle_t *handle);
void dc_motor_coast(motor_handle_t *handle);

int dc_motor_get_count(motor_handle_t *handle);
void dc_motor_clear_count(motor_handle_t *handle);
