
#pragma once

#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "button.h"
#include "espnow.h"
#include "pindef.h"

void button_espnow_loop(void);
