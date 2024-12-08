#pragma once

#include "util/pffpch.h"
#include "util/UUID.h"

namespace PFF {

	struct project_data {

		FORCEINLINE bool is_project_path_valid() { return (!project_path.empty() && std::filesystem::exists(project_path) && project_path.extension() == PFF_PROJECT_EXTENTION); }

		std::filesystem::path		project_path;			// system path to the first world when developing the project
		// meta_data

		std::string					display_name{};				// this name will be used in the launcher and editor
		std::string					name{};						// this name is the name of the solution & export-folder
		
		UUID						ID;
		version						engine_version{};			// version of the engine used, used by launcher, engine, editor, ...
		version						project_version{};			// version of this project, mostly uesfull to the user for version managemant
	
		// paths
		std::filesystem::path		build_path;					// system path for exported builds
		std::filesystem::path		start_world;				// system path to the first world when executoing project
		std::filesystem::path		editor_start_world;			// system path to the first world when developing the project

		// Dependencies
		//std::vector<std::string> engine_plugins;				// List of enabled or required plugins
		//std::vector<std::string> external_libraries;			// List of external libraries

		// User data
		system_time					last_modified;				// Timestamp of when the project was last modified.
		std::string					description;				// A short description of this project
		std::vector<std::string>	tags{};						// Tags or categories

	};

}
