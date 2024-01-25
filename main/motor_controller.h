
#pragma once

#include "button.h"
#include "pindef.h"
#include "motor.h"

#include "esp_log.h"

typedef struct
{
        motor_handle_t *left;
        motor_handle_t *right;
        gpio_num_t mcpwm_en;
} motor_controller_handle_t;

bool is_motor_control_pins(gpio_num_t pin);

void motor_controller_config_mcpwm_enable(motor_controller_handle_t *handle);
void motor_controller_set_mcpwm_enable(motor_controller_handle_t *handle);
void motor_controller_clear_mcpwm_enable(motor_controller_handle_t *handle);

motor_controller_handle_t *motor_controller_default_config(motor_controller_handle_t *handle);
motor_controller_handle_t *motor_controller_init(motor_controller_handle_t *handle);
void motor_controller(motor_controller_handle_t *handle, button_event_t *event);
void motor_controller_stop_all(motor_controller_handle_t *handle);
void motor_controller_print_counts(motor_controller_handle_t *handle);
