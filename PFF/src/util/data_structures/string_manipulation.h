#pragma once

#include <imgui.h>

#include "util/data_structures/UUID.h"

namespace PFF::util {


    // @brief Searches for the last occurrence of the specified delimiter in the input string,
    //          and if found, extracts the substring after the delimiter into the 'dest' string.
    //          If the delimiter is not found, the 'dest' string remains unchanged.
    // @param [dest] Reference to a string where the extracted part will be stored.
    // @param [input] The input string from which the part is to be extracted.
    // @param [delimiter] The character delimiter used to identify the part to extract.
    // @return None
    void extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter);

    // @brief Searches for the last occurrence of the specified delimiter in the input string,
    //          and if found, extracts the substring before the delimiter into the 'dest' string.
    //          If the delimiter is not found, the 'dest' string remains unchanged.
    // @param [dest] Reference to a string where the extracted part will be stored.
    // @param [input] The input string from which the part is to be extracted.
    // @param [delimiter] The character delimiter used to identify the part to extract.
    // @return None
    void extract_part_befor_delimiter(std::string& dest, const std::string& input, const char* delimiter);

    // @brief Given a string representing a variable access chain (e.g., "object1->object2.variable"),
    //          this function extracts and returns the name of the variable ("variable" in this example).
    //          The extraction process considers both "->" and "." as delimiters for nested access.
    // @param [input] The input string representing the variable access chain.
    // @return A string containing the name of the variable extracted from the input string.
    std::string extract_vaiable_name(const std::string& input);

    //@brief Converts a string to a boolean value.
    //@param [string] The string to convert.
    //@return true if the string is "true", false otherwise.
    FORCEINLINE constexpr bool str_to_bool(const std::string& string) { return(string == "true") ? true : false; }

    //@brief Converts a boolean value to a string.
    //@param [boolean] The boolean value to convert.
    //@return [conat char*] "true" if the boolean value is true, "false" otherwise.
    FORCEINLINE constexpr const char* bool_to_str(bool boolean) { return boolean ? "true" : "false"; }

    std::string add_spaces(const u32 multiple_of_indenting_spaces, u32 num_of_indenting_spaces = 2);

    u32 measure_indentation(const std::string& str, u32 num_of_indenting_spaces = 2);

    int count_lines(const char* text);

