#pragma once
#include "info_advanced.h"

/* ---> 遙控車設定 <--- */

// 當遙控車與遙控器斷開連接後，多少秒後自動關機
// 單位: 秒 - s
// 範圍: 10 to 100
// 默認: 10
#define IDLE_SHUTDOWN_SECONDS 10

#if (CAR_USE_PID_CONTROL == false)

// 左車輪PWM
// 單位: 百分比 - %
// 範圍: 25 to 100
// 默認: 35
#define LEFT_MOTOR_POWER 35

// 右車輪PWM
// 單位: 百分比 - %
// 範圍: 25 to 100
// 默認: 35
#define RIGHT_MOTOR_POWER 35

#endif

// 設定後，遙控車將會剎車而非滑行
// 可選: true, false
// 默認: false
#define MOTOR_BRAKE_ON_IDLE false

/* ---> 投射器設定 <--- */

// 激光強度
// 單位: 百分比 - %
// 範圍: 0 to 100
// 默認: 10
#define LASER_POWER 10
