#pragma once

#include <glm/glm.hpp>

namespace PFF::math {

    bool is_valid_vec3(const glm::vec3& vec);

	bool decompose_transform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

	bool compose_transform(glm::mat4& transform, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

    PFF_API FORCEINLINE f32 calc_array_average(const f32* array, u32 size);

    PFF_API FORCEINLINE f32 calc_array_max(const f32* array, u32 size);

    template<typename T>
    static FORCEINLINE T min(T left, T right) { return left < right ? left : right; }

    template<typename T>
    static FORCEINLINE T max(T left, T right) { return left >= right ? left : right; }

    template<typename T>
    static FORCEINLINE T clamp(T value, T min, T max) { return (value < min) ? min : (value > max) ? max : value; }

    template<typename T>
    static FORCEINLINE T lerp(T a, T b, float time) { return (T)(a + (b - a) * time); }

    template<typename T>
    static FORCEINLINE void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

    // Absolute value function
    template<typename T>
    static FORCEINLINE T abs(T value) { return value < 0 ? -value : value; }

    template<typename T>
    static FORCEINLINE T add_clamp_overflow(T a, T b, T min, T max) { if (b < 0 && (a < min - b)) return min; if (b > 0 && (a > max - b)) return max; return a + b; }

    template<typename T>
    static FORCEINLINE T sub_clamp_overflow(T a, T b, T min, T max) { if (b > 0 && (a < min + b)) return min; if (b < 0 && (a > max + b)) return max; return a - b; }

}
