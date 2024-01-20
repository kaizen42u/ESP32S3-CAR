
#include "motor.h"

motor_handle_t *brushed_motor_default_config(motor_handle_t *handle)
{
        handle->ch_A_pin = GPIO_NUM_NC;
        handle->ch_B_pin = GPIO_NUM_NC;
        handle->mcpwm_config.cmpr_a = 0;
        handle->mcpwm_config.cmpr_b = 0;
        handle->mcpwm_config.counter_mode = MCPWM_UP_COUNTER;
        handle->mcpwm_config.duty_mode = MCPWM_DUTY_MODE_0;
        handle->mcpwm_config.frequency = 60;
        return handle;
}

void brushed_motor_init(motor_handle_t *handle)
{
        esp_err_t ret;
        ret = mcpwm_gpio_init(handle->mcpwm_unit, handle->mcpwm_ch_A_signal, handle->ch_A_pin);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_gpio_init(handle->mcpwm_unit, handle->mcpwm_ch_B_signal, handle->ch_B_pin);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_init(handle->mcpwm_unit, handle->mcpwm_timer, &handle->mcpwm_config);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_deadtime_disable(handle->mcpwm_unit, handle->mcpwm_timer);
        ESP_ERROR_CHECK(ret);
}

void brushed_motor_set(motor_handle_t *handle, float duty_cycle)
{
        if (duty_cycle < 0.0F)
                brushed_motor_backward(handle, -duty_cycle);
        else if (duty_cycle > 0.0F)
                brushed_motor_forward(handle, duty_cycle);
        else
                brushed_motor_stop(handle);
}

void brushed_motor_forward(motor_handle_t *handle, float duty_cycle)
{
        esp_err_t ret;
        ret = mcpwm_set_signal_low(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_A);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_set_duty(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_B, duty_cycle);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_set_duty_type(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_B, handle->mcpwm_config.duty_mode); // call this each time, if operator was previously in low/high state
        ESP_ERROR_CHECK(ret);
}

void brushed_motor_backward(motor_handle_t *handle, float duty_cycle)
{
        esp_err_t ret;
        ret = mcpwm_set_signal_low(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_B);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_set_duty(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_A, duty_cycle);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_set_duty_type(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_A, handle->mcpwm_config.duty_mode); // call this each time, if operator was previously in low/high state
        ESP_ERROR_CHECK(ret);
}

void brushed_motor_stop(motor_handle_t *handle)
{
        esp_err_t ret;
        ret = mcpwm_set_signal_low(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_A);
        ESP_ERROR_CHECK(ret);
        ret = mcpwm_set_signal_low(handle->mcpwm_unit, handle->mcpwm_timer, MCPWM_OPR_B);
        ESP_ERROR_CHECK(ret);
}
