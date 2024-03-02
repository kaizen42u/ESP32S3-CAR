
#pragma once

#include "ws2812.h"

typedef uint16_t interval_ms_t;
typedef uint16_t voltage_mv_t;

typedef struct
{
        int counter;
        float set_velocity;
        float velocity, acceleration, duty_cycle;
} motor_stat_t;

typedef struct
{
        motor_stat_t left_motor, right_motor;
        float delta_distance, delta_velocity;
} motor_group_stat_pkt_t;

typedef struct
{
        voltage_mv_t pot_x, pot_y;
        button_state_t a : 2, b : 2, c : 2, d : 2, e : 2, f : 2, g : 2;
} __packed remote_button_data_t;

typedef struct
{
        ws2812_rgb_t led;
} led_color_data_t;

typedef struct
{
        voltage_mv_t battery_mv;
} car_stat_t;

typedef struct
{
        interval_ms_t motor_stat;
        interval_ms_t battery_stat;
} car_feature_config_t;
