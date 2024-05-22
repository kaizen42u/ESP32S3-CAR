#pragma once

/* ---> Remote Control Car Settings <--- */

// Does the remote control car have a goalkeeper module?
// Options: true, false
// Default: true
#define HAS_GOALKEEPER_MODULE true

// Does the remote control car have a catapult module?
// Options: true, false
// Default: true
#define HAS_CATAPULT_MODULE true

// Use PID control for the remote control car?
// This option REQUIRES the addition hardware of two wheel counters! (which is not present by default)
// Options: true, false
// Default: false
#define CAR_USE_PID_CONTROL false

// Transmit PID debugging messages from the remote control car to the controller via Wi-Fi?
// Options: true, false
// Default: false
#define DEBUG_TRANSMIT_PID_STATUS_TO_REMOTE false

#if (CAR_USE_PID_CONTROL == false)

#else

#define LEFT_MOTOR_P_TERM 0.07
#define LEFT_MOTOR_I_TERM 0.14
#define LEFT_MOTOR_D_TERM 0.007

#define RIGHT_MOTOR_P_TERM 0.07
#define RIGHT_MOTOR_I_TERM 0.14
#define RIGHT_MOTOR_D_TERM 0.007

#endif

// Default motor PWM frequency
// LOWER frequency will cause voltage spikes --> unstable
// HIGHER frequency will cause the motor to run in reduced efficiency --> more PWM value needed
// Unit: frequency - Hz
// Range: 20 to 20000
// Default: 500
#define MOTOR_PWM_FREQUENCY 500

// Super Secret Setting
// Options: true, false
// Default: false
#define ENABLE_MUSIC_PLAYER false

/* ---> Wi-Fi Settings <--- */

// Show Wi-Fi connection status every 3 seconds
// Options: true, false
// Default: false
#define SHOW_CONNECTION_STATUS false

// Clear old connections when uploading new code (Clears ROM)
// Options: true, false
// Default: true
#define CLEAR_PAIRED_PEER_ON_NEW_UPLOAD true

// Attempt to establish a connection when signal strength is greater than this value
// Unit: decibel-milliwatts - dBm
// Range: 0 to -100
// Default: -20
#define MIN_RSSI_TO_INITIATE_CONNECTION -20

/* ---> Built-in RGB LED Settings <--- */
// https://www.selecolor.com/en/hsv-color-picker/

// Built-in RGB LED - Hue
// Unit: angle/phase - °
// Range: 0 to 360
// Default: 350
#define RGB_LED_HUE 350

// Built-in RGB LED - Saturation
// Unit: percentage - %
// Range: 0 to 100
// Default: 75
#define RGB_LED_SATURATION 75

// Built-in RGB LED - Brightness / Value
// Unit: percentage - %
// Range: 0 to 100
// Default: 10
#define RGB_LED_VALUE 10
