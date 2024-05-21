#pragma once

#include "../tone_player.h"
#include "../musical_frequencies.h"

// https://www.qupu123.com/jipu/p287007.html
#define BPM (138) // This sets your music BPM
#define BPM_TO_USPB(bpm) (60000000 / (bpm))
#define TONE_WHOLE_NOTE (BPM_TO_USPB(BPM) * 4)
#define TONE_HALF_NOTE (BPM_TO_USPB(BPM) * 2)
#define TONE_QUARTER_NOTE BPM_TO_USPB(BPM)
#define TONE_EIGHTH_NOTE (BPM_TO_USPB(BPM) / 2)
#define TONE_SIXTEENTH_NOTE (BPM_TO_USPB(BPM) / 4)
#define TONE_THIRTY_SECOND_NOTE (BPM_TO_USPB(BPM) / 8)
#define TONE_BREAK_NOTE (1e3)

// Up by #F
#define NOTE_5D NOTE_CS4
#define NOTE_5SD NOTE_D4
#define NOTE_6D NOTE_DS4
#define NOTE_7D NOTE_F4
#define NOTE_1 NOTE_FS4
#define NOTE_2 NOTE_GS4
#define NOTE_3 NOTE_AS4
#define NOTE_4 NOTE_B4
#define NOTE_5 NOTE_CS5
#define NOTE_6 NOTE_DS5
#define NOTE_7 NOTE_F5
#define NOTE_1U NOTE_FS5

// Up by G
#define NOTE_5D_2 NOTE_D4
#define NOTE_5SD_2 NOTE_DS4
#define NOTE_6D_2 NOTE_E4
#define NOTE_7D_2 NOTE_FS4
#define NOTE_1_2 NOTE_G4
#define NOTE_2_2 NOTE_A4
#define NOTE_3_2 NOTE_B4
#define NOTE_4_2 NOTE_C4
#define NOTE_5_2 NOTE_D5
#define NOTE_6_2 NOTE_E5
#define NOTE_7_2 NOTE_FS5
#define NOTE_1U_2 NOTE_G5

static tone_t tones_main[] = {
    // 0
    {NOTE_MAXIMUM, TONE_WHOLE_NOTE},
    // 1
    {NOTE_6D, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_5, TONE_EIGHTH_NOTE},
    // 2
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    // 3
    {NOTE_6D, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    // 4
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_EIGHTH_NOTE},
    {NOTE_5SD, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    // 5
    {NOTE_6D, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_5, TONE_EIGHTH_NOTE},
    // 6
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    // 7
    {NOTE_6D, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    // 8
    {NOTE_7D, TONE_QUARTER_NOTE},
    {NOTE_1, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_QUARTER_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    // 9
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    // 10
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    // 11
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    // 12
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    {NOTE_5SD_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    // 13
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    // 14
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    // 15
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    // 16
    {NOTE_7D_2, TONE_QUARTER_NOTE},
    {NOTE_1_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    // 17
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 18
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 19
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_EIGHTH_NOTE},
    // 20
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 21
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 22
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 23
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_EIGHTH_NOTE},
    // 24
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 25
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 26
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 27
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_EIGHTH_NOTE},
    // 28
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 29
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 30
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_7, TONE_EIGHTH_NOTE},
    // 31
    {NOTE_1U, TONE_EIGHTH_NOTE},
    {NOTE_7, TONE_EIGHTH_NOTE},
    {NOTE_6, TONE_EIGHTH_NOTE},
    {NOTE_5, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_QUARTER_NOTE},
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_3, TONE_EIGHTH_NOTE},
    // 32
    {NOTE_2, TONE_EIGHTH_NOTE},
    {NOTE_1, TONE_EIGHTH_NOTE},
    {NOTE_7D, TONE_EIGHTH_NOTE},
    {NOTE_5D, TONE_EIGHTH_NOTE},
    {NOTE_6D, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    // 33
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 34
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 35
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    // 36
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 37
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 38
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 39
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    // 40
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 41
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 42
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 43
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_EIGHTH_NOTE},
    // 44
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_QUARTER_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 45
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 46
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_7_2, TONE_EIGHTH_NOTE},
    // 47
    {NOTE_1U_2, TONE_EIGHTH_NOTE},
    {NOTE_7_2, TONE_EIGHTH_NOTE},
    {NOTE_6_2, TONE_EIGHTH_NOTE},
    {NOTE_5_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_QUARTER_NOTE},
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_3_2, TONE_EIGHTH_NOTE},
    // 48
    {NOTE_2_2, TONE_EIGHTH_NOTE},
    {NOTE_1_2, TONE_EIGHTH_NOTE},
    {NOTE_7D_2, TONE_EIGHTH_NOTE},
    {NOTE_5D_2, TONE_EIGHTH_NOTE},
    {NOTE_6D_2, TONE_HALF_NOTE},
};

static tone_t tones_sub[] = {
    // Add notes here
};