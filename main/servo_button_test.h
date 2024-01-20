
#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "button.h"
#include "servo.h"
#include "pindef.h"

float button_state_to_angle(button_state_t state);

void servo_button_loop(void);
