
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
        // setup motors

        static motor_handle_t left_motor_handle, right_motor_handle;
        dc_motor_default_config(&left_motor_handle);
        dc_motor_default_config(&right_motor_handle);

        left_motor_handle.ch_A_pin = GPIO_MCPWM_UNIT0_PWM0A_OUT;
        left_motor_handle.ch_B_pin = GPIO_MCPWM_UNIT0_PWM0B_OUT;
        left_motor_handle.mcpwm_ch_A_signal = MCPWM0A;
        left_motor_handle.mcpwm_ch_B_signal = MCPWM0B;
        left_motor_handle.mcpwm_unit = MCPWM_UNIT_0;
        left_motor_handle.mcpwm_timer = MCPWM_TIMER_0;
        left_motor_handle.pcnt_pin = GPIO_MOTORL_COUNTER;

        right_motor_handle.ch_A_pin = GPIO_MCPWM_UNIT0_PWM1A_OUT;
        right_motor_handle.ch_B_pin = GPIO_MCPWM_UNIT0_PWM1B_OUT;
        right_motor_handle.mcpwm_ch_A_signal = MCPWM1A;
        right_motor_handle.mcpwm_ch_B_signal = MCPWM1B;
        right_motor_handle.mcpwm_unit = MCPWM_UNIT_0;
        right_motor_handle.mcpwm_timer = MCPWM_TIMER_1;
        right_motor_handle.pcnt_pin = GPIO_MOTORR_COUNTER;

        // setup speed meters
        static ringbuffer_handle_t left_velocity_rb_handle, right_velocity_rb_handle;
        static float left_velocity_buffer[VELOCITY_INTETGRATE_SAMPLES];
        static float right_velocity_buffer[VELOCITY_INTETGRATE_SAMPLES];
        ringbuffer_init(&left_velocity_rb_handle, left_velocity_buffer, VELOCITY_INTETGRATE_SAMPLES);
        ringbuffer_init(&right_velocity_rb_handle, right_velocity_buffer, VELOCITY_INTETGRATE_SAMPLES);

        static ringbuffer_handle_t left_acceleration_rb_handle, right_acceleration_rb_handle;
        static float left_acceleration_buffer[ACCELERATION_INTETGRATE_SAMPLES];
        static float right_acceleration_buffer[ACCELERATION_INTETGRATE_SAMPLES];
        ringbuffer_init(&left_acceleration_rb_handle, left_acceleration_buffer, ACCELERATION_INTETGRATE_SAMPLES);
        ringbuffer_init(&right_acceleration_rb_handle, right_acceleration_buffer, ACCELERATION_INTETGRATE_SAMPLES);

        static ringbuffer_handle_t distance_difference_rb_handle;
        static float distance_difference_rb_buffer[DISTANCE_DIFFERENCE_INTEGRATE_SAMPLES];
        ringbuffer_init(&distance_difference_rb_handle, distance_difference_rb_buffer, DISTANCE_DIFFERENCE_INTEGRATE_SAMPLES);

        // setup pid controllers

        static pid_handle_t left_motor_pid_handle, right_motor_pid_handle;
        pid_init(&left_motor_pid_handle, 0.001, 0.01, 0.06, 0.0001, 0.3);
        pid_init(&right_motor_pid_handle, 0.001, 0.01, 0.06, 0.0001, 0.3);
        pid_set_output_range(&left_motor_pid_handle, -100, 100);
        pid_set_output_range(&right_motor_pid_handle, -100, 100);

        static pid_handle_t distance_difference_pid_handle;
        pid_init(&distance_difference_pid_handle, 0.001, 0, 0, 0, 0);
        pid_set_output_range(&distance_difference_pid_handle, -50, 50);

        // create the handle

        handle->left_motor_handle = &left_motor_handle;
        handle->right_motor_handle = &right_motor_handle;
        handle->left_velocity_rb_handle = &left_velocity_rb_handle;
        handle->right_velocity_rb_handle = &right_velocity_rb_handle;
        handle->left_acceleration_rb_handle = &left_acceleration_rb_handle;
        handle->right_acceleration_rb_handle = &right_acceleration_rb_handle;
        handle->left_motor_pid_handle = &left_motor_pid_handle;
        handle->right_motor_pid_handle = &right_motor_pid_handle;

        handle->distance_difference_rb_handle = &distance_difference_rb_handle;
        handle->distance_difference_pid_handle = &distance_difference_pid_handle;
        handle->mcpwm_en = GPIO_MCPWM_ENABLE;
        return handle;
}

