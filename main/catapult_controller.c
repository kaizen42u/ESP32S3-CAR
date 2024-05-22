
#include "catapult_controller.h"

static const char *TAG = "catapult_controller";

extern const float LASER_SERVO_ANGLES[];
extern const float AIMING_SERVO_ANGLES[];
static size_t array_index = 0;

// Get array length
#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))

size_t get_array_size(void)
{
        const static size_t laser_top = COUNT_OF(LASER_SERVO_ANGLES);
        const static size_t aiming_top = COUNT_OF(AIMING_SERVO_ANGLES);

        if (laser_top != aiming_top)
        {
                LOG_ERROR("Size of 'LASER_SERVO_ANGLES' is different from 'AIMING_SERVO_ANGLES'!")
                return 0;
        }

        if (laser_top <= 0)
        {
                LOG_ERROR("Size of 'LASER_SERVO_ANGLES' is 0!")
                return 0;
        }

        if (aiming_top <= 0)
        {
                LOG_ERROR("Size of 'LASER_SERVO_ANGLES' is 0!")
                return 0;
        }

        return laser_top;
}

void update_servo_angles(catapult_controller_handle_t *handle, size_t array_index)
{
        size_t length = get_array_size();
        if ((length <= 0) || array_index >= length)
        {
                LOG_ERROR("Failed to update servo angle, array_size=%d, array_index=%d", length, array_index);
                return;
        }
        handle->laser_angle = LASER_SERVO_ANGLES[array_index];
        handle->wind_angle = AIMING_SERVO_ANGLES[array_index];
}

void reset_servo_angles(catapult_controller_handle_t *handle)
{
        size_t length = get_array_size();
        if (length <= 0)
                return;
        array_index = 0;
        update_servo_angles(handle, array_index);
}

void next_servo_angles(catapult_controller_handle_t *handle)
{
        size_t length = get_array_size();
        if (length <= 0)
                return;
        if (array_index >= (length - 1)) // off-by-one
                return;
        array_index = array_index + 1;
        update_servo_angles(handle, array_index);
}

void prev_servo_angles(catapult_controller_handle_t *handle)
{
        size_t length = get_array_size();
        if (length <= 0)
                return;
        if (array_index <= 0)
                return;
        array_index = array_index - 1;
        update_servo_angles(handle, array_index);
}

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
        reset_servo_angles(handle);
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
        servo_set_angle(handle->wind, handle->wind_angle);
        servo_set_angle(handle->laser, handle->laser_angle);

        laser_default_config(handle->aiming_laser);
        laser_init(handle->aiming_laser, LEDC_TIMER_1, LEDC_CHANNEL_4, GPIO_LASER_PWM);
        laser_set_duty_cycle(handle->aiming_laser, handle->laser_duty_cycle);
}

void catapult_controller_set_state(catapult_controller_handle_t *handle, catapult_controller_state_t state)
{
        LOG_INFO("state set [%s ---> %s]", CATAPULT_CONTROLLER_STATE_STRING[handle->state], CATAPULT_CONTROLLER_STATE_STRING[state]);
        handle->state = state;
}

void catapult_controller(catapult_controller_handle_t *handle, button_event_t *event)
{
        switch (handle->state)
        {
        // Resets all servo motors to its default
        case STATE_CATAPULT_RESET:
                servo_set_angle(handle->lock, CATAPULT_LOCK_LOCKED_ANGLE);
                reset_servo_angles(handle);
                catapult_controller_set_state(handle, STATE_CATAPULT_IDLE);
                break;

        // Listen to most of the events
        case STATE_CATAPULT_IDLE:
                // When INCREASE is set, increase the shooting power / aiming angle
                if (event->pin == GPIO_BUTTON_CA_INCREASE && event->new_state == BUTTON_DOWN)
                {
                        next_servo_angles(handle);
                        catapult_controller_set_state(handle, STATE_CATAPULT_STRENGTH_INCREASE);
                }
                // When DECREASE is set, decrease the shooting power / aiming angle
                if (event->pin == GPIO_BUTTON_CA_DECREASE && event->new_state == BUTTON_DOWN)
                {
                        prev_servo_angles(handle);
                        catapult_controller_set_state(handle, STATE_CATAPULT_STRENGTH_DECREASE);
                }
                // Goes to SHOOT routine
                if (event->pin == GPIO_BUTTON_CA_SHOOT && event->new_state == BUTTON_DOWN)
                        catapult_controller_set_state(handle, STATE_CATAPULT_SHOOT);
                // Goes to CONFIGURE routine
                if (event->pin == GPIO_BUTTON_CA_CONFIGURE_MODE && event->new_state == BUTTON_DOWN)
                        catapult_controller_set_state(handle, STATE_CATAPULT_SET_SERVO);
                break;

        // Sets all servo to a defined angle to allow for easy assembly
        // Exits when NORMAL mode button is pressed
        case STATE_CATAPULT_SET_SERVO:
                if (event->pin == GPIO_BUTTON_CA_NORMAL_MODE && event->new_state == BUTTON_DOWN)
                        catapult_controller_set_state(handle, STATE_CATAPULT_RESET);
                servo_set_angle(handle->lock, CATAPULT_LOCK_UNLOCKED_ANGLE);
                handle->wind_angle = CATAPULT_WIND_DEFAULT_ANGLE;
                handle->laser_angle = CATAPULT_LASER_SET_ANGLE;
                break;

        // Return to IDLE when the button is released, otherwise block other event
        case STATE_CATAPULT_STRENGTH_INCREASE:
                if (event->pin == GPIO_BUTTON_CA_INCREASE && event->new_state == BUTTON_UP)
                        catapult_controller_set_state(handle, STATE_CATAPULT_IDLE);
                break;

        // Return to IDLE when the button is released, otherwise block other event
        case STATE_CATAPULT_STRENGTH_DECREASE:
                if (event->pin == GPIO_BUTTON_CA_DECREASE && event->new_state == BUTTON_UP)
                        catapult_controller_set_state(handle, STATE_CATAPULT_IDLE);
                break;

        // Shoots the ball and enters DECAY routine
        case STATE_CATAPULT_SHOOT:
                servo_set_angle(handle->lock, CATAPULT_LOCK_UNLOCKED_ANGLE);
                handle->catapult_shot_time = esp_timer_get_time();
                catapult_controller_set_state(handle, STATE_CATAPULT_DECAY);
                break;

        // Slowly turns the winding servo back to its default position
        // To reduce spikes in power rail
        case STATE_CATAPULT_DECAY:
                handle->wind_angle = handle->wind_angle + 0.5;
                // Goes to COOLDOWN routine if the angle is to its minimum (back to default)
                if (handle->wind_angle >= CATAPULT_WIND_DEFAULT_ANGLE)
                        catapult_controller_set_state(handle, STATE_CATAPULT_RESET_DELAY);
                break;

        // Wait for sone time before goes back to IDLE
        // This prevents catapult being triggered immediately right after
        case STATE_CATAPULT_RESET_DELAY:
                if (esp_timer_get_time() - handle->catapult_shot_time > CATAPULT_RESET_TIMEOUT_US)
                        catapult_controller_set_state(handle, STATE_CATAPULT_RESET);
                break;

        // We should never reach this routine!
        default:
                LOG_ERROR("Something went wrong with the code! This line should never be executed.");
                break;
        }

        // ESP_LOGI(TAG, "Wind angle = %f | Laser angle = %f", handle->wind_angle, handle->laser_angle);
        servo_set_angle(handle->wind, handle->wind_angle);
        servo_set_angle(handle->laser, handle->laser_angle);
}
