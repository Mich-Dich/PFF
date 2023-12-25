#include "core/pch.h"
#include "util.h"


float clamp(const float value, const float min, const float max) {

    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}