motor_controller_handle_t *motor_controller_init(motor_controller_handle_t *handle)
{
        dc_motor_init(handle->left_motor_handle);
        dc_motor_init(handle->right_motor_handle);
        motor_controller_config_mcpwm_enable(handle);
        return handle;
}

static float left_velocity, right_velocity;
static float left_acceleration, right_acceleration;
static float left_duty_cycle, right_duty_cycle;

static float distance_difference;
static float speed_difference;

void motor_controller(motor_controller_handle_t *handle, button_event_t *event)
{
        const int speed_reference = 50;
        static int target_speed_left = 0, target_speed_right = 0;
        int left_counter = dc_motor_get_count(handle->left_motor_handle);
        int right_counter = dc_motor_get_count(handle->right_motor_handle);
        static int stall_count;
        if ((handle->left_motor_handle->count == left_counter) && (handle->right_motor_handle->count == right_counter))
        {
                if (stall_count)
                        stall_count--;
                else
                {
                        pid_reset(handle->left_motor_pid_handle);
                        pid_reset(handle->right_motor_pid_handle);
                }
        }
        else
                stall_count = STALL_RESET_TIMEOUT_MS;

        differentiator_update(handle->left_velocity_rb_handle, left_counter, &left_velocity);
        differentiator_update(handle->right_velocity_rb_handle, right_counter, &right_velocity);
        differentiator_update(handle->left_acceleration_rb_handle, left_velocity, &left_acceleration);
        differentiator_update(handle->right_acceleration_rb_handle, right_velocity, &right_acceleration);

        differentiator_update(handle->distance_difference_rb_handle, left_counter - right_counter, &distance_difference);
        speed_difference = pid_update(handle->distance_difference_pid_handle, 0, distance_difference);

        left_duty_cycle = pid_update(handle->left_motor_pid_handle, target_speed_left, left_velocity - speed_difference);
        right_duty_cycle = pid_update(handle->right_motor_pid_handle, target_speed_right, right_velocity + speed_difference);
        dc_motor_set_duty(handle->left_motor_handle, left_duty_cycle);
        dc_motor_set_duty(handle->right_motor_handle, right_duty_cycle);

        if (event->new_state == BUTTON_LONG)
                return;
        if (!is_motor_control_pins(event->pin))
                return;
        if (event->new_state == BUTTON_UP)
        {
                target_speed_left = 0;
                target_speed_right = 0;
                return;
        }

        switch (event->pin)
        {
        case GPIO_BUTTON_UP:
                ESP_LOGI(TAG, "forward");
                target_speed_left = speed_reference;
                target_speed_right = speed_reference;
                break;
        case GPIO_BUTTON_DOWN:
                ESP_LOGI(TAG, "backward");
                target_speed_left = -speed_reference;
                target_speed_right = -speed_reference;
                break;
        case GPIO_BUTTON_LEFT:
                ESP_LOGI(TAG, "rotate left");
                target_speed_left = -speed_reference * 0.5;
                target_speed_right = speed_reference * 0.5;
                break;
        case GPIO_BUTTON_RIGHT:
                ESP_LOGI(TAG, "rotate right");
                target_speed_left = speed_reference * 0.5;
                target_speed_right = -speed_reference * 0.5;
                break;
        default:
                ESP_LOGW(TAG, "button id %d is undefined", event->pin);
                break;
        }
}

void motor_controller_stop_all(motor_controller_handle_t *handle)
{
        dc_motor_set_duty(handle->left_motor_handle, 0);
        dc_motor_set_duty(handle->right_motor_handle, 0);
}

void motor_controller_print_stat(motor_controller_handle_t *handle)
{
        int lcnt = dc_motor_get_count(handle->left_motor_handle);
        int rcnt = dc_motor_get_count(handle->right_motor_handle);
        LOG_INFO("Lcnt:%6d, Rcnt:%6d | Lspd:%6.3f, Rspd:%6.3f | Lacc:%6.3f, Racc:%6.3f | Lpwm:%6.3f, Rpwm:%6.3f | Δd: %6.3f | Δs: %6.3f",
                 lcnt, rcnt,
                 left_velocity, right_velocity,
                 left_acceleration, right_acceleration,
                 left_duty_cycle, right_duty_cycle,
                 distance_difference,
                 speed_difference);
}