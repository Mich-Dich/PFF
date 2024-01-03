
#include <filesystem>
#include "io_handler.h"
#include "config.h"

namespace PFF::config {

	//
	std::string configtype_to_string(config_file_types type) {

		static const std::unordered_map<config_file_types, std::string> typeStrings{
			{config_file_types::default_editor, "default_editor"},
			{config_file_types::default_engine, "default_engine"},
			{config_file_types::default_game, "default_game"},
			{config_file_types::default_input, "default_input"}
		};

		auto it = typeStrings.find(type);
		return (it != typeStrings.end()) ? it->second : "Unknown";
	}

	//
	void init() {

		std::string config_dir = "./config";
		io_handler::create_directory(config_dir);

		for (int i = static_cast<int>(config_file_types::default_editor); i <= static_cast<int>(config_file_types::default_input); ++i) {

			config_file_types type = static_cast<config_file_types>(i);
			std::string fileName = config_dir + "/" + configtype_to_string(type) + ".ini";

			std::ofstream config_file(fileName, std::ios::app);
			if (!config_file.is_open()) {

				std::cerr << "Failed to open/create config file: " << fileName << std::endl;
				return;
			}

			CORE_LOG(Trace, "Confirmed existence of config file: [" << fileName << "]");
			config_file.close();
		}
	}

}
