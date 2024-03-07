#pragma once

// =================================================================================  includes  ==================================================================================

#include <type_traits> // For std::is_vector

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

    // =================================================  smart pointers  =================================================

    template<typename T>
    // @brief [ref] is a smart pointer to a resource [T]
    using ref = std::shared_ptr<T>;

    template<typename T, typename ... args>
    constexpr ref<T> create_ref(args&& ... arguments) {

        return std::make_shared<T>(std::forward<args>(arguments)...);
    }

    template<typename T>
    // @brief [scope_ref] is a [ref] for a scope
    using scope_ref = std::unique_ptr<T>;

    template<typename T, typename ... args>
    constexpr scope_ref<T> create_scoped_ref(args&& ... arguments) {

        return std::make_unique<T>(std::forward<args>(arguments)...);
    }
    
    // =================================================  utilitys =================================================

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

        template<typename T>
        struct is_vector : std::false_type {};

        template<typename T, typename Alloc>
        struct is_vector<std::vector<T, Alloc>> : std::true_type {};


        // @brief Converts a value of type T to its string representation.
        // @brief Can handle conversion from various types such as: arithmetic types, boolean, glm::vec2, glm::vec3, glm::vec4, ImVec2, ImVec4, and glm::mat4
        // @brief If the input value type is not supported, a DEBUG_BREAK() is triggered.
        // @param [value] The value to be converted.
        // @tparam T The type of the value to be converted.
        // @return A string representing the input value.
        template<typename T>
        constexpr void convert_to_string(const T& src_value, std::string& dest_string) {

            if constexpr (std::is_same_v<T, bool>) {

                dest_string = bool_to_str(src_value);
                return;
            }

            else if constexpr (std::is_arithmetic_v<T>) {

                dest_string = std::to_string(src_value);
                return;
            }

            else if constexpr (std::is_convertible_v<T, std::string>) {

                //LOG(Fatal, "called: convert_to_string() with string");
                dest_string = src_value;
                std::replace(dest_string.begin(), dest_string.end(), ' ', '_');
                return;
            }

            else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

                std::ostringstream oss;
                oss << src_value.x << ' ' << src_value.y;
                dest_string = oss.str();
                return;
            }

            else if constexpr (std::is_same_v<T, glm::vec3>) {

                std::ostringstream oss;
                oss << src_value.x << ' ' << src_value.y << ' ' << src_value.z;
                dest_string = oss.str();
                return;
            }

            else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

                std::ostringstream oss;
                oss << src_value.x << ' ' << src_value.y << ' ' << src_value.z << ' ' << src_value.w;
                dest_string = oss.str();
                return;
            }

            else if constexpr (std::is_enum_v<T>) {

                dest_string = std::to_string(static_cast<std::underlying_type_t<T>>(src_value));
                return;
            }

            // Matrix of size 4         TODO: move into seperate template for all matrixes
            else if constexpr (std::is_same_v<T, glm::mat4>) {

                std::ostringstream oss;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        oss << src_value[i][j];
                        if (i != 3 || j != 3) // Not the last element
                            oss << ' ';
                    }
                }
                dest_string = oss.str();
                return;
            }

            else
                DEBUG_BREAK();		// Input value is not supported
        }


        template<typename T>
        // @brief Converts a string representation to a value of type T.
        // @brief Can handle conversion into various types such as: arithmetic types, boolean, glm::vec2, glm::vec3, glm::vec4, ImVec2, ImVec4, and glm::mat4.
        // @brief If the input value type is not supported, a DEBUG_BREAK() is triggered.
        // @param [string] The string to be converted.
        // @param [value] Reference to the variable that will store the converted value.
        // @tparam T The type of the value [string] should be converted to.
        constexpr void convert_from_string(const std::string& src_string, T& dest_value) {

            if constexpr (std::is_same_v<T, bool>) {

                dest_value = util::str_to_bool(src_string);
                return;
            }

            else if constexpr (std::is_arithmetic_v<T>) {

                dest_value = util::str_to_num<T>(src_string);
                return;
            }

            else if constexpr (std::is_convertible_v<T, std::string>) {

                dest_value = src_string;
                std::replace(dest_value.begin(), dest_value.end(), '_', ' ');
                return;
            }

            else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

                std::istringstream iss(src_string);
                iss >> dest_value.x >> dest_value.y;
                return;
            }

            else if constexpr (std::is_same_v<T, glm::vec3>) {

                std::istringstream iss(src_string);
                iss >> dest_value.x >> dest_value.y >> dest_value.z;
                return;
            }

            else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

                std::istringstream iss(src_string);
                iss >> dest_value.x >> dest_value.y >> dest_value.z >> dest_value.w;
                return;
            }

            else if constexpr (std::is_enum_v<T>) {

                dest_value = static_cast<T>(std::stoi(src_string));
                return;
            }

            // Matrix of size 4         TODO: move into seperate template for all matrixes
            else if constexpr (std::is_same_v<T, glm::mat4>) {
                std::istringstream iss(src_string);
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        iss >> dest_value[i][j];
                    }
                }
                return;
            }


            else
                DEBUG_BREAK();		// Input value is not supported
        }

		template <typename T, typename... Rest>
        // @brief Combines hash values. from: https://stackoverflow.com/a/57595105
        // @tparam T The type of the value to hash.
        // @tparam Rest Additional types to hash.
        // @param seed The seed value for the hash.
        // @param v The value to hash.
        // @param rest Additional values to hash.
        constexpr void hash_combine(std::size_t& seed, const T& v, const Rest&... rest) {

			seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
			(hash_combine(seed, rest), ...);
		}

		template <size_t N, size_t K>
        // @brief Removes a substring from a character array.
        // @tparam N The size of the source character array.
        // @tparam K The size of the remove character array.
        // @param source The source character array.
        // @param remove The substring to remove.
        // @return A character array with the specified substring removed.
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
        
        //@brief Checks the validity of a pointer.
        //@param [ptr] Pointer to check.
        //@return [conat char*] "valid" if the pointer is not null, "invalid" otherwise.
		FORCEINLINE constexpr const char* ptr_validity_check(const void* ptr) {return (ptr != nullptr) ? "valid" : "invalid";}
        
        //@brief Converts a string to a boolean value.
        //@param [string] The string to convert.
        //@return true if the string is "true", false otherwise.
		FORCEINLINE constexpr bool str_to_bool(const std::string& string) { return(string == "true") ? true : false; }
                
        //@brief Converts a boolean value to a string.
        //@param [boolean] The boolean value to convert.
        //@return [conat char*] "true" if the boolean value is true, "false" otherwise.
		FORCEINLINE constexpr const char* bool_to_str(bool boolean) { return boolean ? "true" : "false"; }
	}
}
