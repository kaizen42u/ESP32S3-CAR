
#include "catapult_controller.h"

float catapult_controller_constrain(float value, float min, float max)
{
        if (value >= max)
                return max;
        if (value <= min)
                return min;
        return value;
}

catapult_controller_handle_t *catapult_controller_default_config(catapult_controller_handle_t *handle)
{
        static servo_handle_t lock, wind, laser, turn;

        handle->lock = &lock;
        handle->turn = &turn;
        handle->wind = &wind;
        handle->laser = &laser;
        handle->state = STATE_RESET;
        handle->catapult_shot_time = 0;
        handle->turn_angle = CATAPULT_TURN_DEFAULT_ANGLE;
        handle->wind_angle = CATAPULT_WIND_DEFAULT_ANGLE;

        return handle;
}

void catapult_controller_init(catapult_controller_handle_t *handle)
{
        servo_default_config(handle->lock);
        servo_default_config(handle->wind);
        servo_default_config(handle->laser);
        servo_default_config(handle->turn);
        handle->turn->angle_offset = CATAPULT_TURN_ANGLE_OFFSET;
        servo_init(handle->lock, LEDC_TIMER_0, LEDC_CHANNEL_0, GPIO_SERVO_1);
        servo_init(handle->wind, LEDC_TIMER_0, LEDC_CHANNEL_1, GPIO_SERVO_2);
        servo_init(handle->laser, LEDC_TIMER_0, LEDC_CHANNEL_2, GPIO_SERVO_3);
        servo_init(handle->turn, LEDC_TIMER_0, LEDC_CHANNEL_3, GPIO_SERVO_4);
        servo_set_angle(handle->lock, CATAPULT_LOCK_LOCKED_ANGLE);
        servo_set_angle(handle->wind, CATAPULT_WIND_DEFAULT_ANGLE);
        servo_set_angle(handle->laser, CATAPULT_LASER_DEFAULT_ANGLE);
        servo_set_angle(handle->turn, CATAPULT_TURN_DEFAULT_ANGLE);
}

void catapult_controller(catapult_controller_handle_t *handle, button_event_t *event)
{
        switch (handle->state)
        {
        case STATE_RESET:
                servo_set_angle(handle->lock, CATAPULT_LOCK_LOCKED_ANGLE);
                servo_set_angle(handle->wind, CATAPULT_WIND_DEFAULT_ANGLE);
                servo_set_angle(handle->laser, CATAPULT_LASER_DEFAULT_ANGLE);
                handle->wind_angle = CATAPULT_WIND_DEFAULT_ANGLE;
                handle->state = STATE_BASE_TURN;
                break;
        case STATE_BASE_TURN:
                if (event->pin == GPIO_BUTTON_TILT_LEFT && event->event == BUTTON_DOWN)
                        handle->state = STATE_BASE_LEFT;
                if (event->pin == GPIO_BUTTON_TILT_RIGHT && event->event == BUTTON_DOWN)
                        handle->state = STATE_BASE_RIGHT;
                if (event->pin == GPIO_BUTTON_SHOOT && event->event == BUTTON_DOWN)
                        handle->state = STATE_CATAPULT_ACCUMULATE;
                break;
        case STATE_BASE_LEFT:
                if (event->pin == GPIO_BUTTON_TILT_LEFT && event->event == BUTTON_UP)
                        handle->state = STATE_BASE_TURN;
                handle->turn_angle = catapult_controller_constrain(handle->turn_angle + 0.05, 0, 180);
                break;
        case STATE_BASE_RIGHT:
                if (event->pin == GPIO_BUTTON_TILT_RIGHT && event->event == BUTTON_UP)
                        handle->state = STATE_BASE_TURN;
                handle->turn_angle = catapult_controller_constrain(handle->turn_angle - 0.05, 0, 180);
                break;
        case STATE_CATAPULT_ACCUMULATE:
                if (event->pin == GPIO_BUTTON_SHOOT && event->event == BUTTON_UP)
                        handle->state = STATE_CATAPULT_SHOOT;
                handle->wind_angle = catapult_controller_constrain(handle->wind_angle - 0.03, 0, 180);
                break;
        case STATE_CATAPULT_SHOOT:
                servo_set_angle(handle->lock, CATAPULT_LOCK_UNLOCKED_ANGLE);
                handle->catapult_shot_time = esp_timer_get_time();
                handle->state = STATE_CATAPULT_DECAY;
                break;
        case STATE_CATAPULT_DECAY:
                handle->wind_angle = catapult_controller_constrain(handle->wind_angle + 0.03, 0, 145);
                if (handle->wind_angle >= 145.0F) handle->state = STATE_RESET_DELAY;
                break;
        case STATE_RESET_DELAY:
                if (esp_timer_get_time() - handle->catapult_shot_time > CATAPULT_RESET_TIMEOUT_US)
                        handle->state = STATE_RESET;
        default:
                break;
        }

        // ESP_LOGI(TAG, "STATE = %d, Base = %f, Catapult = %f", catapult_state, catapult_base_angle, shooting_angle);
        servo_set_angle(handle->wind, handle->wind_angle);
        servo_set_angle(handle->laser, CATAPULT_LASER_DEFAULT_ANGLE);
        servo_set_angle(handle->turn, handle->turn_angle);
}
