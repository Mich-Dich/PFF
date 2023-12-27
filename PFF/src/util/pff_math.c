
#include "pch.h"
#include "pff_math.h"



void clamp_f(f32 value, const f32 min, const f32 max) {

    if (value < min)
        value = min;

    else if (value > max)
        value = max;
}


f32 clamp_f_get(const f32 value, const f32 min, const f32 max) {

    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    } else {
        return value;
    }
}


i32 rand_in_range(i32 min, i32 max) {

    // Ensure that min is less than or equal to max
    if (min > max) {
        i32 temp = min;
        min = max;
        max = temp;
    }

    i32 range_size = max - min + 1;
    return (rand() % range_size) + min;
}