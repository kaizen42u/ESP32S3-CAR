
#pragma once

#include "button.h"
#include "pindef.h"
#include "motor.h"
#include "pid.h"
#include "ringbuffer.h"
#include "logging.h"

#include "esp_log.h"

#define DISTANCE_DIFFERENCE_INTEGRATE_SAMPLES (50)
#define VELOCITY_INTETGRATE_SAMPLES (50)
#define ACCELERATION_INTETGRATE_SAMPLES (50)

typedef enum
{
        DIRECTION_NONE,
        DIRECTION_COAST,
        DIRECTION_BRAKE,
        DIRECTION_FORWARD,
        DIRECTION_BACKWARD,
        DIRECTION_TURN_LEFT,
        DIRECTION_TURN_RIGHT,
        DIRECTION_MAX,
} direction_t;

static const char __attribute__((unused)) * DIRECTION_STRING[] = {
    "DIRECTION_NONE",
    "DIRECTION_COAST",
    "DIRECTION_BRAKE",
    "DIRECTION_FORWARD",
    "DIRECTION_BACKWARD",
    "DIRECTION_TURN_LEFT",
    "DIRECTION_TURN_RIGHT",
    "DIRECTION_MAX"};

typedef struct
{
        motor_handle_t *left_motor_handle, *right_motor_handle;
        pid_handle_t *left_motor_pid_handle, *right_motor_pid_handle;
        ringbuffer_handle_t *left_velocity_rb_handle, *right_velocity_rb_handle;
        ringbuffer_handle_t *left_acceleration_rb_handle, *right_acceleration_rb_handle;

        pid_handle_t *distance_difference_pid_handle;
        gpio_num_t mcpwm_en;
        direction_t direction;
} motor_controller_handle_t;

bool is_motor_control_pins(gpio_num_t pin);

void motor_controller_config_mcpwm_enable(motor_controller_handle_t *handle);
void motor_controller_set_mcpwm_enable(motor_controller_handle_t *handle);
void motor_controller_clear_mcpwm_enable(motor_controller_handle_t *handle);

motor_controller_handle_t *motor_controller_default_config(motor_controller_handle_t *handle);
motor_controller_handle_t *motor_controller_init(motor_controller_handle_t *handle);
void motor_controller(motor_controller_handle_t *handle, button_event_t *event);
void motor_controller_open(motor_controller_handle_t *handle, button_event_t *event);

void motor_controller_stop_all(motor_controller_handle_t *handle);
void motor_controller_print_stat(motor_controller_handle_t *handle);