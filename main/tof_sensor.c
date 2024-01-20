
#include "tof_sensor.h"

static const char *TAG = "tof_sensor";

typedef struct
{
        gpio_num_t trig_pin;
        gpio_num_t echo_pin;

        tof_sensor_state_t state;
        int64_t trig_high_time;
        int64_t trig_low_time;
        int64_t echo_high_time;
        int64_t echo_low_time;
        int64_t duration;
} tof_sensor_data_t;

tof_sensor_data_t tof_sensor_data = {0};
QueueHandle_t tof_sensor_queue = NULL;
TaskHandle_t tof_sensor_task_handle = NULL;

void set_trig(void)
{
        gpio_set_level(tof_sensor_data.trig_pin, 1);
}

void clear_trig(void)
{
        gpio_set_level(tof_sensor_data.trig_pin, 0);
}

bool read_echo(void)
{
        return gpio_get_level(tof_sensor_data.echo_pin);
}

static void IRAM_ATTR echo_pin_event(void *arg)
{
        bool state = read_echo();
        if (state)
                tof_sensor_data.echo_high_time = esp_timer_get_time();
        else
        {
                tof_sensor_data.echo_low_time = esp_timer_get_time();
                tof_sensor_data.duration = tof_sensor_data.echo_low_time - tof_sensor_data.echo_high_time;
        }
        // ets_printf("ISR event %d\r\n", state);
}

static void tof_sensor_send_event(tof_sensor_data_t tof_sensor_data, tof_sensor_state_t state)
{
        tof_sensor_event_t event = {
            .echo_pin = tof_sensor_data.echo_pin,
            .trig_pin = tof_sensor_data.trig_pin,
            .state = state,
            .duration_us = tof_sensor_data.duration,
        };
        xQueueOverwrite(tof_sensor_queue, &event);
}

static void tof_sensor_task(void *pvParameter)
{
        while (true)
        {
                uint8_t loop = 10;
                while (0 < loop--)
                {
                        switch (tof_sensor_data.state)
                        {
                        case TOF_INIT:
                                ESP_LOGV(TAG, "TOF_INIT");
                                tof_sensor_data.state = TOF_TRIG_HIGH;
                                break;
                        case TOF_TRIG_HIGH:
                                ESP_LOGV(TAG, "TOF_TRIG_HIGH");
                                set_trig();
                                tof_sensor_data.state = TOF_TRIG_HOLD;
                                tof_sensor_data.trig_high_time = esp_timer_get_time();
                                break;
                        case TOF_TRIG_HOLD:
                                ESP_LOGV(TAG, "TOF_TRIG_HOLD");
                                if ((esp_timer_get_time() - tof_sensor_data.trig_high_time) > SR04_ECHO_HIGH_TIME_US)
                                        tof_sensor_data.state = TOF_TRIG_LOW;
                                else
                                        loop = 0;
                                break;
                        case TOF_TRIG_LOW:
                                ESP_LOGV(TAG, "TOF_TRIG_LOW");
                                clear_trig();
                                tof_sensor_data.state = TOF_WAIT_HIGH;
                                tof_sensor_data.trig_low_time = esp_timer_get_time();
                                break;
                        case TOF_WAIT_HIGH:
                                ESP_LOGV(TAG, "TOF_WAIT_HIGH");
                                if (tof_sensor_data.echo_high_time >= tof_sensor_data.trig_high_time)
                                        tof_sensor_data.state = TOF_WAIT_LOW;
                                else if ((esp_timer_get_time() - tof_sensor_data.trig_high_time) > SR04_TIMEOUT_TIME_US)
                                        tof_sensor_data.state = TOF_DEVICE_TIMEOUT;
                                else
                                        loop = 0;
                                break;
                        case TOF_WAIT_LOW:
                                ESP_LOGV(TAG, "TOF_WAIT_LOW");
                                if (tof_sensor_data.echo_low_time >= tof_sensor_data.echo_high_time)
                                        if (tof_sensor_data.duration < SR04_BAD_MEASUREMENT_TIME_US)
                                                tof_sensor_data.state = TOF_BAD_MEASUREMENT;
                                        else if (tof_sensor_data.duration > SR04_OUT_OF_RANGE_TIME_US)
                                                tof_sensor_data.state = TOF_OUT_OF_RANGE;
                                        else
                                        {
                                                tof_sensor_send_event(tof_sensor_data, TOF_OK);
                                                tof_sensor_data.state = TOF_COOLDOWN;
                                        }
                                else
                                        loop = 0;
                                break;
                        case TOF_COOLDOWN:
                                ESP_LOGV(TAG, "TOF_COOLDOWN");
                                if ((tof_sensor_data.echo_low_time + tof_sensor_data.duration * 2) <= esp_timer_get_time())
                                        tof_sensor_data.state = TOF_TRIG_HIGH;
                                else
                                        loop = 0;
                                break;
                        case TOF_OUT_OF_RANGE:
                                tof_sensor_send_event(tof_sensor_data, TOF_OUT_OF_RANGE);
                                // ESP_LOGI(TAG, "TOF_OUT_OF_RANGE");
                                tof_sensor_data.state = TOF_INIT;
                                break;
                        case TOF_DEVICE_TIMEOUT:
                                tof_sensor_send_event(tof_sensor_data, TOF_DEVICE_TIMEOUT);
                                // ESP_LOGI(TAG, "TOF_DEVICE_TIMEOUT");
                                tof_sensor_data.state = TOF_INIT;
                                break;
                        case TOF_BAD_MEASUREMENT:
                                tof_sensor_send_event(tof_sensor_data, TOF_BAD_MEASUREMENT);
                                // ESP_LOGI(TAG, "TOF_BAD_MEASUREMENT");
                                tof_sensor_data.state = TOF_INIT;
                                break;
                        default:
                                // tof_sensor_send_event(tof_sensor_data);
                                ESP_LOGE(TAG, "(%d, %d) ERROR", tof_sensor_data.trig_pin, tof_sensor_data.echo_pin);
                                vTaskDelete(NULL);
                                break;
                        }
                }
                vTaskDelay(1);
        }
}

