#pragma once

// =================================================================================  includes  ==================================================================================

#include "util/logger.h"
#include "util/io/io_handler.h"
#include "util/io/config.h"
#include "util/data_types.h"

// =================================================================================  macros  ==================================================================================

#define ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

#define APP_NAMESPACE PFF

#define BIT(x) (1 << x)

#define BIND_FN(x)							std::bind(&x, this, std::placeholders::_1)

#define DELETE_COPY(type)					type(const type&) = delete;								\
											type& operator=(const type&) = delete

#define DELETE_MOVE(type)					type(type&&) = delete;									\
											type& operator=(type&&) = delete

#define DELETE_COPY_MOVE(type)				type(const type&) = delete;								\
											type& operator=(const type&) = delete;					\
											type(type&&) = delete;									\
											type& operator=(type&&) = delete

#define CONSOLE_LIST_BEGIN					" " << (char)(200) << " "

// ================================================================================  functions  ================================================================================

namespace PFF {

	namespace util {

		template <size_t N>
		struct char_array {
			char data[N];
		};

		template <typename T>
		T str_to_num(const std::string& str) {
			std::istringstream ss(str);
			T num{};
			ss >> num;
			return num;
		}

        template <typename T>
        std::string num_to_str(const T& num) {
            std::ostringstream oss;
            oss << num;
            return oss.str();
        }

        // @brief Converts a value of type T to its string representation.
        // @brief Can handle conversion from various types such as: arithmetic types, boolean, glm::vec2, glm::vec3, glm::vec4, ImVec2, ImVec4, and glm::mat4
        // @brief If the input value type is not supported, a DEBUG_BREAK() is triggered.
        // 
        // @param [value] The value to be converted.
        // @tparam T The type of the value to be converted.
        // @return A string representing the input value.
        template<typename T>
        constexpr void convert_to_string(std::string& string, T& value) {

            if constexpr (std::is_arithmetic_v<T>)
                string = std::to_string(value);

            else if constexpr (std::is_same_v<T, bool>)
                string = bool_to_str(value);

            else if constexpr (std::is_convertible_v<T, std::string>)
                string = value;

            else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

                std::ostringstream oss;
                oss << value.x << ' ' << value.y;
                string = oss.str();
            }

            else if constexpr (std::is_same_v<T, glm::vec3>) {

                std::ostringstream oss;
                oss << value.x << ' ' << value.y << ' ' << value.z;
                string = oss.str();
            }

            else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

                std::ostringstream oss;
                oss << value.x << ' ' << value.y << ' ' << value.z << ' ' << value.w;
                string = oss.str();
            }

            else if constexpr (std::is_enum_v<T>)
                string = std::to_string(static_cast<std::underlying_type_t<T>>(value));

            // Matrix of size 4         TODO: move into seperate template for all matrixes
            else if constexpr (std::is_same_v<T, glm::mat4>) {

                std::ostringstream oss;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        oss << value[i][j];
                        if (i != 3 || j != 3) // Not the last element
                            oss << ' ';
                    }
                }
                string = oss.str();
            }

            else
                DEBUG_BREAK();		// Input value is not supported
        }


        // @brief Converts a string representation to a value of type T.
        // @brief Can handle conversion into various types such as: arithmetic types, boolean, glm::vec2, glm::vec3, glm::vec4, ImVec2, ImVec4, and glm::mat4.
        // @brief If the input value type is not supported, a DEBUG_BREAK() is triggered.
        // 
        // @param [string] The string to be converted.
        // @param [value] Reference to the variable that will store the converted value.
        // @tparam T The type of the value [string] should be converted to.
        template<typename T>
        constexpr void convert_from_string(const std::string& string, T& value) {

            if constexpr (std::is_arithmetic_v<T>)
                value = util::str_to_num<T>(string);

            else if constexpr (std::is_same_v<T, bool>)
                value = util::str_to_bool(string);

            else if constexpr (std::is_convertible_v<T, std::string>)
                value = string;

            else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

                std::istringstream iss(string);
                iss >> value.x >> value.y;
            }

            else if constexpr (std::is_same_v<T, glm::vec3>) {

                std::istringstream iss(string);
                iss >> value.x >> value.y >> value.z;
            }

            else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

                std::istringstream iss(string);
                iss >> value.x >> value.y >> value.z >> value.w;
            }

            else if constexpr (std::is_enum_v<T>)
                value = static_cast<T>(std::stoi(string));

            // Matrix of size 4         TODO: move into seperate template for all matrixes
            else if constexpr (std::is_same_v<T, glm::mat4>) {
                std::istringstream iss(string);
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        iss >> value[i][j];
                    }
                }
            }


            else
                DEBUG_BREAK();		// Input value is not supported
        }



		// from: https://stackoverflow.com/a/57595105
		template <typename T, typename... Rest>
		void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {
			seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			(hash_combine(seed, rest), ...);
		}

		template <size_t N, size_t K>
		constexpr auto remove_substring(const char(&source)[N], const char(&remove)[K]) {

			char_array<N> result = {};
			size_t srcIndex = 0;
			size_t dstIndex = 0;
			while (srcIndex < N) {

				size_t matchIndex = 0;
				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && source[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;

				if (matchIndex == K - 1)
					srcIndex += matchIndex;

				result.data[dstIndex++] = source[srcIndex] == '"' ? '\'' : source[srcIndex];
				srcIndex++;
			}
			return result;
		}

		FORCEINLINE constexpr const char* ptr_validity_check(const void* ptr) {return (ptr != nullptr) ? "valid" : "invalid";}

		FORCEINLINE constexpr bool str_to_bool(const std::string& string) { return(string == "true") ? true : false; }
		FORCEINLINE constexpr const char* bool_to_str(bool boolean) { return boolean ? "true" : "false"; }
	}
}
