
#pragma once

#include "driver/gpio.h"

#include "pindef.h"

#define TIME_BEFORE_RESET (10) // In seconds

// Keep the power latch enabled
void keep_power(void);

// Turn off the power latch, in turns kills the power
void kill_power(void);

// Configure the latch enable GPIO
void config_wake_gpio(void);
