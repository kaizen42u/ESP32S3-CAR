
#pragma once

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "rom/ets_sys.h"

#include "esp_log.h"
#include "esp_timer.h"

#define SR04_TIMEOUT_TIME_US (1000 * 1000)
#define SR04_BAD_MEASUREMENT_TIME_US (200)
#define SR04_OUT_OF_RANGE_TIME_US (40 * 1000)
#define SR04_ECHO_HIGH_TIME_US (10)

typedef enum
{
        TOF_INIT,
        TOF_TRIG_HIGH,
        TOF_TRIG_HOLD,
        TOF_TRIG_LOW,
        TOF_WAIT_HIGH,
        TOF_WAIT_LOW,
        TOF_COOLDOWN,
        TOF_DEVICE_TIMEOUT,
        TOF_OK,
        TOF_OUT_OF_RANGE,
        TOF_BAD_MEASUREMENT,
        TOF_ERROR,
} tof_sensor_state_t;

static const char __attribute__((unused)) * TOF_SENSOR_STATE_STRING[] = {
    "TOF_INIT",
    "TOF_TRIG_HIGH",
    "TOF_TRIG_HOLD",
    "TOF_TRIG_LOW",
    "TOF_WAIT_HIGH",
    "TOF_WAIT_LOW",
    "TOF_COOLDOWN",
    "TOF_DEVICE_TIMEOUT",
    "TOF_OK",
    "TOF_OUT_OF_RANGE",
    "TOF_BAD_MEASUREMENT",
    "TOF_ERROR"};

typedef struct
{
        uint64_t duration_us;
        gpio_num_t trig_pin;
        gpio_num_t echo_pin;
        tof_sensor_state_t state;
} tof_sensor_event_t;

QueueHandle_t tof_sensor_init(gpio_num_t trig, gpio_num_t echo);
void tof_sensor_deinit(void);