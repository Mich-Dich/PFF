#pragma once

#include <type_traits>
#include <glm/glm.hpp>


struct ImVec2;
struct ImVec4;

namespace PFF::config {

    enum class PFF_API_EDITOR file : u8 {
		ui,
		editor,
		engine,
		imgui,
		game,
		input,
	};

    enum class PFF_API_EDITOR operation : u8 {
		save,
		load,
	};

	// @brief Initializes the configuration files by creating necessary directories and default files.
	// @param [config_dir] The directory where configuration files will be stored.
	// @return [void] This function does not have a return value.
    PFF_API_EDITOR void init(std::filesystem::path PFF_dir);

	PFF_API_EDITOR void create_config_files_for_project(std::filesystem::path project_dir);

	PFF_API_EDITOR FORCEINLINE std::string file_type_to_string(file type);

	//PFF_API_EDITOR std::filesystem::path get_filepath_from_configtype(file type);

	PFF_API_EDITOR std::filesystem::path get_filepath_from_configtype(std::filesystem::path root, file type);

	PFF_API_EDITOR std::filesystem::path get_filepath_from_configtype_ini(std::filesystem::path root, file type);

	// @brief Checks for the existence of a configuration in the specified file, updates it if (found && override == true), and adds if not found.
	// @param [target_config_file] The type of configuration file to be checked/updated.
	// @param [section] The section within the configuration file where the key-value pair is located.
	// @param [key] The key of the configuration to be checked/updated.
	// @param [value] Reference to the string where the value of the configuration will be stored or updated.
	// @param [override] If true, overrides the existing value with the provided one; if false, updates the value reference.
	// @return [bool] True if the configuration is found and updated, false otherwise.
    PFF_API_EDITOR bool check_for_configuration(const file target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override);


}
