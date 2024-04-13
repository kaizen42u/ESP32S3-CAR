
#pragma once

#include "driver/gpio.h"

#include "pindef.h"

#define TIME_BEFORE_RESET (10)

void keep_power(void);
void kill_power(void);
void config_wake_gpio(void);
