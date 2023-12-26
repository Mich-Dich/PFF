
#include "pch.h"
#include "pff_math.h"


float clamp(const float value, const float min, const float max) {

    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}
