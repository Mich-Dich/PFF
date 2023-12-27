#pragma once

#include "dll_interface.h"

/**
 * Override [value] with the clamped value within the specified range [min, max].
 *
 * @param value f32 value to be clamped.
 * @param min The minimum value of the range (inclusive).
 * @param max The maximum value of the range (inclusive).
 */
PFF_API void clamp_f(f32 value, const f32 min, const f32 max);


/**
 * Get the clamped value of a 32-bit floating-point number within the specified range [min, max].
 *
 * @param value The f32 value to be clamped (not changing value).
 * @param min The minimum value of the range (inclusive).
 * @param max The maximum value of the range (inclusive).
 * @return The clamped value.
 */
PFF_API f32 clamp_f_get(const f32 value, const f32 min, const f32 max);


/**
 * Generate a random u32 within the specified range [min, max]
 *
 * @param min The minimum value of the range (inclusive)
 * @param max The maximum value of the range (inclusive)
 * @return A random u32 within the specified range
 */
PFF_API i32 rand_in_range(i32 min, i32 max);

/**
 * Generate a random f32 within the specified range [min, max]
 *
 * @param min The minimum value of the range (inclusive)
 * @param max The maximum value of the range (inclusive)
 * @return A random f32 within the specified range
 */
PFF_API f32 rand_f_in_range(f32 min, f32 max);