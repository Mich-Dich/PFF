#pragma once



namespace PFF {

#define BUILD_CONFIG_PATH(x)	CONFIG_DIR + config::file_type_to_string(x) + FILE_EXTENSION_CONFIG
	
	namespace config {


		enum class file_types {
			ui,
			editor,
			engine,
			game,
			input,
		};

		// @brief Initializes the configuration files by creating necessary directories and default files.
		// @param [config_dir] The directory where configuration files will be stored.
		// @return [void] This function does not have a return value.
		void init();



		// @brief Converts a configuration file type enum to its corresponding string representation.
		// @param [type] The configuration file type to be converted.
		// @return [std::string] The string representation of the given configuration file type.
		std::string file_type_to_string(file_types type);


		// @brief Checks for the existence of a configuration in the specified file, updates it if (found && override == true), and adds if not found.
		// @param [target_config_file] The type of configuration file to be checked/updated.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @param [value] Reference to the string where the value of the configuration will be stored or updated.
		// @param [override] If true, overrides the existing value with the provided one; if false, updates the value reference.
		// @return [bool] True if the configuration is found and updated, false otherwise.
		bool check_for_configuration(const file_types target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override);


		// ======================================================================= LOAD CONFIG =======================================================================
		

		// @brief Macro to check for the existence of a numerical configuration in the specified file, update it if found, and convert it to the desired type.
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the numerical variable that will store the configuration value.
		// @param [type] The data type of the numerical configuration.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @see check_for_configuration
		#define LOAD_CONFIG_NUM(config_file, value, type, section, key)																										\
														{std::string buffer = num_to_str(value);																			\
														PFF::config::check_for_configuration(PFF::config::file_types::config_file, section, key, buffer, false);		\
														value = str_to_num<type>(buffer);}

		// @brief Macro to load a string value from specified config file (save if not found)
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the string variable that will store the configuration value.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be loaded.
		// @see check_for_configuration
		#define LOAD_CONFIG_STR(config_file, value, section, key)																											\
														{std::string buffer = (value);																						\
														PFF::config::check_for_configuration(PFF::config::file_types::config_file, section, key, buffer, false);		\
														value = (buffer);}

		// @brief Macro to check for the existence of a boolean configuration in the specified file, update it if found, and convert it to the bool type.
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the boolean variable that will store the configuration value.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @see check_for_configuration
		#define LOAD_CONFIG_BOOL(config_file, value, section, key)																											\
														{std::string buffer = bool_to_str(value);																			\
														PFF::config::check_for_configuration(PFF::config::file_types::config_file, section, key, buffer, false);		\
														value = str_to_bool(buffer);}

		
		// ====================================================================================== SAVE CONFIG ======================================================================================


		// @brief Macro to check for the existence of a numerical configuration in the specified file, update it if found, and convert it to the desired type.
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the numerical variable that will store the configuration value.
		// @param [type] The data type of the numerical configuration.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @see check_for_configuration
		#define SAVE_CONFIG_NUM(config_file, value, type, section, key)																										\
														{std::string buffer = num_to_str(value);																			\
														PFF::config::check_for_configuration(PFF::config::file_types::config_file, section, key, buffer, true);		\
														value = str_to_num<type>(buffer);}

		// @brief Macro to save a string value in the specified file (update if found)
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the string variable that will store the configuration value.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be saved.
		// @see check_for_configuration
		#define SAVE_CONFIG_STR(config_file, value, section, key)																											\
														{std::string buffer = (value);																						\
														PFF::config::check_for_configuration(PFF::config::file_types::config_file, section, key, buffer, true);		\
														value = (buffer);}

		// @brief Macro to check for the existence of a boolean configuration in the specified file, update it if found, and convert it to the bool type.
		// @param [config_file] The type of configuration file to be checked/updated.
		// @param [value] Reference to the boolean variable that will store the configuration value.
		// @param [section] The section within the configuration file where the key-value pair is located.
		// @param [key] The key of the configuration to be checked/updated.
		// @see check_for_configuration
		#define SAVE_CONFIG_BOOL(config_file, value, section, key)																											\
														{std::string buffer = bool_to_str(value);																			\
														PFF::config::check_for_configuration(PFF::config::file_types::config_file, section, key, buffer, true);		\
														value = str_to_bool(buffer);}

	}

}
