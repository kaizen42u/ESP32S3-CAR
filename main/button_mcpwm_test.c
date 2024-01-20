
#include "button_mcpwm_test.h"

static const char *TAG = "button_mcpwm_test";

void button_mcpwm_loop(void)
{
        motor_handle_t front_left_motor;
        motor_handle_t front_right_motor;
        motor_handle_t rear_left_motor;
        motor_handle_t rear_right_motor;

        brushed_motor_default_config(&front_left_motor);
        brushed_motor_default_config(&front_right_motor);
        brushed_motor_default_config(&rear_left_motor);
        brushed_motor_default_config(&rear_right_motor);

        front_left_motor.ch_A_pin = GPIO_MCPWM_UNIT0_PWM0A_OUT;
        front_left_motor.ch_B_pin = GPIO_MCPWM_UNIT0_PWM0B_OUT;
        front_left_motor.mcpwm_ch_A_signal = MCPWM0A;
        front_left_motor.mcpwm_ch_B_signal = MCPWM0B;
        front_left_motor.mcpwm_unit = MCPWM_UNIT_0;
        front_left_motor.mcpwm_timer = MCPWM_TIMER_0;

        front_right_motor.ch_A_pin = GPIO_MCPWM_UNIT0_PWM1A_OUT;
        front_right_motor.ch_B_pin = GPIO_MCPWM_UNIT0_PWM1B_OUT;
        front_right_motor.mcpwm_ch_A_signal = MCPWM1A;
        front_right_motor.mcpwm_ch_B_signal = MCPWM1B;
        front_right_motor.mcpwm_unit = MCPWM_UNIT_0;
        front_right_motor.mcpwm_timer = MCPWM_TIMER_1;

        rear_left_motor.ch_A_pin = GPIO_MCPWM_UNIT1_PWM0A_OUT;
        rear_left_motor.ch_B_pin = GPIO_MCPWM_UNIT1_PWM0B_OUT;
        rear_left_motor.mcpwm_ch_A_signal = MCPWM0A;
        rear_left_motor.mcpwm_ch_B_signal = MCPWM0B;
        rear_left_motor.mcpwm_unit = MCPWM_UNIT_1;
        rear_left_motor.mcpwm_timer = MCPWM_TIMER_0;

        rear_right_motor.ch_A_pin = GPIO_MCPWM_UNIT0_PWM1A_OUT;
        rear_right_motor.ch_B_pin = GPIO_MCPWM_UNIT0_PWM1B_OUT;
        rear_right_motor.mcpwm_ch_A_signal = MCPWM1A;
        rear_right_motor.mcpwm_ch_B_signal = MCPWM1B;
        rear_right_motor.mcpwm_unit = MCPWM_UNIT_1;
        rear_right_motor.mcpwm_timer = MCPWM_TIMER_1;

        brushed_motor_init(&front_left_motor);
        brushed_motor_init(&front_right_motor);
        brushed_motor_init(&rear_left_motor);
        brushed_motor_init(&rear_right_motor);

        QueueHandle_t button_event_queue = button_init(GPIO_INPUT_PIN_SEL);
        button_event_t button_event;

        while (true)
        {
                // read the events and write to console
                if (xQueueReceive(button_event_queue, &button_event, portMAX_DELAY))
                {
                        ESP_LOGI(TAG, "GPIO event: pin %d, event = %s", button_event.pin, BUTTON_STATE_STRING[button_event.event]);
                        if (button_event.event != BUTTON_DOWN)
                        {
                                brushed_motor_set(&front_left_motor, 0);
                                brushed_motor_set(&front_right_motor, 0);
                        }
                        else
                        {
                                switch (button_event.pin)
                                {
                                case GPIO_BUTTON_UP:
                                        ESP_LOGI(TAG, "forward");
                                        brushed_motor_set(&front_left_motor, 100);
                                        brushed_motor_set(&front_right_motor, 100);
                                        break;
                                case GPIO_BUTTON_DOWN:
                                        ESP_LOGI(TAG, "backward");
                                        brushed_motor_set(&front_left_motor, -100);
                                        brushed_motor_set(&front_right_motor, -100);
                                        break;
                                case GPIO_BUTTON_LEFT:
                                        ESP_LOGI(TAG, "rotate left");
                                        brushed_motor_set(&front_left_motor, -100);
                                        brushed_motor_set(&front_right_motor, 100);
                                        break;
                                case GPIO_BUTTON_RIGHT:
                                        ESP_LOGI(TAG, "rotate right");
                                        brushed_motor_set(&front_left_motor, 100);
                                        brushed_motor_set(&front_right_motor, -100);
                                        break;
                                default:
                                        break;
                                }
                        }
                }
        }
}
