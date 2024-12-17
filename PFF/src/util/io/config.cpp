
#include "util/pffpch.h"

#include <filesystem>
#include <cctype> // for std::isspace
#include <iostream>
#include <fstream>
#include <string>

#include "io_handler.h"
#include "util/io/config.h"

#define REMOVE_WHITE_SPACE(line)                line.erase(std::remove_if(line.begin(), line.end(),                                         \
                                                [](char c) { return c == '\r' || c == '\n' || c == '\t'; }),                                \
                                                line.end());

#define BUILD_CONFIG_PATH(x)	CONFIG_DIR + config::file_type_to_string(x) + FILE_EXTENSION_CONFIG


namespace PFF::config {

    //
    void init(std::filesystem::path PFF_dir) {

        PFF_PROFILE_FUNCTION();

        io_handler::create_directory(PFF_dir / CONFIG_DIR);
        CORE_LOG(Trace, "Checking Engine config files at: " << PFF_dir / CONFIG_DIR);
        for (int i = 0; i <= static_cast<int>(file::input); ++i) {

            std::filesystem::path file_path = PFF_dir / CONFIG_DIR / (config::file_type_to_string(static_cast<file>(i)) + FILE_EXTENSION_CONFIG);
            std::ofstream config_file(file_path, std::ios::app);
            if (!config_file.is_open()) {

                CORE_LOG(Error, "Failed to open/create config file: [" << file_path << "]");
                return;
            }
            config_file.close();
        }
    }

    //
    void create_config_files_for_project(std::filesystem::path project_dir) {

        io_handler::create_directory(project_dir / CONFIG_DIR);
        CORE_LOG(Trace, "Checking project config files at: " << project_dir / CONFIG_DIR);
        for (int i = 0; i <= static_cast<int>(file::input); ++i) {

            std::filesystem::path file_path = project_dir / CONFIG_DIR / (config::file_type_to_string(static_cast<file>(i)) + FILE_EXTENSION_CONFIG);
            std::ofstream config_file(file_path, std::ios::app);
            if (!config_file.is_open()) {

                CORE_LOG(Error, "Failed to open/create config file: [" << file_path << "]");
                return;
            }
            config_file.close();
        }
    }

    /*
    template<typename T>
    void load(const file config_file, const std::string& section, const std::string& key, T& value) {

        access(config_file, section, key, value, false);
    }

    template<typename T>
    void save(const file config_file, const std::string& section, const std::string& key, T& value) {

        access(config_file, section, key, value, true);
    }
    */

    // 
    bool check_for_configuration(const file target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override) {

        PFF_PROFILE_FUNCTION();

        std::filesystem::path file_path = BUILD_CONFIG_PATH(target_config_file);
        std::ifstream configFile(file_path, std::ios::in | std::ios::binary);
        CORE_VALIDATE(configFile.is_open(), return false, "", "Fauled to open file: [" << file_path << "]");

        bool found_key = false;
        bool section_found = false;
        std::string line;
        std::ostringstream updatedConfig;
        while (std::getline(configFile, line)) {

            REMOVE_WHITE_SPACE(line);

            // Check if the line contains the desired section
            if (line.find("[" + section + "]") != std::string::npos) {

                section_found = true;
                updatedConfig << line << '\n';

                // Read and update the lines inside the section until a line with '[' is encountered
                while (std::getline(configFile, line) && (line.find('[') == std::string::npos)) {

                    REMOVE_WHITE_SPACE(line);

                    std::size_t found = line.find('=');
                    if (found != std::string::npos) {

                        std::string currentKey = line.substr(0, found);
                        if (currentKey == key) {

                            found_key = true;
                            if (override) {

                                // Update the value for the specified key
                                line.clear();
                                line = key + "=" + value;
                            } else {

                                value.clear();
                                value = line.substr(found +1);
                            }
                        }
                    }
                    updatedConfig << line << '\n';
                }

                if (!found_key) {

                    updatedConfig << key + "=" + value << '\n';
                    found_key = true;
                }

                if (!line.empty()) {

                    REMOVE_WHITE_SPACE(line);
                    updatedConfig << line << '\n';
                }
            }
                
            else {

                if(!line.empty())
                    if (line.find("[") == std::string::npos)
                        updatedConfig << line << '\n';
                    else
                        updatedConfig << line << '\n';
            }
        }


        // Close the original file
        configFile.close();

        if (!section_found || found_key || override) {

            // Open the file in truncation mode to clear its content
            std::ofstream outFile(file_path, std::ios::trunc);
            if (!outFile.is_open()) {
                CORE_LOG(Error, "problems opening file");
                return false;
            }

            // Write the updated content to the file
            outFile << updatedConfig.str();
            if (!section_found) {

                outFile << "[" << section << "]" << '\n';
                outFile << key << "=" << value << '\n';
            }

            outFile.close();
            // CORE_LOG(Trace, "File [" << file_path << "] updated with [" << std::setw(20) << std::left << section << " / " << std::setw(25) << std::left << key << "]: [" << value << "]");
        }
        return false; // Key not found
    }

    // ----------------------------------------------- file path resolution ----------------------------------------------- 

    //std::filesystem::path get_projerct_filepath_from_configtype(file type) { return util::get_executable_path() / CONFIG_DIR / (config::file_type_to_string(type) + FILE_EXTENSION_CONFIG); }

    //std::filesystem::path get_editor_filepath_from_configtype(file type) { return util::get_executable_path() / CONFIG_DIR / (config::file_type_to_string(type) + FILE_EXTENSION_CONFIG); }

    std::filesystem::path get_filepath_from_configtype(std::filesystem::path root, file type) { return root / CONFIG_DIR / (config::file_type_to_string(type) + FILE_EXTENSION_CONFIG); }

    std::filesystem::path get_filepath_from_configtype_ini(std::filesystem::path root, file type) { return root / CONFIG_DIR / (config::file_type_to_string(type) + FILE_EXTENSION_INI); }

    //
    std::string file_type_to_string(file type) {

        static const std::unordered_map<file, std::string> typeStrings{
            {file::ui, "ui" },
            {file::editor, "editor"},
            {file::engine, "engine"},
            {file::imgui, "imgui"},
            {file::game, "game"},
            {file::input, "input"},
        };

        auto it = typeStrings.find(type);
        return (it != typeStrings.end()) ? it->second : "unknown";
    }

}
