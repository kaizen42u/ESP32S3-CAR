
#include "motor_controller.h"

static const char *TAG = "motor_controller";

static motor_group_stat_pkt_t motor_stat;
const int speed_reference = 10;
const float rampup_initial = 0.6, rampup_delta = 0.005;
static float rampup = 0;

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
        static float left_velocity_buffer[VELOCITY_INTEGRATE_SAMPLES];
        static float right_velocity_buffer[VELOCITY_INTEGRATE_SAMPLES];
        ringbuffer_init(&left_velocity_rb_handle, left_velocity_buffer, VELOCITY_INTEGRATE_SAMPLES);
        ringbuffer_init(&right_velocity_rb_handle, right_velocity_buffer, VELOCITY_INTEGRATE_SAMPLES);

        static ringbuffer_handle_t left_acceleration_rb_handle, right_acceleration_rb_handle;
        static float left_acceleration_buffer[ACCELERATION_INTEGRATE_SAMPLES];
        static float right_acceleration_buffer[ACCELERATION_INTEGRATE_SAMPLES];
        ringbuffer_init(&left_acceleration_rb_handle, left_acceleration_buffer, ACCELERATION_INTEGRATE_SAMPLES);
        ringbuffer_init(&right_acceleration_rb_handle, right_acceleration_buffer, ACCELERATION_INTEGRATE_SAMPLES);

        // setup pid controllers

        static pid_handle_t left_motor_pid_handle, right_motor_pid_handle;
        pid_init(&left_motor_pid_handle, 0.01, 0.07, 0.14, 0.007, 0.3);
        pid_init(&right_motor_pid_handle, 0.01, 0.07, 0.13, 0.005, 0.3);
        pid_set_output_range(&left_motor_pid_handle, -100, 100);
        pid_set_output_range(&right_motor_pid_handle, -100, 100);

        static pid_handle_t distance_difference_pid_handle;
        pid_init(&distance_difference_pid_handle, 0.01, 0.02, 0.02, 0.0, 0.4);
        pid_set_output_range(&distance_difference_pid_handle, -10, 10);

        // create the handle

        handle->left_motor_handle = &left_motor_handle;
        handle->right_motor_handle = &right_motor_handle;
        handle->left_velocity_rb_handle = &left_velocity_rb_handle;
        handle->right_velocity_rb_handle = &right_velocity_rb_handle;
        handle->left_acceleration_rb_handle = &left_acceleration_rb_handle;
        handle->right_acceleration_rb_handle = &right_acceleration_rb_handle;
        handle->left_motor_pid_handle = &left_motor_pid_handle;
        handle->right_motor_pid_handle = &right_motor_pid_handle;

        handle->distance_difference_pid_handle = &distance_difference_pid_handle;
        handle->mcpwm_en = GPIO_MCPWM_ENABLE;
        handle->direction = DIRECTION_NONE;
        return handle;
}

motor_controller_handle_t *motor_controller_init(motor_controller_handle_t *handle)
{
        dc_motor_init(handle->left_motor_handle);
        dc_motor_init(handle->right_motor_handle);
        motor_controller_config_mcpwm_enable(handle);
        return handle;
}

void motor_controller_set_direction(motor_controller_handle_t *handle, direction_t direction)
{
        LOG_INFO("direction set [%s ---> %s]", DIRECTION_STRING[handle->direction], DIRECTION_STRING[direction]);
        handle->direction = direction;
        dc_motor_clear_count(handle->left_motor_handle);
        dc_motor_clear_count(handle->right_motor_handle);
        pid_reset(handle->left_motor_pid_handle);
        pid_reset(handle->right_motor_pid_handle);
        pid_reset(handle->distance_difference_pid_handle);
        ringbuffer_clear(handle->left_velocity_rb_handle);
        ringbuffer_clear(handle->right_velocity_rb_handle);
        ringbuffer_clear(handle->left_acceleration_rb_handle);
        ringbuffer_clear(handle->right_acceleration_rb_handle);
}

void motor_controller(motor_controller_handle_t *handle, button_event_t *event)
{
        update_feedback(handle);
        update_pid(handle);
        read_buttons(handle, event);
        update_motors(handle);
}

void update_feedback(motor_controller_handle_t *handle)
{
        motor_stat.left_motor.counter = dc_motor_get_count(handle->left_motor_handle);
        motor_stat.right_motor.counter = dc_motor_get_count(handle->right_motor_handle);
        differentiator_update(handle->left_velocity_rb_handle, motor_stat.left_motor.counter, &motor_stat.left_motor.velocity);
        differentiator_update(handle->right_velocity_rb_handle, motor_stat.right_motor.counter, &motor_stat.right_motor.velocity);
        differentiator_update(handle->left_acceleration_rb_handle, motor_stat.left_motor.velocity, &motor_stat.left_motor.acceleration);
        differentiator_update(handle->right_acceleration_rb_handle, motor_stat.right_motor.velocity, &motor_stat.right_motor.acceleration);
}

void update_pid(motor_controller_handle_t *handle)
{
        motor_stat.delta_distance = motor_stat.left_motor.counter - motor_stat.right_motor.counter;
        motor_stat.delta_velocity = pid_update(handle->distance_difference_pid_handle, 0, motor_stat.delta_distance);

        motor_stat.left_motor.duty_cycle = pid_update(handle->left_motor_pid_handle, motor_stat.left_motor.set_velocity, motor_stat.left_motor.velocity - motor_stat.delta_velocity);
        motor_stat.right_motor.duty_cycle = pid_update(handle->right_motor_pid_handle, motor_stat.right_motor.set_velocity, motor_stat.right_motor.velocity + motor_stat.delta_velocity);

        motor_stat.left_motor.duty_cycle *= rampup;
        motor_stat.right_motor.duty_cycle *= rampup;
        (rampup >= 1) ? (rampup = 1) : (rampup += rampup_delta);
}

