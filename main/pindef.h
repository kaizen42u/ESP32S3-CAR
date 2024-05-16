
#pragma once

// FUNDUINO JOYSTICK SHIELD
#define JOYSTICK_SHIELD_BUTTON_A   (GPIO_NUM_18) // key A (TOP SIDE button)
#define JOYSTICK_SHIELD_BUTTON_B   (GPIO_NUM_17) // key B (RIGHT SIDE button)
#define JOYSTICK_SHIELD_BUTTON_C   (GPIO_NUM_19) // key C (BOTTOM SIDE button)
#define JOYSTICK_SHIELD_BUTTON_D   (GPIO_NUM_20) // key D (LEFT SIDE button)
#define JOYSTICK_SHIELD_BUTTON_E   (GPIO_NUM_3)  // key E (SMALL RIGHT SIDE button)
#define JOYSTICK_SHIELD_BUTTON_F   (GPIO_NUM_14) // key F (SMALL LEFT SIDE button)
#define JOYSTICK_SHIELD_BUTTON_K   (GPIO_NUM_21) // key K (JOYSTICK button)
#define JOYSTICK_SHIELD_JOYSTICK_X (GPIO_NUM_2)  // potentiometer X (JOYSTICK HORIZONTAL)
#define JOYSTICK_SHIELD_JOYSTICK_Y (GPIO_NUM_1)  // potentiometer Y (JOYSTICK VERTICAL)

// MOTOR DRIVER CONNECTIONS
#define GPIO_MCPWM_UNIT0_PWM0A_OUT (GPIO_NUM_16) // Set GPIO 16 as UNIT0_PWM0A
#define GPIO_MCPWM_UNIT0_PWM0B_OUT (GPIO_NUM_15) // Set GPIO 15 as UNIT0_PWM0B
#define GPIO_MCPWM_UNIT0_PWM1A_OUT (GPIO_NUM_18) // Set GPIO 18 as UNIT0_PWM1A
#define GPIO_MCPWM_UNIT0_PWM1B_OUT (GPIO_NUM_17) // Set GPIO 17 as UNIT0_PWM1B
#define GPIO_MCPWM_ENABLE (GPIO_NUM_7)           // Set GPIO 7 as MCPWM global enable
// #define GPIO_MCPWM_UNIT1_PWM0A_OUT (GPIO_NUM_16) // Set GPIO 16 as UNIT1_PWM0A
// #define GPIO_MCPWM_UNIT1_PWM0B_OUT (GPIO_NUM_17) // Set GPIO 17 as UNIT1_PWM0B
// #define GPIO_MCPWM_UNIT1_PWM1A_OUT (GPIO_NUM_18) // Set GPIO 18 as UNIT1_PWM1A
// #define GPIO_MCPWM_UNIT1_PWM1B_OUT (GPIO_NUM_8)  // Set GPIO  8 as UNIT1_PWM1B

// Motor wheel counters
#define GPIO_MOTORL_COUNTER (GPIO_NUM_2)
#define GPIO_MOTORR_COUNTER (GPIO_NUM_1)

// Car controls
#define GPIO_BUTTON_UP             (GPIO_NUM_4) // !NOT A GPIO, just an ID
#define GPIO_BUTTON_DOWN           (GPIO_NUM_5) // !NOT A GPIO, just an ID
#define GPIO_BUTTON_LEFT           (GPIO_NUM_6) // !NOT A GPIO, just an ID
#define GPIO_BUTTON_RIGHT          (GPIO_NUM_7) // !NOT A GPIO, just an ID
#define GPIO_BUTTON_TOGGLE_CONTROL (JOYSTICK_SHIELD_BUTTON_K)

// Car catapult controls
#define GPIO_BUTTON_CA_SHOOT          (JOYSTICK_SHIELD_BUTTON_C) // Shoot the ball
#define GPIO_BUTTON_CA_INCREASE       (JOYSTICK_SHIELD_BUTTON_D) // Increase power
#define GPIO_BUTTON_CA_DECREASE       (JOYSTICK_SHIELD_BUTTON_B) // Decrease power
#define GPIO_BUTTON_CA_CONFIGURE_MODE (JOYSTICK_SHIELD_BUTTON_E) // Enters the configure mode (tune servo angles)
#define GPIO_BUTTON_CA_NORMAL_MODE    (JOYSTICK_SHIELD_BUTTON_F) // Allow remote control

// Car goal-keeper controls
#define GPIO_BUTTON_GK_SWEEP_MODE  (JOYSTICK_SHIELD_BUTTON_A)
#define GPIO_BUTTON_GK_NORMAL_MODE (JOYSTICK_SHIELD_BUTTON_C)
#define GPIO_BUTTON_GK_TILT_RIGHT  (JOYSTICK_SHIELD_BUTTON_B)
#define GPIO_BUTTON_GK_TILT_LEFT   (JOYSTICK_SHIELD_BUTTON_D)

// 128x64 SSD1306 OLED pinouts
// #define GPIO_SSD1306_SPI_CLOCK (GPIO_NUM_9)
// #define GPIO_SSD1306_SPI_DATA  (GPIO_NUM_10)
// #define GPIO_SSD1306_SPI_RESET (GPIO_NUM_11)
// #define GPIO_SSD1306_SPI_DC    (GPIO_NUM_12)
// #define GPIO_SSD1306_SPI_CS    (U8G2_ESP32_HAL_UNDEFINED)

// HC-SR05 pinouts
// #define GPIO_ULTRASOUND_ECHO (GPIO_NUM_13)
// #define GPIO_ULTRASOUND_TRIG (GPIO_NUM_14)

// Servo motors pinouts
#define GPIO_SERVO_1 (GPIO_NUM_9)
#define GPIO_SERVO_2 (GPIO_NUM_10)
#define GPIO_SERVO_3 (GPIO_NUM_11)
#define GPIO_SERVO_4 (GPIO_NUM_12)

// Laser diode pin
#define GPIO_LASER_PWM (GPIO_NUM_13)

// Power supply enable / disable pin
#define GPIO_WAKE (GPIO_NUM_14)

/*
 * Let's say, GPIO_BUTTON_UP=4, GPIO_BUTTON_DOWN=5, GPIO_BUTTON_LEFT=6, GPIO_BUTTON_RIGHT=7
 * In binary representation,
 * 1ULL<<GPIO_BUTTON_UP    is equal to 0000000000000000000000000000000000010000 and
 * 1ULL<<GPIO_BUTTON_DOWN  is equal to 0000000000000000000000000000000000100000
 * 1ULL<<GPIO_BUTTON_LEFT  is equal to 0000000000000000000000000000000001000000
 * 1ULL<<GPIO_BUTTON_RIGHT is equal to 0000000000000000000000000000000010000000
 * GPIO_INPUT_PIN_SEL                  0000000000000000000000000000000011110000
 * */
