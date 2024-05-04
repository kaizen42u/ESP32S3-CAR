
#pragma once

#include "servo.h"
#include "button.h"
#include "pindef.h"
#include "mathop.h"
#include "laser.h"
#include "info.h"

#define CATAPULT_WIND_DEFAULT_ANGLE (145)
#define CATAPULT_LOCK_LOCKED_ANGLE (90)
#define CATAPULT_LOCK_UNLOCKED_ANGLE (0)
#define CATAPULT_LASER_DEFAULT_ANGLE (110)
#define CATAPULT_LASER_SET_ANGLE (90)
#define CATAPULT_RESET_TIMEOUT_US (1 * 1e6)

// Indicates the catapult controller state
typedef enum
{
        STATE_CATAPULT_RESET = 0, // Initial condition
        STATE_CATAPULT_IDLE,      // Waiting for command (button event)
        STATE_CATAPULT_SET_SERVO, // Test case that all servo are in a preset condition
        STATE_STRENGTH_INCREASE,  // Increasing the pull on the spring
        STATE_STRENGTH_DECREASE,  // Decreasing the pull on the spring
        STATE_CATAPULT_SHOOT,     // Shoots teh ball!
        STATE_CATAPULT_DECAY,     // Slowly returns the winding servo to its default value
        STATE_RESET_DELAY,        // Do nothing after shooting, then goes into RESET
} catapult_controller_state_t;

// All the resources that are used for the catapult top module
typedef struct
{
        servo_handle_t *lock;              // Servo motor handler for `locking` the catapult
        servo_handle_t *wind;              // Servo motor handler for `arming` the catapult
        servo_handle_t *laser;             // Servo motor handler for `aiming` the laser diode
        laser_handle_t *aiming_laser;      // PWM output handler for the laser diode
        catapult_controller_state_t state; // State of the catapult controller
        float wind_angle;                  // `locking` angle
        float laser_angle;                 // `aiming` angle
        float laser_duty_cycle;            // PWM percentage of the laser diode
        uint64_t catapult_shot_time;       // CPU Time when the catapult is shot (in us)
} catapult_controller_handle_t;

// Creates the resources for catapult controller
void catapult_controller_init(catapult_controller_handle_t *handle);

// Loads default settings of the catapult controller
// Returns a queue for button events
// Call only after `catapult_controller_init()`
catapult_controller_handle_t *catapult_controller_default_config(catapult_controller_handle_t *handle);

// Runs the catapult controller, with button event as its parameter
void catapult_controller(catapult_controller_handle_t *handle, button_event_t *event);