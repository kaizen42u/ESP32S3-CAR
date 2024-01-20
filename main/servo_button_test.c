
#include "servo_button_test.h"

static const char *TAG = "servo_button_test";

float button_state_to_angle(button_state_t state)
{
        float angle;
        switch (state)
        {
        case BUTTON_DOWN:
        case BUTTON_LONG:
                angle = 90;
                break;
        case BUTTON_UP:
        default:
                angle = 0;
                break;
        }
        return angle;
}

void servo_button_loop(void)
{
        button_event_t button_event;
        QueueHandle_t button_event_queue = button_init(GPIO_INPUT_PIN_SEL);

        servo_handle_t servo0, servo1, servo2, servo3;
        servo_default_config(&servo0);
        servo_default_config(&servo1);
        servo_default_config(&servo2);
        servo_default_config(&servo3);
        servo_init(&servo0, LEDC_TIMER_0, LEDC_CHANNEL_0, GPIO_SERVO_1);
        servo_init(&servo1, LEDC_TIMER_0, LEDC_CHANNEL_1, GPIO_SERVO_2);
        servo_init(&servo2, LEDC_TIMER_0, LEDC_CHANNEL_2, GPIO_SERVO_3);
        servo_init(&servo3, LEDC_TIMER_0, LEDC_CHANNEL_3, GPIO_SERVO_4);
        servo_set_angle(&servo0, 0);
        servo_set_angle(&servo1, 45);
        servo_set_angle(&servo2, 90);
        servo_set_angle(&servo3, 135);

        while (true)
        {
                // read the events and write to console
                if (xQueueReceive(button_event_queue, &button_event, portMAX_DELAY))
                {
                        ESP_LOGI(TAG, "GPIO event: pin %d, event = %s", button_event.pin, BUTTON_STATE_STRING[button_event.event]);
                        float angle = button_state_to_angle(button_event.event);
                        switch (button_event.pin)
                        {
                        case GPIO_BUTTON_UP:
                                ESP_LOGI(TAG, "servo0 set angle = %g", angle);
                                servo_set_angle(&servo0, angle);
                                break;
                        case GPIO_BUTTON_DOWN:
                                ESP_LOGI(TAG, "servo1 set angle = %g", angle);
                                servo_set_angle(&servo1, angle);
                                break;
                        case GPIO_BUTTON_LEFT:
                                ESP_LOGI(TAG, "servo2 set angle = %g", angle);
                                servo_set_angle(&servo2, angle);
                                break;
                        case GPIO_BUTTON_RIGHT:
                                ESP_LOGI(TAG, "servo3 set angle = %g", angle);
                                servo_set_angle(&servo3, angle);
                                break;
                        default:
                                break;
                        }
                }
        }
}