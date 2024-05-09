
#include "tone_player.h"

esp_err_t tone_player_load_data(tone_player_handle_t *handle, tone_t *tones, size_t size)
{
        handle->tones = tones;
        handle->size = size;
        handle->start_time_us = 0;
        handle->idle.frequency = 20000;
        return ESP_OK;
}

esp_err_t tone_player_update(tone_player_handle_t *handle, tone_t **output)
{
        if (handle->size <= 0)
        {
                *output = NULL;
                return ESP_ERR_INVALID_SIZE;
        }

        int64_t current_time_us = esp_timer_get_time();

        if (handle->start_time_us == 0)
        {
                handle->index = 0;
                handle->start_time_us = current_time_us;
        }

        int64_t note_duration_us = current_time_us - handle->start_time_us;
        if (note_duration_us > handle->tones[handle->index].duration_us)
        {
                handle->start_time_us += note_duration_us;
                handle->index++;
        }

        if (handle->index >= handle->size)
        {
                handle->index = 0;
                handle->start_time_us = current_time_us;
        }

        *output = &handle->tones[handle->index];
        if ((*output)->frequency == NOTE_MAXIMUM)
        {
                *output = &handle->idle;
        }

        return ESP_OK;
}

esp_err_t tone_player_set_idle_frequency(tone_player_handle_t *handle, uint32_t freq)
{
        handle->idle.frequency = freq;
        return ESP_OK;
}