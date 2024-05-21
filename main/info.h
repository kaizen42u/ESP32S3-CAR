#pragma once
#include "info_advanced.h"

/* ---> Remote Control Car Settings <--- */

// When the remote control car disconnects from the remote controller, 
// how many seconds until it automatically shuts down
// Unit: seconds (s)
// Range: 10 to 100
// Default: 10
#define IDLE_SHUTDOWN_SECONDS 10

// Left wheel PWM
// Unit: percentage - %
// Range: 25 to 100
// Default: 35
#define LEFT_MOTOR_POWER 35

// Right wheel PWM
// Unit: percentage - %
// Range: 25 to 100
// Default: 35
#define RIGHT_MOTOR_POWER 35

// When set, the remote control car will brake instead of coasting
// Options: true, false
// Default: false
#define MOTOR_BRAKE_ON_IDLE false

/* ---> Catapult Setting <--- */

// Laser intensity
// Unit: percentage - %
// Range: 0 to 100
// Default: 10
#define LASER_POWER 10
