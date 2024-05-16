
#include "goalkeeper_controller.h"

static const char *TAG = "goalkeeper_controller";
static uint64_t previous_time, auto_mode_delay_time;
static int auto_mode;

goalkeeper_controller_handle_t *goalkeeper_controller_default_config(goalkeeper_controller_handle_t *handle)
{
        static servo_handle_t goalkeeper_servo;
        handle->goalkeeping_servo = &goalkeeper_servo;
        handle->state = STATE_GOALKEEPER_RESET;
        handle->goalkeeping_servo_angle = GOALKEEPER_DEFAULT_ANGLE;
        return handle;
}

void goalkeeper_controller_init(goalkeeper_controller_handle_t *handle)
{
        servo_default_config(handle->goalkeeping_servo);
        servo_init(handle->goalkeeping_servo, LEDC_TIMER_0, LEDC_CHANNEL_3, GPIO_SERVO_4);
        servo_set_angle(handle->goalkeeping_servo, GOALKEEPER_DEFAULT_ANGLE);
}

void goalkeeper_controller_set_state(goalkeeper_controller_handle_t *handle, goalkeeper_controller_state_t state)
{
        LOG_INFO("state set [%s ---> %s]", GOALKEEPER_CONTROLLER_STATE_STRING[handle->state], GOALKEEPER_CONTROLLER_STATE_STRING[state]);
        handle->state = state;
}

void goalkeeper_controller(goalkeeper_controller_handle_t *handle, button_event_t *event)
{
        switch (handle->state)
        {
        // Resets goalkeeping servo motor to its default angle
        case STATE_GOALKEEPER_RESET:
                handle->goalkeeping_servo_angle = GOALKEEPER_DEFAULT_ANGLE;
                goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_IDLE);
                break;

        // Listen to most of the events
        case STATE_GOALKEEPER_IDLE:
                handle->goalkeeping_servo_angle = GOALKEEPER_DEFAULT_ANGLE;
                if (event->pin == GPIO_BUTTON_GK_TILT_LEFT && event->new_state == BUTTON_DOWN)
                        goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_LEFT);
                if (event->pin == GPIO_BUTTON_GK_TILT_RIGHT && event->new_state == BUTTON_DOWN)
                        goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_RIGHT);
                if (event->pin == GPIO_BUTTON_GK_SWEEP_MODE && event->new_state == BUTTON_DOWN)
                {
                        goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_AUTO);
                        LOG_INFO("Auto Mode Speed: SLOW");
                        previous_time = esp_timer_get_time();
                        auto_mode_delay_time = AUTO_SPEED_SLOW;
                        auto_mode = 0;
                }
                break;
        case STATE_GOALKEEPER_LEFT:
                // If the button is released, change to IDLE mode
                if (event->pin == GPIO_BUTTON_GK_TILT_LEFT && event->new_state == BUTTON_UP)
                        goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_IDLE);
                // Tilt the model to left
                handle->goalkeeping_servo_angle = GOALKEEPER_TILT_LEFT_ANGLE;
                break;
        case STATE_GOALKEEPER_RIGHT:
                // If the button is released, change to IDLE mode
                if (event->pin == GPIO_BUTTON_GK_TILT_RIGHT && event->new_state == BUTTON_UP)
                        goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_IDLE);
                // Tile the model to right
                handle->goalkeeping_servo_angle = GOALKEEPER_TILT_RIGHT_ANGLE;
                break;
        case STATE_GOALKEEPER_AUTO:
                if (event->pin == GPIO_BUTTON_GK_NORMAL_MODE && event->new_state == BUTTON_UP)
                        goalkeeper_controller_set_state(handle, STATE_GOALKEEPER_IDLE);
                if (event->pin == GPIO_BUTTON_GK_SWEEP_MODE && event->new_state == BUTTON_DOWN) // If button A is pressed, change the auto mode
                {
                        // Change the auto mode index
                        // After FAST mode ends, return to SLOW mode
                        auto_mode = (auto_mode + 1) % 3;

                        switch (auto_mode)
                        {
                        case 0:
                                LOG_INFO("Auto Mode Speed: SLOW");
                                auto_mode_delay_time = AUTO_SPEED_SLOW;
                                break;
                        case 1:
                                LOG_INFO("Auto Mode Speed: MIDDLE");
                                auto_mode_delay_time = AUTO_SPEED_MIDDLE;
                                break;
                        case 2:
                                LOG_INFO("Auto Mode Speed: FAST");
                                auto_mode_delay_time = AUTO_SPEED_FAST;
                                break;
                        default:
                                break;
                        }
                }
                if (esp_timer_get_time() - previous_time > auto_mode_delay_time) // Change the servo angle in different speed
                {
                        if (handle->goalkeeping_servo_angle != GOALKEEPER_TILT_LEFT_ANGLE)
                                handle->goalkeeping_servo_angle = GOALKEEPER_TILT_LEFT_ANGLE;
                        else
                                handle->goalkeeping_servo_angle = GOALKEEPER_TILT_RIGHT_ANGLE;
                        previous_time = esp_timer_get_time();
                }
                break;
        default:
                break;
        }
        // static const char *TAG = "Goalkeeper";
        // ESP_LOGI(TAG, "Angle = %f", handle->goalkeeping_servo_angle);
        servo_set_angle(handle->goalkeeping_servo, handle->goalkeeping_servo_angle);
}