void read_buttons(motor_controller_handle_t *handle, button_event_t *event)
{
        if (event->new_state == BUTTON_LONG)
                return;
        if (!is_motor_control_pins(event->pin))
                return;
        if (event->new_state == BUTTON_UP)
        {
                motor_controller_set_direction(handle, DIRECTION_BRAKE);
                set_velocity(0, 0);
                return;
        }
        switch (event->pin)
        {
        case GPIO_BUTTON_UP:
                motor_controller_set_direction(handle, DIRECTION_FORWARD);
                set_velocity(speed_reference, speed_reference);
                break;
        case GPIO_BUTTON_DOWN:
                motor_controller_set_direction(handle, DIRECTION_BACKWARD);
                set_velocity(speed_reference, speed_reference);
                break;
        case GPIO_BUTTON_LEFT:
                motor_controller_set_direction(handle, DIRECTION_TURN_LEFT);
                set_velocity(speed_reference / 2, speed_reference / 2);
                break;
        case GPIO_BUTTON_RIGHT:
                motor_controller_set_direction(handle, DIRECTION_TURN_RIGHT);
                set_velocity(speed_reference / 2, speed_reference / 2);
                break;
        default:
                ESP_LOGW(TAG, "button id %d is undefined", event->pin);
                break;
        }
        rampup = rampup_initial;
}

void set_velocity(float left, float right)
{
        motor_stat.left_motor.set_velocity = left;
        motor_stat.right_motor.set_velocity = right;
}

void update_motors(motor_controller_handle_t *handle)
{
        switch (handle->direction)
        {
        case DIRECTION_FORWARD:
                dc_motor_set_direction(handle->left_motor_handle, MOTOR_DIRECTION_DEFAULT);
                dc_motor_set_direction(handle->right_motor_handle, MOTOR_DIRECTION_DEFAULT);
                dc_motor_set_duty(handle->left_motor_handle, motor_stat.left_motor.duty_cycle);
                dc_motor_set_duty(handle->right_motor_handle, motor_stat.right_motor.duty_cycle);
                break;
        case DIRECTION_BACKWARD:
                dc_motor_set_direction(handle->left_motor_handle, MOTOR_DIRECTION_REVERSED);
                dc_motor_set_direction(handle->right_motor_handle, MOTOR_DIRECTION_REVERSED);
                dc_motor_set_duty(handle->left_motor_handle, motor_stat.left_motor.duty_cycle);
                dc_motor_set_duty(handle->right_motor_handle, motor_stat.right_motor.duty_cycle);
                break;
        case DIRECTION_TURN_LEFT:
                dc_motor_set_direction(handle->left_motor_handle, MOTOR_DIRECTION_REVERSED);
                dc_motor_set_direction(handle->right_motor_handle, MOTOR_DIRECTION_DEFAULT);
                dc_motor_set_duty(handle->left_motor_handle, motor_stat.left_motor.duty_cycle);
                dc_motor_set_duty(handle->right_motor_handle, motor_stat.right_motor.duty_cycle);
                break;
        case DIRECTION_TURN_RIGHT:
                dc_motor_set_direction(handle->left_motor_handle, MOTOR_DIRECTION_DEFAULT);
                dc_motor_set_direction(handle->right_motor_handle, MOTOR_DIRECTION_REVERSED);
                dc_motor_set_duty(handle->left_motor_handle, motor_stat.left_motor.duty_cycle);
                dc_motor_set_duty(handle->right_motor_handle, motor_stat.right_motor.duty_cycle);
                break;
        default:
                dc_motor_coast(handle->left_motor_handle);
                dc_motor_coast(handle->right_motor_handle);
                break;
        }
}

void motor_controller_openloop(motor_controller_handle_t *handle, button_event_t *event)
{
        motor_stat.left_motor.duty_cycle = 50;
        motor_stat.right_motor.duty_cycle = 50;
        update_motors(handle);
        read_buttons(handle, event);
}

void motor_controller_stop_all(motor_controller_handle_t *handle)
{
        dc_motor_brake(handle->left_motor_handle);
        dc_motor_brake(handle->right_motor_handle);
}

motor_group_stat_pkt_t *motor_controller_get_stat(void)
{
        return &motor_stat;
}

void motor_controller_print_stat(void)
{
        LOG_INFO("Lcnt:%6d, Rcnt:%6d | Lspd:%6.3f, Rspd:%6.3f | Lacc:%6.3f, Racc:%6.3f | Lpwm:%6.3f, Rpwm:%6.3f | Δd: %6.3f | Δs: %6.3f",
                 motor_stat.left_motor.counter, motor_stat.right_motor.counter,
                 motor_stat.left_motor.velocity, motor_stat.right_motor.velocity,
                 motor_stat.left_motor.acceleration, motor_stat.right_motor.acceleration,
                 motor_stat.left_motor.duty_cycle, motor_stat.right_motor.duty_cycle,
                 motor_stat.delta_distance,
                 motor_stat.delta_velocity);
}