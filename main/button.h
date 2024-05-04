
#pragma once

#include <string.h>

#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_timer.h"

#include "logging.h"

#define BUTTON_PRESSED_HISTORY (0x003F)
#define BUTTON_RELEASED_HISTORY (0xF000)
#define BUTTON_ACTIVITY_MASK (BUTTON_PRESSED_HISTORY | BUTTON_RELEASED_HISTORY)
#define BUTTON_LONG_PRESS_DURATION_US (1000 * 1000)
#define BUTTON_QUEUE_DEPTH (16)
#define BUTTON_MAX_ARRAY_SIZE (16)

// Indicates the button state
typedef enum
{
        BUTTON_DOWN, // The button is `pressed`
        BUTTON_UP,   // The button is `released`
        BUTTON_LONG, // The button is being `held` for more than one second
        BUTTON_ERROR
} button_state_t;

// Sets the edge trigger condition
typedef enum
{
        BUTTON_CONFIG_ACTIVE_LOW, // The button is pulling the GPIO `low` when pressed
        BUTTON_CONFIG_ACTIVE_HIGH // The button is pulling the GPIO `high` when pressed
} button_config_active_t;

static const char __attribute__((unused)) * BUTTON_STATE_STRING[] = {
    "BUTTON_DOWN",
    "BUTTON_UP",
    "BUTTON_LONG",
    "BUTTON_ERROR"};

// Instance of button state event
typedef struct
{
        gpio_num_t pin : 8;            // GPIO pin of said button (`id`)
        button_state_t prev_state : 4; // old state of button (`from`)
        button_state_t new_state : 4;  // new state of button (`to`)
} __packed button_event_t;

// Creates the task for reading the GPIO and returns a queue for button events
QueueHandle_t button_init(void);

// Register one button at `pin` and its trigger condition
// Call only after `button_init()`
void button_register(const gpio_num_t pin, const button_config_active_t inverted);

void button_deinit(void);
