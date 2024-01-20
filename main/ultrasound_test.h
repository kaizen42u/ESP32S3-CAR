
#pragma once

#include <string.h>

#include "esp_timer.h"
#include "esp_log.h"
#include "ctype.h"

#include "u8g2_esp32_hal.h"
#include "tof_sensor.h"
#include "pindef.h"
#include "mem_probe.h"

float constrain(float value, float min, float max);
float ultrasound_duration_to_distance(float duration_us);

void display_ultrasound_draw(u8g2_t *u8g2, tof_sensor_event_t tof_sensor_event);

void drawLogo(u8g2_t *u8g2);
void drawURL(u8g2_t *u8g2);

u8g2_t display_init(void);
void display_ultrasound_loop(void);
