
#pragma once

// Constrains a number to be within a range
float constrain(float value, float min, float max);

// Re-maps a number from one range to another.
// That is, a value of in_min would get mapped to out_min, a value of in_max to out_max, values in-between to values in-between, etc.
float map(float value, float in_min, float in_max, float out_min, float out_max);
