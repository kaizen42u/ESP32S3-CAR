
#pragma once

#include "servo.h"
#include "button.h"
#include "pindef.h"
#include "mathop.h"
#include "laser.h"

#define GOALKEEPER_DEFAULT_ANGLE (85)
#define GOALKEEPER_TILT_LEFT_ANGLE (60)
#define GOALKEEPER_TILT_RIGHT_ANGLE (110)

typedef enum
{
        STATE_GOALKEEPER_RESET,
        STATE_GOALKEEPER_IDLE,
        STATE_GOALKEEPER_LEFT,
        STATE_GOALKEEPER_RIGHT,
} GK_servo_state_t;

typedef struct
{
        servo_handle_t *GK_servo;
        GK_servo_state_t state;
        float GK_servo_angle;
} goalkeeper_controller_handle_t;

goalkeeper_controller_handle_t *goalkeeper_controller_default_config(goalkeeper_controller_handle_t *handle);
void goalkeeper_controller_init(goalkeeper_controller_handle_t *handle);

void goalkeeper_controller(goalkeeper_controller_handle_t *handle, button_event_t *event);