
#pragma once

#include "servo.h"
#include "button.h"
#include "pindef.h"
#include "mathop.h"
#include "laser.h"

#define CATAPULT_WIND_DEFAULT_ANGLE (145)
#define CATAPULT_LOCK_LOCKED_ANGLE (90)
#define CATAPULT_LOCK_UNLOCKED_ANGLE (0)
#define CATAPULT_LASER_DEFAULT_ANGLE (110)
#define CATAPULT_LASER_SET_ANGLE (90)
#define CATAPULT_RESET_TIMEOUT_US (1 * 1e6)

typedef enum
{
        STATE_CATAPULT_RESET = 0,
        STATE_CATAPULT_IDLE,
        STATE_CATAPULTY_SET_SERVO,
        STATE_STRENGTH_INCREASE,
        STATE_STRENGTH_DECREASE,
        STATE_CATAPULT_SHOOT,
        STATE_CATAPULT_DECAY,
        STATE_RESET_DELAY,
} sm_state_t;

typedef struct
{
        servo_handle_t *lock;
        servo_handle_t *wind;
        servo_handle_t *laser;

        laser_handle_t *aiming_laser;

        sm_state_t state;
        float wind_angle;
        float laser_angle;
        float laser_duty_cycle;
        uint64_t catapult_shot_time;

} catapult_controller_handle_t;

catapult_controller_handle_t *catapult_controller_default_config(catapult_controller_handle_t *handle);
void catapult_controller_init(catapult_controller_handle_t *handle);

void catapult_controller(catapult_controller_handle_t *handle, button_event_t *event);