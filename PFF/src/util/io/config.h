#pragma once

#include <type_traits>

#include <glm/glm.hpp>

struct ImVec2;
struct ImVec4;

namespace PFF {

	namespace config {

        enum class PFF_API_EDITOR file {
			ui,
			editor,
			engine,
			game,
			input,
		};

        enum class PFF_API_EDITOR operation {
			save,
			load,
		};

		// @brief Initializes the configuration files by creating necessary directories and default files.
		// @param [config_dir] The directory where configuration files will be stored.
		// @return [void] This function does not have a return value.
        PFF_API_EDITOR void init();

		// @brief Converts a configuration file type enum to its corresponding string representation.
		// @param [type] The configuration file type to be converted.
		// @return [std::string] The string representation of the given configuration file type.
        PFF_API_EDITOR std::string file_type_to_string(file type);

		// @brief Checks for the existence of a configuration in the specified file, updates it if (found && override == true), and adds if not found.
		// @param [target_config_file] The type of configuration file to be checked/updated.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @param [value] Reference to the string where the value of the configuration will be stored or updated.
		// @param [override] If true, overrides the existing value with the provided one; if false, updates the value reference.
		// @return [bool] True if the configuration is found and updated, false otherwise.
        PFF_API_EDITOR bool check_for_configuration(const file target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override);


        template <typename T>
        void access(const file config_file, const std::string& section, const std::string& key, T& value, bool override) {

            std::string m_buffer = "";
            if constexpr (std::is_arithmetic_v<T>)
                m_buffer = std::to_string(value);

            else if constexpr (std::is_same_v<T, bool>)
                m_buffer = bool_to_str(value);

            else if constexpr (std::is_convertible_v<T, std::string>)
                m_buffer = value;

            else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

                std::ostringstream oss;
                oss << value.x << ' ' << value.y;
                m_buffer = oss.str();
            }

            else if constexpr (std::is_same_v<T, glm::vec3>) {

                std::ostringstream oss;
                oss << value.x << ' ' << value.y << ' ' << value.z;
                m_buffer = oss.str();
            }

            else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

                std::ostringstream oss;
                oss << value.x << ' ' << value.y << ' ' << value.z << ' ' << value.w;
                m_buffer = oss.str();
            }

            // Matrix of size 4
            else if constexpr (std::is_same_v<T, glm::mat4>) {

                std::ostringstream oss;
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        oss << value[i][j];
                        if (i != 3 || j != 3) // Not the last element
                            oss << ' ';
                    }
                }
                m_buffer = oss.str();
            }


            else
                DEBUG_BREAK();		// Input value is not supported

            check_for_configuration(config_file, section, key, m_buffer, override);

            if constexpr (std::is_arithmetic_v<T>)
                value = util::str_to_num<T>(m_buffer);

            else if constexpr (std::is_same_v<T, bool>)
                value = util::str_to_bool(m_buffer);

            else if constexpr (std::is_convertible_v<T, std::string>)
                value = m_buffer;

            else if constexpr (std::is_same_v<T, glm::vec2> || std::is_same_v<T, ImVec2>) {

                std::istringstream iss(m_buffer);
                iss >> value.x >> value.y;
            }

            else if constexpr (std::is_same_v<T, glm::vec3>) {

                std::istringstream iss(m_buffer);
                iss >> value.x >> value.y >> value.z;
            }

            else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

                std::istringstream iss(m_buffer);
                iss >> value.x >> value.y >> value.z >> value.w;
            }

            // Matrix of size 4         TODO: move into seperate template for all matrixes
            else if constexpr (std::is_same_v<T, glm::mat4>) {
                std::istringstream iss(m_buffer);
                for (int i = 0; i < 4; ++i) {
                    for (int j = 0; j < 4; ++j) {
                        iss >> value[i][j];
                    }
                }
            }


            else
                DEBUG_BREAK();		// Input value is not supported

        }

		// @brief load a value from specified config file (save if not found)
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the variable that will store the configuration value.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be loaded.
		// @see check_for_configuration
		template <typename T>
        void load(const file config_file, const std::string& section, const std::string& key, T& value) {

            access(config_file, section, key, value, false);
        }

		// @brief save a value from specified config file (create if not found)
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the variable that will be stored in the configuration falie.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be loaded.
		// @see check_for_configuration
		template <typename T>
        void save(const file config_file, const std::string& section, const std::string& key, T& value) {

            access(config_file, section, key, value, true);
        }

    }

}

// PFF_API
// PFF_API_EDITOR
