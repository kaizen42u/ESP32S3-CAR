
#pragma once

#include "button.h"
#include "pindef.h"
#include "motor.h"
#include "pid.h"
#include "ringbuffer.h"
#include "logging.h"

#include "packets.h"
#include "esp_log.h"
#include "info.h"

#define DISTANCE_DIFFERENCE_INTEGRATE_SAMPLES (5)
#define VELOCITY_INTEGRATE_SAMPLES (5)
#define ACCELERATION_INTEGRATE_SAMPLES (5)

// The motor tank directions
typedef enum
{
        DIRECTION_NONE,       // Not specified
        DIRECTION_COAST,      // Coasting
        DIRECTION_BRAKE,      // Braking
        DIRECTION_FORWARD,    // Moving forward
        DIRECTION_BACKWARD,   // Moving backward
        DIRECTION_TURN_LEFT,  // Turning left
        DIRECTION_TURN_RIGHT, // Turning Right
        DIRECTION_MAX,
} motor_tank_direction_t;

static const char __attribute__((unused)) * DIRECTION_STRING[] = {
    "DIRECTION_NONE",
    "DIRECTION_COAST",
    "DIRECTION_BRAKE",
    "DIRECTION_FORWARD",
    "DIRECTION_BACKWARD",
    "DIRECTION_TURN_LEFT",
    "DIRECTION_TURN_RIGHT",
    "DIRECTION_MAX"};

// All the resources that are used for the motor controller
typedef struct
{
        motor_handle_t *left_motor_handle, *right_motor_handle;                          // Motor handlers
        pid_handle_t *left_motor_pid_handle, *right_motor_pid_handle;                    // Motor PID handlers
        ringbuffer_handle_t *left_velocity_rb_handle, *right_velocity_rb_handle;         // Motor speed sense handlers
        ringbuffer_handle_t *left_acceleration_rb_handle, *right_acceleration_rb_handle; // Motor accel sense handlers
        pid_handle_t *distance_difference_pid_handle;                                    // Motor diff PID handler
        gpio_num_t mcpwm_en;                                                             // Motor controller enable pin
        motor_tank_direction_t direction;                                                // Motor tank direction
} motor_controller_handle_t;

// Returns true if a button is used to control the motor, use GPIO number as id
bool is_motor_control_buttons(gpio_num_t pin);

// Configure motor driver enable pin
void motor_controller_config_mcpwm_enable(motor_controller_handle_t *handle);
// Enable motor drivers
void motor_controller_set_mcpwm_enable(motor_controller_handle_t *handle);
// Disable motor drivers
void motor_controller_clear_mcpwm_enable(motor_controller_handle_t *handle);

// Initialize the motor controllers
motor_controller_handle_t *motor_controller_init(motor_controller_handle_t *handle);

// Loads default settings of the motor controllers
// Call only after `motor_controller_init()`
motor_controller_handle_t *motor_controller_default_config(motor_controller_handle_t *handle);

// Update motor feedback (speed)
void update_feedback(motor_controller_handle_t *handle);

// Update motor PIDs, 2 inner PID and 1 outer PID
void update_pid(motor_controller_handle_t *handle);

// Decode button into direction
void read_buttons(motor_controller_handle_t *handle, button_event_t *event);

// Set target velocity
void set_velocity(float left, float right);

// Sets PWM values
void update_motors(motor_controller_handle_t *handle);

// Use PID configuration
void motor_controller_closeloop(motor_controller_handle_t *handle, button_event_t *event);

// Use openloop configuration
void motor_controller_openloop(motor_controller_handle_t *handle, button_event_t *event);

// Stop both motors
void motor_controller_stop_all(motor_controller_handle_t *handle);

// Package motor controller status into packed struct
motor_group_stat_pkt_t *motor_controller_get_stat(void);

// Prints motor controller status
void motor_controller_print_stat(void);