QueueHandle_t tof_sensor_init(gpio_num_t trig, gpio_num_t echo)
{
        tof_sensor_data.trig_pin = trig;
        tof_sensor_data.echo_pin = echo;
        tof_sensor_data.state = TOF_INIT;
        tof_sensor_data.trig_high_time = 0;

        // Configure the pins
        ESP_ERROR_CHECK(gpio_install_isr_service(0));
        ESP_ERROR_CHECK(gpio_isr_handler_add(echo, echo_pin_event, NULL));

        gpio_config_t echo_io_conf = {
            .pin_bit_mask = 1 << echo,
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_ANYEDGE,
        };
        ESP_ERROR_CHECK(gpio_config(&echo_io_conf));

        gpio_config_t trig_io_conf = {
            .pin_bit_mask = 1 << trig,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&trig_io_conf));

        tof_sensor_queue = xQueueCreate(1, sizeof(tof_sensor_event_t)); // TODO: statically allow memory
        if (tof_sensor_queue == NULL)
        {
                ESP_LOGE(TAG, "NO MEMORY");
                tof_sensor_deinit();
                return NULL;
        }

        // Spawn a task to monitor the ultrasound sensor
        xTaskCreate(tof_sensor_task, "tof_sensor_task", 4096, NULL, 10, &tof_sensor_task_handle);

        return tof_sensor_queue;
}

void tof_sensor_deinit(void)
{
        if (tof_sensor_task_handle != NULL)
        {
                vTaskDelete(tof_sensor_task_handle);
                tof_sensor_task_handle = NULL;
        }
        if (tof_sensor_queue != NULL)
        {
                vQueueDelete(tof_sensor_queue);
                tof_sensor_queue = NULL;
        }
        gpio_reset_pin(tof_sensor_data.trig_pin);
        gpio_reset_pin(tof_sensor_data.echo_pin);
        memset(&tof_sensor_data, 0, sizeof(tof_sensor_data_t));
}