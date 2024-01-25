
#include "motor_controller.h"

static const char *TAG = "motor_controller";

bool is_motor_control_pins(gpio_num_t pin)
{
        switch (pin)
        {
        case GPIO_BUTTON_UP:
        case GPIO_BUTTON_DOWN:
        case GPIO_BUTTON_LEFT:
        case GPIO_BUTTON_RIGHT:
                return true;
        default:
                return false;
        }
}

void motor_controller_config_mcpwm_enable(motor_controller_handle_t *handle)
{
        gpio_config_t mcpwm_enable_io_conf = {
            .pin_bit_mask = 1 << handle->mcpwm_en,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        ESP_ERROR_CHECK(gpio_config(&mcpwm_enable_io_conf));
}
void motor_controller_set_mcpwm_enable(motor_controller_handle_t *handle)
{
        gpio_set_level(handle->mcpwm_en, 1);
}
void motor_controller_clear_mcpwm_enable(motor_controller_handle_t *handle)
{
        gpio_set_level(handle->mcpwm_en, 0);
}

motor_controller_handle_t *motor_controller_default_config(motor_controller_handle_t *handle)
{
        static motor_handle_t left_motor, right_motor;
        brushed_motor_default_config(&left_motor);
        brushed_motor_default_config(&right_motor);

        left_motor.ch_A_pin = GPIO_MCPWM_UNIT0_PWM0A_OUT;
        left_motor.ch_B_pin = GPIO_MCPWM_UNIT0_PWM0B_OUT;
        left_motor.mcpwm_ch_A_signal = MCPWM0A;
        left_motor.mcpwm_ch_B_signal = MCPWM0B;
        left_motor.mcpwm_unit = MCPWM_UNIT_0;
        left_motor.mcpwm_timer = MCPWM_TIMER_0;
        left_motor.pcnt_pin = GPIO_MOTORL_COUNTER;

        right_motor.ch_A_pin = GPIO_MCPWM_UNIT0_PWM1A_OUT;
        right_motor.ch_B_pin = GPIO_MCPWM_UNIT0_PWM1B_OUT;
        right_motor.mcpwm_ch_A_signal = MCPWM1A;
        right_motor.mcpwm_ch_B_signal = MCPWM1B;
        right_motor.mcpwm_unit = MCPWM_UNIT_0;
        right_motor.mcpwm_timer = MCPWM_TIMER_1;
        right_motor.pcnt_pin = GPIO_MOTORR_COUNTER;

        handle->left = &left_motor;
        handle->right = &right_motor;
        handle->mcpwm_en = GPIO_MCPWM_ENABLE;
        return handle;
}

motor_controller_handle_t *motor_controller_init(motor_controller_handle_t *handle)
{
        brushed_motor_init(handle->left);
        brushed_motor_init(handle->right);
        motor_controller_config_mcpwm_enable(handle);
        return handle;
}

void motor_controller(motor_controller_handle_t *handle, button_event_t *event)
{
        const int speed = 50;
        if (event->new_state == BUTTON_LONG)
                return;
        if (!is_motor_control_pins(event->pin))
                return;
        if (event->new_state == BUTTON_UP)
        {
                brushed_motor_set(handle->left, 0);
                brushed_motor_set(handle->right, 0);
                return;
        }

        switch (event->pin)
        {
        case GPIO_BUTTON_UP:
                ESP_LOGI(TAG, "forward");
                brushed_motor_set(handle->left, speed);
                brushed_motor_set(handle->right, speed);
                break;
        case GPIO_BUTTON_DOWN:
                ESP_LOGI(TAG, "backward");
                brushed_motor_set(handle->left, -speed);
                brushed_motor_set(handle->right, -speed);
                break;
        case GPIO_BUTTON_LEFT:
                ESP_LOGI(TAG, "rotate left");
                brushed_motor_set(handle->left, -speed);
                brushed_motor_set(handle->right, speed);
                break;
        case GPIO_BUTTON_RIGHT:
                ESP_LOGI(TAG, "rotate right");
                brushed_motor_set(handle->left, speed);
                brushed_motor_set(handle->right, -speed);
                break;
        default:
                ESP_LOGW(TAG, "button id %d is undefined", event->pin);
                break;
        }
}

void motor_controller_stop_all(motor_controller_handle_t *handle)
{
        brushed_motor_set(handle->left, 0);
        brushed_motor_set(handle->right, 0);
}

void motor_controller_print_counts(motor_controller_handle_t *handle)
{
        int lcnt = brushed_motor_get_count(handle->left);
        int rcnt = brushed_motor_get_count(handle->right);
        LOG_INFO("L:%6d, R:%6d", lcnt, rcnt);
}