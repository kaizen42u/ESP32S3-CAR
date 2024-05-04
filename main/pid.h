
#pragma once

#include "mathop.h"
#include <stdbool.h>

// All the resources that are used for the PID controller
typedef struct
{
	float dt, Kp, Ki, Kd;				  // PID parameters
	float output;						  // Result of PID calculations
	float out_min, out_max;				  // Limit of PID outputs
	float prev_measurement, prev_error;	  // Intermediate values
	float integrator;					  // Integrator sums
	float integrator_min, integrator_max; // Limit of the Integrator
	float differentiator;				  // Differentiator value
	float differentiator_tau;			  // Time base of differentiator
	bool first_time;					  // Low accuracy mode
} pid_handle_t;

// Initialize the PID controller
void pid_init(pid_handle_t *handle, float dt, float Kp, float Ki, float Kd, double Ki_cap);

// Clears PID internal state to default
void pid_reset(pid_handle_t *handle);

// Sets time base of PID
void pid_set_timebase(pid_handle_t *handle, float dt);
// Sets the Kp, Ki, and Kd of PID
void pid_set_tuning_values(pid_handle_t *handle, float Kp, float Ki, float Kd);
// Sets the output range of PID
void pid_set_output_range(pid_handle_t *handle, float out_min, float out_max);
// Sets the integrator range of PID
void pid_set_integrator_cap(pid_handle_t *handle, float imin, float imax);

// Updates PID controller
float pid_update(pid_handle_t *handle, float setpoint, float measurement);
