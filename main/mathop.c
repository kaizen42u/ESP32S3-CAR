
#include "mathop.h"

float constrain(float value, float min, float max)
{
    if (value >= max)
        return max;
    if (value <= min)
        return min;
    return value;
}

float map(float value, float in_max, float in_min, float out_max, float out_min)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}