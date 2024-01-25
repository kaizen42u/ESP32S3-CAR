
#include "pid.h"

void pid_init(pid_handle_t *handle, float dt, float Kp, float Ki, float Kd, double Ki_cap)
{
	pid_set_timebase(handle, dt);
	pid_set_tuning_values(handle, Kp, Ki, Kd);
	pid_set_integrator_cap(handle, -0.2, 0.2);
	pid_set_output_range(handle, -1, 1);
	pid_reset(handle);
}

void pid_reset(pid_handle_t *handle)
{
	handle->first_time = true;
	handle->output = 0;
	handle->integrator = 0;
	handle->differentiator = 0;
	handle->prev_error = 0;
	handle->prev_measurement = 0;
}

void pid_set_timebase(pid_handle_t *handle, float dt)
{
	handle->dt = dt;
	handle->differentiator_tau = handle->dt * 2;
}

void pid_set_tuning_values(pid_handle_t *handle, float Kp, float Ki, float Kd)
{
	handle->Kp = Kp;
	handle->Ki = Ki;
	handle->Kd = Kd;
}

void pid_set_output_range(pid_handle_t *handle, float out_min, float out_max)
{
	handle->out_min = out_min;
	handle->out_max = out_max;
}

void pid_set_integrator_cap(pid_handle_t *handle, float imin, float imax)
{
	handle->integrator_min = imin;
	handle->integrator_max = imax;
}

float pid_update(pid_handle_t *handle, float setpoint, float measurement)
{
	float error = setpoint - measurement;
	double proportional = handle->Kp * error;
	handle->integrator = handle->integrator + 0.5f * handle->Ki * handle->dt * (error + handle->prev_error);
	handle->integrator = constrain(handle->integrator, handle->integrator_min, handle->integrator_max);
	if (!handle->first_time)
		handle->differentiator = -(2.0f * handle->Kd * (measurement - handle->prev_measurement) + (2.0f * handle->differentiator_tau - handle->dt) * handle->differentiator) / (2.0f * handle->differentiator_tau + handle->dt);
	handle->output = proportional + handle->integrator + handle->differentiator;
	handle->output = constrain(handle->output, -1, 1);

	handle->prev_error = error;
	handle->prev_measurement = measurement;
	handle->first_time = false;
	return map(handle->output, -1, 1, handle->out_min, handle->out_max);
}
