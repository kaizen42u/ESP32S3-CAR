
#pragma once

#include "servo.h"
#include "button.h"
#include "pindef.h"
#include "mathop.h"
#include "laser.h"
#include "info.h"

#define GOALKEEPER_DEFAULT_ANGLE (85)
#define GOALKEEPER_TILT_LEFT_ANGLE (60)
#define GOALKEEPER_TILT_RIGHT_ANGLE (110)
#define AUTO_SPEED_SLOW (1 * 1e6)       // Changes every 1 second
#define AUTO_SPEED_MIDDLE (0.5 * 1e6)   // Changes every 0.5 seconds
#define AUTO_SPEED_FAST (0.25 * 1e6)    // Changes every 0.25 seconds

// Indicates the goalkeeper controller state
typedef enum
{
        STATE_GOALKEEPER_RESET = 0, // Initial condition
        STATE_GOALKEEPER_IDLE,      // Waiting for command (button event)
        STATE_GOALKEEPER_LEFT,      // Tilting to the left
        STATE_GOALKEEPER_RIGHT,     // Tilting to the right
        STATE_GOALKEEPER_AUTO,      // Auto tilting
        STATE_GOALKEEPER_MAX,
} goalkeeper_controller_state_t;

static const char __attribute__((unused)) * GOALKEEPER_CONTROLLER_STATE_STRING[] = {
    "STATE_GOALKEEPER_RESET",
    "STATE_GOALKEEPER_IDLE",
    "STATE_GOALKEEPER_LEFT",
    "STATE_GOALKEEPER_RIGHT",
    "STATE_GOALKEEPER_AUTO",
    "STATE_GOALKEEPER_MAX"};

// All the resources that are used for the goalkeeper top module
typedef struct
{
        servo_handle_t *goalkeeping_servo;   // Servo motor handler for tilting
        goalkeeper_controller_state_t state; // State of the goalkeeper controller
        float goalkeeping_servo_angle;       // Tilting angle
} goalkeeper_controller_handle_t;

// Creates the resources for goalkeeper controller
void goalkeeper_controller_init(goalkeeper_controller_handle_t *handle);

// Loads default settings of the goalkeeper controller
// Returns a queue for button events
// Call only after `goalkeeper_controller_init()`
goalkeeper_controller_handle_t *goalkeeper_controller_default_config(goalkeeper_controller_handle_t *handle);

// Runs the goalkeeper controller, with button event as its parameter
void goalkeeper_controller(goalkeeper_controller_handle_t *handle, button_event_t *event);