
#pragma once

#include <string.h>

#include "esp_timer.h"

#include "musical_frequencies.h"

typedef struct
{
        float frequency;
        int64_t duration_us;
} tone_t;

typedef struct
{
        tone_t *tones;
        size_t size;
        size_t index;
        int64_t start_time_us;

        tone_t idle;
} tone_player_handle_t;

esp_err_t tone_player_load_data(tone_player_handle_t *handle, tone_t *tones, size_t size);
esp_err_t tone_player_update(tone_player_handle_t *handle, tone_t **output);
esp_err_t tone_player_set_idle_frequency(tone_player_handle_t *handle, uint32_t freq);
