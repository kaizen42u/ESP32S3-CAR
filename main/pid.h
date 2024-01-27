
#pragma once

#include "mathop.h"
#include <stdbool.h>

typedef struct
{
	float dt, Kp, Ki, Kd;
	float output, out_min, out_max;
	float prev_measurement, prev_error;
	float integrator, integrator_min, integrator_max;
	float differentiator, differentiator_tau;
	bool first_time;
} pid_handle_t;

void pid_init(pid_handle_t *handle, float dt, float Kp, float Ki, float Kd, double Ki_cap);

void pid_reset(pid_handle_t *handle);

void pid_set_timebase(pid_handle_t *handle, float dt);
void pid_set_tuning_values(pid_handle_t *handle, float Kp, float Ki, float Kd);
void pid_set_output_range(pid_handle_t *handle, float out_min, float out_max);
void pid_set_integrator_cap(pid_handle_t *handle, float imin, float imax);

float pid_update(pid_handle_t *handle, float setpoint, float measurement);
