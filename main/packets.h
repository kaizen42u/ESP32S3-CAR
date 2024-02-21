
#pragma once

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
