
#pragma once

#include "ws2812.h"

typedef uint16_t interval_ms_t; // `NOT IMPLEMENTED`
typedef uint16_t voltage_mv_t;  // `NOT IMPLEMENTED`

// Motor status
typedef struct
{
        int counter;        // Pulse counter counts
        float set_velocity; // PID target velocity
        float velocity;     // Motor speed
        float acceleration; // Motor accel
        float duty_cycle;   // Motor PWM duty cycle
} motor_stat_t;

// Motor group status packet
typedef struct
{
        motor_stat_t left_motor, right_motor; // Motor status
        float delta_distance;                 // Difference in counts
        float delta_velocity;                 // Difference in speed
} motor_group_stat_pkt_t;

// `NOT IMPLEMENTED`
typedef struct
{
        voltage_mv_t pot_x, pot_y;                                      // `NOT IMPLEMENTED`
        button_state_t a : 2, b : 2, c : 2, d : 2, e : 2, f : 2, g : 2; // `NOT IMPLEMENTED`
} __packed remote_button_data_t;

// `NOT IMPLEMENTED`
typedef struct
{
        ws2812_rgb_t led; // `NOT IMPLEMENTED`
} led_color_data_t;

// `NOT IMPLEMENTED`
typedef struct
{
        voltage_mv_t battery_mv; // `NOT IMPLEMENTED`
} car_stat_t;

// `NOT IMPLEMENTED`
typedef struct
{
        interval_ms_t motor_stat;   // `NOT IMPLEMENTED`
        interval_ms_t battery_stat; // `NOT IMPLEMENTED`
} car_feature_config_t;
