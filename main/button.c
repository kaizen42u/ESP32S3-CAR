
#include "button.h"

static const char *TAG = "button";

typedef struct
{
        gpio_num_t pin;
        button_state_t state;
        button_config_active_t inverted;
        uint16_t history;
        uint64_t down_time_us;
} __packed button_data_t;

uint64_t pinmask = 0;
button_data_t button_data[BUTTON_MAX_ARRAY_SIZE];
QueueHandle_t button_queue = NULL;
TaskHandle_t button_task_handle = NULL;

static void update_button(button_data_t *button)
{
        button->history = (button->history << 1) | gpio_get_level(button->pin);
}

static bool button_rose(button_data_t *button)
{
        if ((button->history & BUTTON_ACTIVITY_MASK) == BUTTON_PRESSED_HISTORY)
        {
                button->history = ~(0x0000);
                return true;
        }
        return false;
}
static bool button_fell(button_data_t *button)
{
        if ((button->history & BUTTON_ACTIVITY_MASK) == BUTTON_RELEASED_HISTORY)
        {
                button->history = 0x0000;
                return true;
        }
        return false;
}
static bool button_down(button_data_t *button)
{
        if (button->inverted == BUTTON_CONFIG_ACTIVE_LOW)
                return button_fell(button);
        return button_rose(button);
}
static bool button_up(button_data_t *button)
{
        if (button->inverted == BUTTON_CONFIG_ACTIVE_LOW)
                return button_rose(button);
        return button_fell(button);
}

static void button_send_event(button_data_t *button, const button_state_t prev_state)
{
        button_event_t new_state = {
            .pin = button->pin,
            .prev_state = prev_state,
            .new_state = button->state,
        };

        if (xQueueSend(button_queue, &new_state, 0) != pdTRUE)
                LOG_WARNING("Send queue failed");
}

uint8_t count_num_buttons(const uint64_t bitfield)
{
        uint64_t field = bitfield;
        uint8_t count = 0;
        while (field)
        {
                field &= (field - 1);
                count++;
        }
        return count;
}

static void button_task(void *pvParameter)
{
        for (;;)
        {
                uint8_t num_buttons = count_num_buttons(pinmask);
                for (int idx = 0; idx < num_buttons; idx++)
                {
                        update_button(&button_data[idx]);
                        button_state_t old_state = button_data[idx].state;
                        switch (button_data[idx].state)
                        {
                        case BUTTON_DOWN:
                                if (button_up(&button_data[idx]))
                                        button_data[idx].state = BUTTON_UP;
                                else if (esp_timer_get_time() - button_data[idx].down_time_us > BUTTON_LONG_PRESS_DURATION_US)
                                        button_data[idx].state = BUTTON_LONG;
                                break;
                        case BUTTON_UP:
                                if (button_down(&button_data[idx]))
                                {
                                        button_data[idx].down_time_us = esp_timer_get_time();
                                        button_data[idx].state = BUTTON_DOWN;
                                }
                                break;
                        case BUTTON_LONG:
                                if (button_up(&button_data[idx]))
                                        button_data[idx].state = BUTTON_UP;
                                break;
                        default:
                                LOG_ERROR("unhandled switch statement [%d], aborting", button_data[idx].state);
                                vTaskDelete(NULL);
                                break;
                        }

                        if (old_state != button_data[idx].state)
                        {
                                LOG_VERBOSE("gpio: %d, %s --> %s", button_data[idx].pin, BUTTON_STATE_STRING[old_state], BUTTON_STATE_STRING[button_data[idx].state]);
                                button_send_event(&button_data[idx], old_state);
                        }
                }
                vTaskDelay(pdMS_TO_TICKS(10));
        }
}

QueueHandle_t button_init(void)
{
        if ((button_queue != NULL) || (button_task_handle != NULL))
        {
                LOG_WARNING("Already initialized, queue=0x%X, task=0x%X", (uintptr_t)button_queue, (uintptr_t)button_task_handle);
                return NULL;
        }

        // Initialize queue
        button_queue = xQueueCreate(BUTTON_QUEUE_DEPTH, sizeof(button_event_t)); // TODO: statically allow memory
        if (button_queue == NULL)
        {
                LOG_ERROR("Create queue failed");
                button_deinit();
                return NULL;
        }

        // Spawn a task to monitor the pins
        xTaskCreate(button_task, "button_task", 4096, NULL, 10, &button_task_handle);

        return button_queue;
}

void button_register(const gpio_num_t pin, const button_config_active_t inverted)
{
        if (pinmask & (1ULL << pin))
        {
                LOG_WARNING("The gpio [%d] has been already initialized as an input", pin);
                return;
        }

        uint8_t num_buttons = count_num_buttons(pinmask);
        LOG_INFO("Registering button on gpio: %d, id: %d", pin, num_buttons);

        // Configure the pins
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << pin),
            .mode = GPIO_MODE_INPUT,          // input only
            .pull_up_en = GPIO_PULLUP_ENABLE, // with internal pullup
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK_WITHOUT_ABORT(gpio_config(&io_conf));

        pinmask = pinmask | (1ULL << pin);
        button_data[num_buttons].pin = pin;
        button_data[num_buttons].down_time_us = 0;
        button_data[num_buttons].inverted = inverted;
        if (button_data[num_buttons].inverted == BUTTON_CONFIG_ACTIVE_LOW)
        {
                button_data[num_buttons].history = 0xffff;
                button_data[num_buttons].state = (gpio_get_level(button_data[num_buttons].pin) ? BUTTON_UP : BUTTON_DOWN);
        }
        else
        {
                button_data[num_buttons].history = 0x0000;
                button_data[num_buttons].state = (gpio_get_level(button_data[num_buttons].pin) ? BUTTON_DOWN : BUTTON_UP);
        }
}

void button_deinit(void)
{
        if (button_task_handle != NULL)
        {
                vTaskDelete(button_task_handle);
                button_task_handle = NULL;
        }
        if (button_queue != NULL)
        {
                vQueueDelete(button_queue);
                button_queue = NULL;
        }

        gpio_config_t io_conf = {
            .pin_bit_mask = pinmask,
            .mode = GPIO_MODE_DISABLE,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&io_conf));
        pinmask = 0;
}