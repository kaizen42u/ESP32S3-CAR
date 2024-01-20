
#pragma once

#include <string.h>

#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_timer.h"

#define BUTTON_LONG_PRESS_DURATION_US (1000 * 1000)

typedef enum
{
        BUTTON_DOWN,
        BUTTON_UP,
        BUTTON_LONG,
        BUTTON_ERROR
} button_state_t;

static const char __attribute__((unused)) * BUTTON_STATE_STRING[] = {
    "BUTTON_DOWN",
    "BUTTON_UP",
    "BUTTON_LONG",
    "BUTTON_ERROR"};

typedef struct
{
        gpio_num_t pin;
        button_state_t event;
} button_event_t;

QueueHandle_t button_init(unsigned long long pin_select);
void button_deinit(void);
