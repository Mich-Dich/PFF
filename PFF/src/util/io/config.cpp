
#include "io_handler.h"
#include "util/io/config.h"

#include <filesystem>
#include <cctype> // for std::isspace

#include <iostream>
#include <fstream>
#include <string>

#define BUILD_CONFIG_PATH(x)	config_dir + "/" + configtype_to_string(x) + ".ini"


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
        bool try_to_find(const config_file_types target_config_file, const std::string& section, const std::string& key, std::string& value) {

            CORE_LOG(Fatal, "DO NOT USE");
            std::filesystem::path file_path = BUILD_CONFIG_PATH(target_config_file);
            std::ifstream configFile(file_path, std::ios::in | std::ios::binary);
            CORE_VALIDATE(configFile.is_open(), "", "Fauled to open file: [" << file_path << "]", return false);

            bool found_key = false;

            // find correct section
            parser loc_parser;
            line_type loc_type;
            std::string line;
            std::string buffer = "";
            bool found_section = false;
            while (std::getline(configFile, line)) {

                loc_type = loc_parser.get_line_type(line);
                if (loc_type == line_type::empty || loc_type == line_type::comment || loc_type == line_type::invalid)
                    continue;

                if (!found_section) {

                    if (loc_parser.get_section_name(line, buffer) && buffer == section) {
                        found_section = true;
                        CORE_LOG(Debug, "found section: " << buffer);
                    }
                }

                else {

                    CORE_VALIDATE(loc_type != line_type::section, "", "key not found in specified section section", break);

                    if (loc_type != line_type::key_value)
                        continue;

                    if (loc_parser.get_value_to_key(line, key, value)) {

                        found_key = true;
                        break;
                    }
                }
            }
            configFile.close();

            LOG(Trace, "Exit Loop; " << value);
            return found_key; // Key not found
        }

        // 
        bool check_for_configuration(const config_file_types target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override) {

            std::filesystem::path file_path = BUILD_CONFIG_PATH(target_config_file);
            std::ifstream configFile(file_path, std::ios::in | std::ios::binary);
            CORE_VALIDATE(configFile.is_open(), "", "Fauled to open file: [" << file_path << "]", return false);

            std::string line;
            std::ostringstream updatedConfig;
            while (std::getline(configFile, line)) {

                // Check if the line contains the desired section
                if (line.find("[" + section + "]") != std::string::npos) {
                    updatedConfig << line;

                    std::string line_buffer = static_cast<std::string>(line);
                    line_buffer.erase(std::remove_if(line_buffer.begin(), line_buffer.end(), ::isspace), line_buffer.end());

                    // Read and update the lines inside the section until a line with '[' is encountered
                    while (std::getline(configFile, line) && line_buffer[0] != ']') {
                        std::size_t found = line_buffer.find('=');
                        if (found != std::string::npos) {

                            std::string currentKey = line_buffer.substr(0, found);
                            if (currentKey == key) {

                                if (override) {

                                    // Update the value for the specified key
                                    line = '\t' + key + "=" + value + '\n';
                                } else {

                                    value.clear();
                                    value = line_buffer.substr(found +1);
                                    CORE_LOG(Warn, "Saving data to value: " << value);
                                }
                            }

                            CORE_LOG(Trace, "Key not found yet")
                        }
                        updatedConfig << line;
                        CORE_LOG(Trace, "Key not found yet")
                    }
                    updatedConfig << line;
                }
                
                else {
                    updatedConfig << line;
                }
            }


            // Close the original file
            configFile.close();

            if (override) {

                // Open the file in truncation mode to clear its content
                std::ofstream outFile(file_path, std::ios::trunc);
                if (outFile.is_open()) {
                    // Write the updated content to the file
                    outFile << updatedConfig.str();
                    outFile.close();
                    CORE_LOG(Debug, "File updated successfully.");
                    return true;
                } else {
                    CORE_LOG(Trace, "Unable to open the file for writing.");
                    return false;
                }
            }

            LOG(Trace, "Exit Loop; " << value);
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






    line_type parser::get_line_type(const std::string_view& line) {

        for (char c : line) {
                
            if (c == '\t' || c == ' ') 
                continue;

            else if (c == '#')
                return line_type::comment;
            else if (c == '[')
                return line_type::section;
            else
                return line_type::key_value;

        }

        return line_type::empty;
    }

    bool PFF::parser::get_section_name(const std::string_view& line, std::string& name) {

        size_t start = line.find('[');
        size_t end = line.find(']');

        if(start == std::string_view::npos || end == std::string_view::npos)
            return false;

        name = std::string(line.substr(start + 1, end -1));
        return true;
    }


    bool PFF::parser::get_value_to_key(const std::string_view& line, const std::string_view& key, std::string& value) {

        std::string line_buffer = static_cast<std::string>(line);
        line_buffer.erase(std::remove_if(line_buffer.begin(), line_buffer.end(), ::isspace), line_buffer.end());

        std::string key_buffer = static_cast<std::string>(key);
        key_buffer.erase(std::remove_if(key_buffer.begin(), key_buffer.end(), ::isspace), key_buffer.end());

        size_t equalPos = line_buffer.find('=');
        if (equalPos == std::string_view::npos)
            return false;


        std::string key_soluetion = "";
        key_soluetion = std::string(line_buffer.substr(0, equalPos));
        if (key_soluetion != key_buffer)
            return false;

        CORE_LOG(Info, "Read values are; [" << line_buffer << "]/[" << key_buffer << "]");

        value.clear();
        value = std::string(line_buffer.substr(equalPos + 1));
        return true;
    }

    //
    bool set_value_to_key(const std::string_view& line, const std::string_view& key, const std::string& value) {

        size_t equalPos = line.find('=');
        if (equalPos == std::string_view::npos)
            return false;


    }








    /*

    error_code parser::addFile(const config_file_types target_config_file) {

        std::filesystem::path file_path = BUILD_CONFIG_PATH(target_config_file);
        std::ifstream configFile(file_path, std::ios::in | std::ios::binary);
        CORE_VALIDATE(configFile.is_open(), "", "Fauled to open file: [" << file_path << "]", return error_code::error_opening_file);

        configFile.seekg(0, std::ios::end);
        size_t fileSize = configFile.tellg();
        configFile.seekg(0, std::ios::beg);

        std::string fileData(fileSize, ' ');
        configFile.read(fileData.data(), fileSize);

        //AddString(fileData);
    }


    void parser::ProcessChar(char c) {

        // Catch controle keys
        if (std::iscntrl(c)) {
            switch (c) {
            case '\t':
                [[fallthrough]];
            case '\n':
                break;
            default:
                return;
            }
        }

        // Normal char parsing
        switch (m_state) {
        case state::ReadForData:
            switch (c) {
            case ';':
                m_state = state::Comment;
                break;
            case '[':
                m_currentSection.clear();
                m_state = state::Section;
                break;
            case ' ':
                [[fallthrough]];
            case '\t':
                [[fallthrough]];
            case '\n':
                break;
            default:
                m_currentKey.clear();
                m_currentKey += c;
                m_state = state::KVKey;
                break;
            }
            break;
        case state::Comment:
            if (c == '\n') {
                m_state = state::ReadForData;
            }
            break;
        case state::Section:
            switch (c) {
            case ']':
                m_state = state::ReadForData;
                break;
            case '\n':
                [[fallthrough]];
            case '\t':
                throw Exception("Newlines or Tabs are not allowed in section names!");
            default:
                m_currentSection += c;
                break;
            }
            break;
        case state::KVKey:
            switch (c) {
            case ' ':
                m_state = state::KVKeyDone;
                break;
            case '\t':
                [[fallthrough]];
            case '\n':
                throw Exception("Newlines or Tabs are not allowed in the key!");
            default:
                m_currentKey += c;
                break;
            }
            break;
        case state::KVKeyDone:
            switch (c) {
            case ' ':
                [[fallthrough]];
            case '\t':
                break;
            case '\n':
                throw Exception("Every key need to have a value!");
            case '=':
                m_state = state::KVEqual;
                break;
            default:
                throw Exception("Keys are not allowed to have spaces in them!");
            }
            break;
        case state::KVEqual:
            switch (c) {
            case ' ':
                [[fallthrough]];
            case '\t':
                break;
            case '\n':
                throw Exception("Values can't be empty!");
            default:
                m_currentValue.clear();
                m_currentValue += c;
                m_state = state::KVValue;
                break;
            }
            break;
        case state::KVValue:
            if (c == '\n') {
                ParseKVPair(m_currentSection, m_currentKey, m_currentValue);
                m_state = state::ReadForData;
            } else {
                m_currentValue += c;
            }
            break;
        }
    }*/
}
