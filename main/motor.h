
#pragma once

#include <string.h>

#include "driver/gpio.h"
#include "driver/mcpwm.h"
#include "driver/pulse_cnt.h"
#include "hal/mcpwm_ll.h"

#include "logging.h"
#include "mathop.h"

// Configures motor is reversed or not
typedef enum
{
        MOTOR_DIRECTION_DEFAULT, // Not reversed
        MOTOR_DIRECTION_REVERSED // Reversed
} motor_direction_t;

// All the resources that are used for the motor
typedef struct
{
        gpio_num_t ch_A_pin;                  // GPIO to channel A of motor driver
        gpio_num_t ch_B_pin;                  // GPIO to channel B of motor driver
        mcpwm_unit_t mcpwm_unit;              // MCPWM unit
        mcpwm_timer_t mcpwm_timer;            // MCPWM timer
        mcpwm_io_signals_t mcpwm_ch_A_signal; // MCPWM io signal for channel A
        mcpwm_io_signals_t mcpwm_ch_B_signal; // MCPWM io signal for channel B
        mcpwm_config_t mcpwm_config;          // MCPWM config
        motor_direction_t direction;          // Motor is reversed or not

        gpio_num_t pcnt_pin;                                   // GPIO to pulse counter
        pcnt_unit_config_t pcnt_unit_config;                   // Pulse counter unit config
        pcnt_unit_handle_t pcnt_unit_handle;                   // Pulse counter unit handle
        pcnt_channel_handle_t pcnt_channel_handle;             // Pulse counter channel config
        pcnt_glitch_filter_config_t pcnt_glitch_filter_config; // Pulse filter config
} motor_handle_t;

// Initialize the motor driver
void dc_motor_init(motor_handle_t *handle);

// Loads default settings of the motor driver
// Call only after `dc_motor_init()`
motor_handle_t *dc_motor_default_config(motor_handle_t *handle);

// Sets the direction of the motor
void dc_motor_set_direction(motor_handle_t *handle, motor_direction_t direction);
// Sets motor PWM regarding to the direction, normalized 0 - 100
void dc_motor_set_duty(motor_handle_t *handle, float duty_cycle);
// Sets the operating PWM frequency of the motor
void dc_motor_set_frequency(motor_handle_t *handle, uint32_t frequency);

// Sets the motor to go forward no matter the direction setting
void dc_motor_forward(motor_handle_t *handle, float duty_cycle);
// Sets the motor to go backward no matter the direction setting
void dc_motor_backward(motor_handle_t *handle, float duty_cycle);

// Sets the motor to go brake no matter the direction setting
void dc_motor_brake(motor_handle_t *handle);
// Sets the motor to go coast no matter the direction setting
void dc_motor_coast(motor_handle_t *handle);

// Get the motor pulse counter counts
int dc_motor_get_count(motor_handle_t *handle);

// Resets the motor pulse counter counts to 0
void dc_motor_clear_count(motor_handle_t *handle);
