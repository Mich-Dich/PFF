#pragma once

#include <glm/glm.hpp>

namespace PFF::math {

    bool is_valid_vec3(const glm::vec3& vec);

	bool decompose_transform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale);

	bool compose_transform(glm::mat4& transform, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale);

    f32 calc_array_average(const f32* array, u32 size);

    f32 calc_array_max(const f32* array, u32 size);
    
#if 0
    
#include <concepts>

    // Define concepts for the required operations
    template<typename T>
    concept less_than_comparable = requires(T a, T b) { { a < b } -> std::convertible_to<bool>; };

    template<typename T>
    concept greater_than_or_equal_comparable = requires(T a, T b) { { a >= b } -> std::convertible_to<bool>; };

    template<typename T>
    concept addable = requires(T a, T b) { { a + b } -> std::convertible_to<T>; };

    template<typename T>
    concept subtractable = requires(T a, T b) { { a - b } -> std::convertible_to<T>; };

    // Use the concepts in the templates
    template<less_than_comparable T>
    static FORCEINLINE T min(T left, T right) { return left < right ? left : right; }

    template<greater_than_or_equal_comparable T>
    static FORCEINLINE T max(T left, T right) { return left >= right ? left : right; }

    template<less_than_comparable T, greater_than_or_equal_comparable T2>
    static FORCEINLINE T clamp(T value, T min, T max) { return (value < min) ? min : (value > max) ? max : value; }

    template<addable T>
    static FORCEINLINE T lerp(T a, T b, float time) { return (T)(a + (b - a) * time); }

    template<typename T>
    static FORCEINLINE void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

    // Absolute value function
    template<less_than_comparable T>
    static FORCEINLINE T abs(T value) { return value < 0 ? -value : value; }

    template<addable T, less_than_comparable T2>
    static FORCEINLINE T add_clamp_overflow(T a, T b, T min, T max) {
        if (b < 0 && (a < min - b)) return min;
        if (b > 0 && (a > max - b)) return max;
        return a + b;
    }

    template<subtractable T, less_than_comparable T2>
    static FORCEINLINE T sub_clamp_overflow(T a, T b, T min, T max) {
        if (b > 0 && (a < min + b)) return min;
        if (b < 0 && (a > max + b)) return max;
        return a - b;
    }

#else
   
#undef min
#undef max

    template<typename T>
    static FORCEINLINE T min(const T left, const T right) { return left < right ? left : right; }

    template<typename T>
    static FORCEINLINE T max(const T left, const T right) { return left >= right ? left : right; }

    template<typename T>
    static FORCEINLINE T clamp(const T value, const T min, const T max) { return (value < min) ? min : (value > max) ? max : value; }

    template<typename T>
    static FORCEINLINE T lerp(const T a, const T b, const float time) { return (T)(a + (b - a) * time); }

    //template<typename T>
    //static FORCEINLINE void swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }

    //// Absolute value function
    //template<typename T>
    //static FORCEINLINE T abs(const T value) { return value < 0 ? -value : value; }

    //template<typename T>
    //static FORCEINLINE T add_clamp_overflow(const T a, const T b, const T min, const T max) { if (b < 0 && (a < min - b)) return min; if (b > 0 && (a > max - b)) return max; return a + b; }

    //template<typename T>
    //static FORCEINLINE T sub_clamp_overflow(const T a, const T b, const T min, const T max) { if (b > 0 && (a < min + b)) return min; if (b < 0 && (a > max + b)) return max; return a - b; }


    //template <typename T>
    //static FORCEINLINE T map(const T value, const T in_min, const T in_max, const T out_min, const T out_max) {

    //    if /*constexpr*/ (in_max == in_min)                                                               // Ensure the input range is valid
    //        throw std::invalid_argument("Input range cannot be zero.");

    //    return out_min + (value - in_min) * (out_max - out_min) / (in_max - in_min);        // Calculate the mapped value
    //}

    //template<typename T>
    //static FORCEINLINE T map(T value, T in_min, T in_max, T out_min, T out_max) {
    //    // Clamp the input value to the input range
    //    value = clamp(value, in_min, in_max);
    //
    //    // Normalize the value to a 0-1 range based on the input range
    //    T normalized = (value - in_min) / (in_max - in_min);
    //
    //    // Scale the normalized value to the output range
    //    return lerp(out_min, out_max, normalized);
    //}

#endif

    // @brief Combines hash values. from: https://stackoverflow.com/a/57595105
    // @tparam T The type of the value to hash.
    // @tparam Rest Additional types to hash.
    // @param seed The seed value for the hash.
    // @param v The value to hash.
    // @param rest Additional values to hash.
    template <typename T, typename... Rest>
    constexpr void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {

        seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
        (hash_combine(seed, rest), ...);
    }

}
