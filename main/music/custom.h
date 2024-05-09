#pragma once

#include "../tone_player.h"
#include "../musical_frequencies.h"

#define BPM (95) // This sets your music BPM
#define BPM_TO_USPB(bpm) (60000000 / (bpm))
#define TONE_WHOLE_NOTE (BPM_TO_USPB(BPM) * 4)
#define TONE_HALF_NOTE (BPM_TO_USPB(BPM) * 2)
#define TONE_QUARTER_NOTE BPM_TO_USPB(BPM)
#define TONE_EIGHTH_NOTE (BPM_TO_USPB(BPM) / 2)
#define TONE_SIXTEENTH_NOTE (BPM_TO_USPB(BPM) / 4)
#define TONE_THIRTY_SECOND_NOTE (BPM_TO_USPB(BPM) / 8)
#define TONE_BREAK_NOTE (1e3)

static tone_t tones_main[] = {
    // Add notes here
};

static tone_t tones_sub[] = {
    // Add notes here
};