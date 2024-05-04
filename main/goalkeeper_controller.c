
#include "goalkeeper_controller.h"

static const char *TAG = "goalkeeper_controller";

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

void goalkeeper_controller(goalkeeper_controller_handle_t *handle, button_event_t *event)
{
        switch (handle->state)
        {
        case STATE_GOALKEEPER_RESET:
                LOG_INFO("State: STATE_GOALKEEPER_RESET");
                handle->goalkeeping_servo_angle = GOALKEEPER_DEFAULT_ANGLE;
                LOG_INFO("State: STATE_GOALKEEPER_IDLE");
                handle->state = STATE_GOALKEEPER_IDLE;
                break;
        case STATE_GOALKEEPER_IDLE:
                handle->goalkeeping_servo_angle = GOALKEEPER_DEFAULT_ANGLE;
                if (event->pin == GPIO_BUTTON_TILT_LEFT && event->new_state == BUTTON_DOWN)
                {
                        LOG_INFO("State: STATE_GOALKEEPER_LEFT");
                        handle->state = STATE_GOALKEEPER_LEFT;
                }
                if (event->pin == GPIO_BUTTON_TILT_RIGHT && event->new_state == BUTTON_DOWN)
                {
                        LOG_INFO("State: STATE_GOALKEEPER_RIGHT");
                        handle->state = STATE_GOALKEEPER_RIGHT;
                }
                break;
        case STATE_GOALKEEPER_LEFT:
                if (event->pin == GPIO_BUTTON_TILT_LEFT && event->new_state == BUTTON_UP)
                {
                        LOG_INFO("State: STATE_GOALKEEPER_IDLE");
                        handle->state = STATE_GOALKEEPER_IDLE;
                }
                handle->goalkeeping_servo_angle = GOALKEEPER_TILT_LEFT_ANGLE;
                break;
        case STATE_GOALKEEPER_RIGHT:
                if (event->pin == GPIO_BUTTON_TILT_RIGHT && event->new_state == BUTTON_UP)
                {
                        LOG_INFO("State: STATE_GOALKEEPER_IDLE");
                        handle->state = STATE_GOALKEEPER_IDLE;
                }
                handle->goalkeeping_servo_angle = GOALKEEPER_TILT_RIGHT_ANGLE;
                break;
        default:
                break;
        }
        // static const char *TAG = "Goalkeeper";
        // ESP_LOGI(TAG, "Angle = %f", handle->goalkeeping_servo_angle);
        servo_set_angle(handle->goalkeeping_servo, handle->goalkeeping_servo_angle);
}
