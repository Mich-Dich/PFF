#pragma once

#include "util/util.h"


namespace PFF {

	namespace config {

		enum class config_file_types {
			default_editor,
			default_engine,
			default_game,
			default_input,
		};


		// @brief Initializes the config system by checking for configuration directory and files.
		void init();

		std::string configtype_to_string(config_file_types type);

		// @brief Tries to find a specified key in the given configuration file.
		// @param [target_config_file] The type of the configuration file to search.
		// @param [key] The key to search for in the configuration file.
		// @param [value] Reference to a string that will store the value associated with the found key.
		// @return [error_code] Indicates the outcome of the operation - success, file not found, or key not found.
		bool try_to_find(const config_file_types target_config_file, const std::string& section, const std::string& key, std::string& value);

		bool check_for_configuration(const config_file_types target_config_file, const std::string& section, const std::string& key, std::string& value, const bool override);
	}


	enum class line_type {
		section,
		key_value,
		comment,
		empty,
		invalid,
	};
		
		
	class parser {

	public:
		parser() {};
		~parser() {};

		line_type get_line_type(const std::string_view& line);
		bool get_section_name(const std::string_view& line, std::string& name);
		bool get_value_to_key(const std::string_view& line, const std::string_view& key, std::string& value);
		bool set_value_to_key(const std::string_view& line, const std::string_view& key, const std::string& value);

	private:
		void ProcessChar(char c);

	private:
		enum class state {				
			ReadForData,		// Internal state flushed and ready for any ini data
			Comment,			// Comment started
			Section,			// Section is beeing defined
			KVKey,				// Key value
			KVKeyDone,
			KVEqual,			// Equal sign of KV value reached
			KVValue,			// KV Value coming
		};
		state m_state = state::ReadForData;
		std::string m_currentSection, m_currentKey, m_currentValue;

	};
}