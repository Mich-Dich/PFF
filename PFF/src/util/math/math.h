#pragma once

#include <glm/glm.hpp>

namespace PFF::math {

	bool decompose_transform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

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

    template<typename T>
    static FORCEINLINE T add_clamp_overflow(T a, T b, T min, T max) { if (b < 0 && (a < min - b)) return min; if (b > 0 && (a > max - b)) return max; return a + b; }

    template<typename T>
    static FORCEINLINE T sub_clamp_overflow(T a, T b, T min, T max) { if (b > 0 && (a < min + b)) return min; if (b < 0 && (a > max + b)) return max; return a - b; }

}