#define EXTRACT_AFTER_PFF(path) ([](const std::string& str) -> std::string {    \
            const std::string delimiter = "PFF";                                        \
            size_t pos = str.find(delimiter);                                           \
            if (pos != std::string::npos) {                                             \
                pos = str.find(delimiter, pos + delimiter.length());                    \
                if (pos != std::string::npos) {                                         \
                    return str.substr(pos + delimiter.length() + 1);                    \
                }                                                                       \
            }                                                                           \
            return "";                                                                  \
        }(path))


    // @brief Removes a substring from a character array.
    // @tparam N The size of the source character array.
    // @tparam K The size of the remove character array.
    // @param source The source character array.
    // @param remove The substring to remove.
    // @return A character array with the specified substring removed.
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
    void convert_typename_to_string(std::string& typename_string) {

#if defined(_MSC_VER)                               // MSVC specific code

        typename_string = typeid(T).name();
        // Remove "class " or "struct " prefix if present
        const char* class_prefix = "class ";
        const char* struct_prefix = "struct ";
        if (typename_string.find(class_prefix) == 0)
            typename_string = typename_string.substr(strlen(class_prefix));
        else if (typename_string.find(struct_prefix) == 0)
            typename_string = typename_string.substr(strlen(struct_prefix));

#elif defined(__GNUC__) || defined(__clang__)       // GCC and Clang specific code

        const char* typeName = typeid(T).name();
        int status;
        char* demangled = abi::__cxa_demangle(typeName, nullptr, nullptr, &status);
        if (status == 0) {
            typename_string = demangled;
            free(demangled);
        } else
            typename_string = typeName;
        
#else
        // Generic fallback
        typename_string = typeid(T).name();
#endif

        // Remove namespace qualifiers if present
        size_t pos = typename_string.find_last_of("::");
        if (pos != std::string::npos)
            typename_string = typename_string.substr(pos + 1);
    }

    // Variadic template function to handle multiple arguments
    template<typename... Args>
    std::string format_string(Args&&... args) {
        std::ostringstream oss;
        (oss << ... << std::forward<Args>(args));
        return oss.str();
    }

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

        else if constexpr (std::is_same_v<T, PFF::version>) {

            std::ostringstream oss;
            oss << src_value.major << ' ' << src_value.minor << ' ' << src_value.patch;
            dest_string = oss.str();
            return;
        }

        else if constexpr (std::is_same_v<T, PFF::system_time>) {

            std::ostringstream oss;
            oss << (u16)src_value.year
                << ' ' << (u16)src_value.month
                << ' ' << (u16)src_value.day
                << ' ' << (u16)src_value.day_of_week
                << ' ' << (u16)src_value.hour
                << ' ' << (u16)src_value.minute
                << ' ' << (u16)src_value.secund
                << ' ' << (u16)src_value.millisecend;
            dest_string = oss.str();
            return;
        }

        else if constexpr (std::is_same_v<T, std::filesystem::path>) {

            dest_string = src_value.string();
            return;
        }

        else if constexpr (std::is_same_v<T, PFF::UUID>) {

            dest_string = std::to_string((u64)src_value);
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
            oss << std::fixed << std::setprecision(4)
                << src_value.x << ' ' << src_value.y << ' ' << src_value.z << ' ' << src_value.w;
            dest_string = oss.str();
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

        else if constexpr (std::is_arithmetic_v<T>) {

            dest_string = std::to_string(src_value);
            return;
        }

        else if constexpr (std::is_convertible_v<T, std::string>) {

            //LOG(Fatal, "called: convert_to_string() with string");
            dest_string = src_value;
            //std::replace(dest_string.begin(), dest_string.end(), ' ', '%');
            std::replace(dest_string.begin(), dest_string.end(), '\n', '$');
            return;
        }

        else if constexpr (std::is_enum_v<T>) {

            dest_string = std::to_string(static_cast<std::underlying_type_t<T>>(src_value));
            return;
        }


        else
            DEBUG_BREAK();		// Input value is not supported
    }

    template<typename T>
    constexpr std::string to_string(const T& src_value) {

        std::string dest_string;

        convert_to_string<T>(src_value, dest_string);
        return dest_string;
    }


    // @brief Converts a string representation to a value of type T.
    // @brief Can handle conversion into various types such as: arithmetic types, boolean, glm::vec2, glm::vec3, glm::vec4, ImVec2, ImVec4, and glm::mat4.
    // @brief If the input value type is not supported, a DEBUG_BREAK() is triggered.
    // @param [string] The string to be converted.
    // @param [value] Reference to the variable that will store the converted value.
    // @tparam T The type of the value [string] should be converted to.
    template<typename T>
    constexpr void convert_from_string(const std::string& src_string, T& dest_value) {

        if constexpr (std::is_same_v<T, bool>) {

            dest_value = PFF::util::str_to_bool(src_string);
            return;
        }

        else if constexpr (std::is_same_v<T, version>) {

            std::istringstream iss(src_string);
            iss >> dest_value.major >> dest_value.minor >> dest_value.patch;
            return;
        }

        else if constexpr (std::is_same_v<T, PFF::system_time>) {

            std::istringstream iss(src_string);
            iss >> dest_value.year
                >> dest_value.month
                >> dest_value.day
                >> dest_value.day_of_week
                >> dest_value.hour
                >> dest_value.minute
                >> dest_value.secund
                >> dest_value.millisecend;
            return;
        }

        else if constexpr (std::is_same_v<T, std::filesystem::path>) {

            dest_value = src_string;
            return;
        }

        else if constexpr (std::is_same_v<T, PFF::UUID>) {

            dest_value = util::str_to_num<u64>(src_string);
            return;
        }

        else if constexpr (std::is_same_v<T, const char*>) {

            std::string temp_str = src_string;
            std::replace(temp_str.begin(), temp_str.end(), '$', '\n');
            dest_value = temp_str.c_str();
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

        else if constexpr (std::is_same_v<T, glm::mat4> || std::is_same_v<T, glm::mat3>) {

            int loc_size = 4;
            if constexpr (std::is_same_v<T, glm::mat3>)
                loc_size = 3;

            std::istringstream iss(src_string);
            for (int i = 0; i < loc_size; ++i) {
                for (int j = 0; j < loc_size; ++j) {
                    iss >> dest_value[i][j];
                }
            }
            return;
        }

        else if constexpr (std::is_arithmetic_v<T>) {

            dest_value = util::str_to_num<T>(src_string);
            return;
        }

        else if constexpr (std::is_convertible_v<T, std::string>) {

            dest_value = src_string;                    // <= HERE
            std::replace(dest_value.begin(), dest_value.end(), '$', '\n');
            return;
        }

        else if constexpr (std::is_enum_v<T>) {

            dest_value = static_cast<T>(std::stoi(src_string));
            return;
        }

        else
            DEBUG_BREAK();		// Input value is not supported
    }

    template<typename T>
    constexpr T from_string(const std::string& src_string) {

        T dest_value;
        convert_from_string<T>(src_string, dest_value);
        return dest_value;
    }

}
