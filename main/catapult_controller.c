
#include "catapult_controller.h"

static const char *TAG = "catapult_controller";
static const float laser_angle_array[] = {110, 108, 106.5, 105.5, 104, 102.5, 101, 101};
static int laser_array_index = 0;

catapult_controller_handle_t *catapult_controller_default_config(catapult_controller_handle_t *handle)
{
        static servo_handle_t lock, wind, laser;
        static laser_handle_t aiming_laser;

        handle->lock = &lock;
        handle->wind = &wind;
        handle->laser = &laser;
        handle->aiming_laser = &aiming_laser;

        handle->state = STATE_CATAPULT_RESET;
        handle->catapult_shot_time = 0;
        handle->wind_angle = CATAPULT_WIND_DEFAULT_ANGLE;
        handle->laser_angle = CATAPULT_LASER_DEFAULT_ANGLE;
        handle->laser_duty_cycle = LASER_POWER;

        return handle;
}

void catapult_controller_init(catapult_controller_handle_t *handle)
{
        servo_default_config(handle->lock);
        servo_default_config(handle->wind);
        servo_default_config(handle->laser);
        servo_init(handle->lock, LEDC_TIMER_0, LEDC_CHANNEL_0, GPIO_SERVO_1);
        servo_init(handle->wind, LEDC_TIMER_0, LEDC_CHANNEL_1, GPIO_SERVO_2);
        servo_init(handle->laser, LEDC_TIMER_0, LEDC_CHANNEL_2, GPIO_SERVO_3);
        servo_set_angle(handle->lock, CATAPULT_LOCK_LOCKED_ANGLE);
        servo_set_angle(handle->wind, CATAPULT_WIND_DEFAULT_ANGLE);
        servo_set_angle(handle->laser, CATAPULT_LASER_DEFAULT_ANGLE);

        laser_default_config(handle->aiming_laser);
        laser_init(handle->aiming_laser, LEDC_TIMER_1, LEDC_CHANNEL_4, GPIO_LASER_PWM);
        laser_set_duty_cycle(handle->aiming_laser, handle->laser_duty_cycle);
}

void catapult_controller(catapult_controller_handle_t *handle, button_event_t *event)
{
        switch (handle->state)
        {
        case STATE_CATAPULT_RESET:
                LOG_INFO("State: STATE_CATAPULT_RESET");
                servo_set_angle(handle->lock, CATAPULT_LOCK_LOCKED_ANGLE);
                servo_set_angle(handle->wind, CATAPULT_WIND_DEFAULT_ANGLE);
                servo_set_angle(handle->laser, CATAPULT_LASER_DEFAULT_ANGLE);
                handle->wind_angle = CATAPULT_WIND_DEFAULT_ANGLE;
                handle->laser_angle = CATAPULT_LASER_DEFAULT_ANGLE;
                laser_array_index = 0;
                LOG_INFO("State: STATE_CATAPULT_IDLE");
                handle->state = STATE_CATAPULT_IDLE;
                break;
        case STATE_CATAPULT_IDLE:
                if (event->pin == GPIO_BUTTON_TILT_LEFT && event->new_state == BUTTON_DOWN)
                {
                        LOG_INFO("State: STATE_STRENGTH_INCREASE");
                        handle->wind_angle = constrain(handle->wind_angle - 20, 5, 145);
                        laser_array_index = constrain(laser_array_index + 1, 0, 7);
                        handle->laser_angle = laser_angle_array[laser_array_index];

                        handle->state = STATE_STRENGTH_INCREASE;
                }
                if (event->pin == GPIO_BUTTON_TILT_RIGHT && event->new_state == BUTTON_DOWN)
                {
                        LOG_INFO("State: STATE_STRENGTH_DECREASE");
                        handle->wind_angle = constrain(handle->wind_angle + 20, 5, 145);
                        laser_array_index = constrain(laser_array_index - 1, 0, 7);
                        handle->laser_angle = laser_angle_array[laser_array_index];

                        handle->state = STATE_STRENGTH_DECREASE;
                }
                if (event->pin == GPIO_BUTTON_SHOOT && event->new_state == BUTTON_DOWN)
                {
                        LOG_INFO("State: STATE_CATAPULT_SHOOT");
                        handle->state = STATE_CATAPULT_SHOOT;
                }
                if (event->pin == GPIO_BUTTON_SET_SERVO && event->new_state == BUTTON_DOWN)
                {
                        LOG_INFO("State: STATE_CATAPULT_SET_SERVO");
                        handle->state = STATE_CATAPULT_SET_SERVO;
                }
                break;
        case STATE_CATAPULT_SET_SERVO:
                if (event->pin == GPIO_BUTTON_RESET_SERVO && event->new_state == BUTTON_DOWN)
                        handle->state = STATE_CATAPULT_RESET;
                servo_set_angle(handle->lock, CATAPULT_LOCK_UNLOCKED_ANGLE);
                handle->wind_angle = CATAPULT_WIND_DEFAULT_ANGLE;
                handle->laser_angle = CATAPULT_LASER_SET_ANGLE;
                break;
        case STATE_STRENGTH_INCREASE:
                if (event->pin == GPIO_BUTTON_TILT_LEFT && event->new_state == BUTTON_UP)
                {
                        LOG_INFO("State: STATE_CATAPULT_IDLE");
                        handle->state = STATE_CATAPULT_IDLE;
                }
                break;
        case STATE_STRENGTH_DECREASE:
                if (event->pin == GPIO_BUTTON_TILT_RIGHT && event->new_state == BUTTON_UP)
                {
                        LOG_INFO("State: STATE_CATAPULT_IDLE");
                        handle->state = STATE_CATAPULT_IDLE;
                }
                break;
        case STATE_CATAPULT_SHOOT:
                servo_set_angle(handle->lock, CATAPULT_LOCK_UNLOCKED_ANGLE);
                handle->catapult_shot_time = esp_timer_get_time();
                LOG_INFO("State: STATE_CATAPULT_DECAY");
                handle->state = STATE_CATAPULT_DECAY;
                break;
        case STATE_CATAPULT_DECAY:
                handle->wind_angle = constrain(handle->wind_angle + 0.5, 0, 145);
                if (handle->wind_angle >= CATAPULT_WIND_DEFAULT_ANGLE)
                {
                        LOG_INFO("State: STATE_RESET_DELAY");
                        handle->state = STATE_RESET_DELAY;
                }
                break;
        case STATE_RESET_DELAY:
                if (esp_timer_get_time() - handle->catapult_shot_time > CATAPULT_RESET_TIMEOUT_US)
                        handle->state = STATE_CATAPULT_RESET;
        default:
                break;
        }

        // ESP_LOGI(TAG, "Wind angle = %f | Laser angle = %f", handle->wind_angle, handle->laser_angle);
        servo_set_angle(handle->wind, handle->wind_angle);
        servo_set_angle(handle->laser, handle->laser_angle);
}
