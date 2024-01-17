#pragma once

#include <type_traits>

struct ImVec4;

namespace PFF {

#define BUILD_CONFIG_PATH(x)	CONFIG_DIR + config::file_type_to_string(x) + FILE_EXTENSION_CONFIG
	
	namespace config {


		enum class file {
			ui,
			editor,
			engine,
			game,
			input,
		};

		enum class operation {
			save,
			load,
		};

		// @brief Initializes the configuration files by creating necessary directories and default files.
		// @param [config_dir] The directory where configuration files will be stored.
		// @return [void] This function does not have a return value.
		void init();

		// @brief Converts a configuration file type enum to its corresponding string representation.
		// @param [type] The configuration file type to be converted.
		// @return [std::string] The string representation of the given configuration file type.
		std::string file_type_to_string(file type);

		// @brief Checks for the existence of a configuration in the specified file, updates it if (found && override == true), and adds if not found.
		// @param [target_config_file] The type of configuration file to be checked/updated.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @param [value] Reference to the string where the value of the configuration will be stored or updated.
		// @param [override] If true, overrides the existing value with the provided one; if false, updates the value reference.
		// @return [bool] True if the configuration is found and updated, false otherwise.
		bool check_for_configuration(const file target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override);


		template <typename T>
		void access(const file config_file, const std::string& section, const std::string& key, T& value, bool override) {

			std::string buffer = "";
			if constexpr (std::is_arithmetic_v<T>)
				buffer = std::to_string(value);
			
			else if constexpr (std::is_same_v<T, bool>)
				buffer = bool_to_str(value);

			else if constexpr (std::is_convertible_v<T, std::string>)
				buffer = value;

			else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

				std::ostringstream oss;
				oss << value.x << ' ' << value.y << ' ' << value.z << ' ' << value.w;
				buffer = oss.str();
			}

			else
				CORE_ASSERT(0 == 1, "", "Input value is not supported");

			PFF::config::check_for_configuration(config_file, section, key, buffer, false);

			if constexpr (std::is_arithmetic_v<T>)
				value = str_to_num<T>(buffer);

			else if constexpr (std::is_same_v<T, bool>)
				value = str_to_bool(buffer);

			else if constexpr (std::is_convertible_v<T, std::string>)
				value = buffer;

			else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>) {

				std::istringstream iss(buffer);
				iss >> value.x >> value.y >> value.z >> value.w;
			}

			else
				CORE_ASSERT(0 == 1, "", "Input value is not supported");

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
