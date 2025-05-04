#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

namespace PFF::util {
    
    // glm::vec3 MY_rand_vec3() { return glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f)); }


    // @brief A class that provides methods for generating random numbers of various types.
    //        It utilizes the Mersenne Twister pseudo-random number generator for high-quality randomness.
    //        The class can generate floating-point numbers and unsigned integers within specified ranges.
    class random {
    public:

        random(u32 seed = std::random_device{}())
            : engine(seed) {}

        template<typename T>
        T get(T min, T max) {

            if constexpr (std::is_floating_point_v<T>) {

                std::uniform_real_distribution<T> dist(min, max);
                return dist(engine);

            } else if constexpr (std::is_integral_v<T>) {

                std::uniform_int_distribution<T> dist(min, max);
                return dist(engine);
            }
        }

        glm::vec3 get_vec3(f32 min = -1.0f, f32 max = 1.0f);

        // @brief Generates a random floating-point number in the range [0.f, 1.f] and checks if its less then the percent.
        // @param [percentage] The value for percent checking (needs to be between 0.f and 1.f).
        // @return A random floating-point number between min and max.
        bool get_percent(f32 percentage = 0.0f);

        std::string get_string(const size_t length);

    private:
        std::mt19937                            engine; // Mersenne Twister pseudo-random generator
    };

}
