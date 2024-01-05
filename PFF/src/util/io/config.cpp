
#include "util/pffpch.h"

#include "io_handler.h"
#include "util/io/config.h"

#include <filesystem>
#include <cctype> // for std::isspace

#include <iostream>
#include <fstream>
#include <string>

#define BUILD_CONFIG_PATH(x)	config_dir + "/" + configtype_to_string(x) + FILE_EXTENSION_CONFIG

namespace PFF {

    namespace config {

        static std::string config_dir = "./config";

        //
        void init() {

            io_handler::create_directory(config_dir);

            for (int i = static_cast<int>(config_file_types::default_editor); i <= static_cast<int>(config_file_types::default_input); ++i) {

                std::filesystem::path file_path = BUILD_CONFIG_PATH(static_cast<config_file_types>(i));
                std::ofstream config_file(file_path, std::ios::app);
                if (!config_file.is_open()) {

                    CORE_LOG(Error, "Failed to open/create config file: [" << file_path << "]");
                    return;
                }

                CORE_LOG(Trace, "Confirmed existence of config file: [" << file_path << "]");
                config_file.close();
            }
        }

        // 
        bool check_for_configuration(const config_file_types target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override) {

            std::filesystem::path file_path = BUILD_CONFIG_PATH(target_config_file);
            std::ifstream configFile(file_path, std::ios::in | std::ios::binary);
            CORE_VALIDATE(configFile.is_open(), "", "Fauled to open file: [" << file_path << "]", return false);

            bool found_key = false;
            bool section_found = false;
            std::string line;
            std::ostringstream updatedConfig;
            while (std::getline(configFile, line)) {

                line.erase(std::remove_if(line.begin(), line.end(),
                    [](char c) { return c == '\r' || c == '\n'; }),
                    line.end());

                // Check if the line contains the desired section
                if (line.find("[" + section + "]") != std::string::npos) {

                    section_found = true;
                    updatedConfig << line << '\n';

                    // Read and update the lines inside the section until a line with '[' is encountered
                    while (std::getline(configFile, line) && (line.find('[') == std::string::npos)) {

                        line.erase(std::remove_if(line.begin(), line.end(),
                            [](char c) { return c == '\r' || c == '\n' || c == '\t'; }),
                            line.end());

                        std::size_t found = line.find('=');
                        if (found != std::string::npos) {

                            std::string currentKey = line.substr(0, found);
                            if (currentKey == key) {

                                found_key = true;
                                if (override) {

                                    // Update the value for the specified key
                                    line.clear();
                                    line = '\t' + key + "=" + value;
                                } else {

                                    value.clear();
                                    value = line.substr(found +1);
                                }
                            } else 
                                line = '\t' + line;
                        }
                        updatedConfig << line << '\n';
                    }
                }
                
                else {
                    updatedConfig << line << '\n';
                }
            }


            // Close the original file
            configFile.close();

            if (!section_found || !found_key || override) {

                // Open the file in truncation mode to clear its content
                std::ofstream outFile(file_path, std::ios::trunc);
                if (!outFile.is_open()) {
                    CORE_LOG(Error, "problems opening file");
                    return false;
                }

                // Write the updated content to the file
                outFile << updatedConfig.str();
                if (!section_found) 
                    outFile << "[" << section << "]" << '\n';

                if (!found_key) 
                    outFile << '\t' << key << "=" << value << '\n';

                outFile.close();
                CORE_LOG(Debug, "File [" << file_path << "] updated with [" << section << "] - [" << key << "][" << value << "]");
            }
            return false; // Key not found
        }

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

    }

}
