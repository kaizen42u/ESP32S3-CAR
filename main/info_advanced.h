#pragma once

/* ---> 遙控車設定 <--- */

// 遙控車有守門模組
// 可選: true, false
// 默認: false
#define HAS_GOALKEEPER_MODULE true

// 遙控車有投射器模組
// 可選: true, false
// 默認: false
#define HAS_CATAPULT_MODULE true

// 使用PID控制
// 可選: true, false
// 默認: false
#define CAR_USE_PID_CONTROL false

// 將遙控車PID除錯訊息經由Wi-Fi發送至遙控器
// 可選: true, false
// 默認: false
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

// 默認車輪PWM頻率
// 單位: 頻率 - Hz
// 範圍: 20 to 20000
// 默認: 500
#define MOTOR_PWM_FREQUENCY 500

// ???
// 可選: true, false
// 默認: false
#define ENABLE_MUSIC_PLAYER false

/* ---> Wi-Fi設定 <--- */

// 每隔3秒列出Wi-Fi連線狀態
// 可選: true, false
// 默認: true
#define SHOW_CONNECTION_STATUS false

// 上傳新代碼時清除舊連線 (Clears ROM)
// 可選: true, false
// 默認: true
#define CLEAR_PAIRED_PEER_ON_NEW_UPLOAD true

// 訊號強度大於該值時將嘗試建立連線
// 單位: 分貝毫瓦 - dBm
// 範圍: 0 to -100
// 默認: -20
#define MIN_RSSI_TO_INITIATE_CONNECTION -20

/* ---> 內置彩色LED設定 <--- */
// https://www.selecolor.com/en/hsv-color-picker/

// 內置彩色LED - 色相
// 單位: 角度/相位 - °
// 範圍: 0 to 360
// 默認: 350
#define RGB_LED_HUE 350

// 內置彩色LED - 飽和度
// 單位: 百分比 - %
// 範圍: 0 to 100
// 默認: 75
#define RGB_LED_SATURATION 75

// 內置彩色LED - 明度
// 單位: 百分比 - %
// 範圍: 0 to 100
// 默認: 10
#define RGB_LED_VALUE 10