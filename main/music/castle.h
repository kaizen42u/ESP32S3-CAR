#pragma once

#include "../tone_player.h"
#include "../musical_frequencies.h"

// http://www.tan8.com/post-282922-0-m.html
#define BPM (95)
#define BPM_TO_USPB(bpm) (60000000 / (bpm))
#define TONE_WHOLE_NOTE (BPM_TO_USPB(BPM) * 4)
#define TONE_HALF_NOTE (BPM_TO_USPB(BPM) * 2)
#define TONE_QUARTER_NOTE BPM_TO_USPB(BPM)
#define TONE_EIGHTH_NOTE (BPM_TO_USPB(BPM) / 2)
#define TONE_SIXTEENTH_NOTE (BPM_TO_USPB(BPM) / 4)
#define TONE_THIRTY_SECOND_NOTE (BPM_TO_USPB(BPM) / 8)
#define TONE_BREAK_NOTE (1e3)

static tone_t tones_main[] = {
    // 1
    {NOTE_MAXIMUM, TONE_HALF_NOTE + TONE_QUARTER_NOTE},
    {NOTE_A4, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    // 2
    {NOTE_C5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    // 3
    {NOTE_B4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_E4, TONE_QUARTER_NOTE},
    // 4
    {NOTE_A4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE},
    {NOTE_A4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 5
    {NOTE_G4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_F4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    // 6
    {NOTE_F4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_F4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 7
    {NOTE_E4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    // 8
    {NOTE_B4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_FS4, TONE_EIGHTH_NOTE},
    {NOTE_F4, TONE_QUARTER_NOTE},
    {NOTE_B4, TONE_QUARTER_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 9
    {NOTE_B4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_A4, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    // 10
    {NOTE_C5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    // 11
    {NOTE_B4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    // 12
    {NOTE_A4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE},
    {NOTE_A4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 13
    {NOTE_G4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_E4, TONE_QUARTER_NOTE},
    // 14
    {NOTE_F4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE + TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 15
    {NOTE_D5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE + TONE_HALF_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 16
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_A4, TONE_QUARTER_NOTE},
    {NOTE_B4, TONE_QUARTER_NOTE},
    {NOTE_GS4, TONE_QUARTER_NOTE},
    // 17
    {NOTE_A4, TONE_HALF_NOTE + TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_D5, TONE_EIGHTH_NOTE},
    // 18
    {NOTE_E5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_D5, TONE_EIGHTH_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    {NOTE_G5, TONE_QUARTER_NOTE},
    // 19
    {NOTE_D5, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE},
    // 20
    {NOTE_C5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 21
    {NOTE_E5, TONE_HALF_NOTE + TONE_QUARTER_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    // 22
    {NOTE_A4, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_D5, TONE_QUARTER_NOTE},
    // 23
    {NOTE_C5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 24
    {NOTE_F5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    {NOTE_D5, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 25
    {NOTE_E5, TONE_WHOLE_NOTE},
    // 26
    {NOTE_E5, TONE_HALF_NOTE + TONE_QUARTER_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    // 27
    {NOTE_A5, TONE_HALF_NOTE},
    {NOTE_G5, TONE_HALF_NOTE},
    // 28
    {NOTE_E5, TONE_QUARTER_NOTE},
    {NOTE_D5, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE + TONE_HALF_NOTE},
    // 29
    {NOTE_D5, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_D5, TONE_EIGHTH_NOTE + TONE_QUARTER_NOTE},
    {NOTE_G5, TONE_QUARTER_NOTE},
    // 30
    {NOTE_E5, TONE_HALF_NOTE + TONE_QUARTER_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    // 31
    {NOTE_A5, TONE_HALF_NOTE},
    {NOTE_G5, TONE_HALF_NOTE},
    // 32
    {NOTE_E5, TONE_QUARTER_NOTE},
    {NOTE_D5, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE + TONE_HALF_NOTE},
    // 33
    {NOTE_D5, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_D5, TONE_EIGHTH_NOTE + TONE_QUARTER_NOTE},
    {NOTE_B4, TONE_QUARTER_NOTE},
    // 34
    {NOTE_A4, TONE_HALF_NOTE + TONE_QUARTER_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_A4, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    // 35
    {NOTE_C5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    // 36
    {NOTE_B4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_E4, TONE_QUARTER_NOTE},
    // 37
    {NOTE_A4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE},
    {NOTE_A4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 38
    {NOTE_G4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_F4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    // 39
    {NOTE_F4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_F4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 40
    {NOTE_E4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    // 41
    {NOTE_B4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_FS4, TONE_EIGHTH_NOTE},
    {NOTE_F4, TONE_QUARTER_NOTE},
    {NOTE_B4, TONE_QUARTER_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 42
    {NOTE_B4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_A4, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    // 43
    {NOTE_C5, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_QUARTER_NOTE},
    // 44
    {NOTE_B4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    // 45
    {NOTE_A4, TONE_QUARTER_NOTE + TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_EIGHTH_NOTE},
    {NOTE_A4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 46
    {NOTE_G4, TONE_HALF_NOTE},
    {NOTE_MAXIMUM, TONE_QUARTER_NOTE},
    {NOTE_E4, TONE_QUARTER_NOTE},
    // 47
    {NOTE_F4, TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE + TONE_QUARTER_NOTE},
    {NOTE_C5, TONE_QUARTER_NOTE},
    // 48
    {NOTE_D5, TONE_QUARTER_NOTE},
    {NOTE_E5, TONE_EIGHTH_NOTE},
    {NOTE_C5, TONE_EIGHTH_NOTE + TONE_HALF_NOTE - TONE_BREAK_NOTE},
    {NOTE_MAXIMUM, TONE_BREAK_NOTE},
    // 49
    {NOTE_C5, TONE_EIGHTH_NOTE},
    {NOTE_B4, TONE_EIGHTH_NOTE},
    {NOTE_A4, TONE_QUARTER_NOTE},
    {NOTE_B4, TONE_QUARTER_NOTE},
    {NOTE_GS4, TONE_QUARTER_NOTE},
    // 50
    {NOTE_A4, TONE_WHOLE_NOTE},
};

static tone_t tones_sub[] = {
    // 1
    {NOTE_MAXIMUM, TONE_WHOLE_NOTE},
    // 2
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 3
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 4
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 5
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 6
    {NOTE_D2, TONE_EIGHTH_NOTE},
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_HALF_NOTE},
    // 7
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 8
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 9
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 10
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 11
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 12
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 13
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 14
    {NOTE_D2, TONE_EIGHTH_NOTE},
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_HALF_NOTE},
    // 15
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 16
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 17
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 18
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 19
    {NOTE_G2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_EIGHTH_NOTE},
    {NOTE_D4, TONE_HALF_NOTE},
    // 20
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 21
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 22
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    // 23
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 24
    {NOTE_D2, TONE_EIGHTH_NOTE},
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_HALF_NOTE},
    // 25
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 26
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 27
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    // 28
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 29
    {NOTE_G2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_EIGHTH_NOTE},
    {NOTE_D4, TONE_HALF_NOTE},
    // 30
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 31
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    // 32
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 33
    {NOTE_G2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_EIGHTH_NOTE},
    {NOTE_D4, TONE_HALF_NOTE},
    // 34
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 35
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 36
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 37
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 38
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 39
    {NOTE_D2, TONE_EIGHTH_NOTE},
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_HALF_NOTE},
    // 40
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 41
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_FS3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_HALF_NOTE},
    // 42
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 43
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 44
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_B3, TONE_HALF_NOTE},
    // 45
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_HALF_NOTE},
    // 46
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_G3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_EIGHTH_NOTE},
    {NOTE_G4, TONE_HALF_NOTE},
    // 47
    {NOTE_D2, TONE_EIGHTH_NOTE},
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_D3, TONE_EIGHTH_NOTE},
    {NOTE_F3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_HALF_NOTE},
    // 48
    {NOTE_A2, TONE_EIGHTH_NOTE},
    {NOTE_E3, TONE_EIGHTH_NOTE},
    {NOTE_A3, TONE_EIGHTH_NOTE},
    {NOTE_C4, TONE_EIGHTH_NOTE},
    {NOTE_E4, TONE_HALF_NOTE},
    // 49
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_F2, TONE_EIGHTH_NOTE},
    {NOTE_C3, TONE_EIGHTH_NOTE},
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    {NOTE_E2, TONE_EIGHTH_NOTE},
    {NOTE_B2, TONE_EIGHTH_NOTE},
    // 50
    {NOTE_A3, TONE_WHOLE_NOTE},
};