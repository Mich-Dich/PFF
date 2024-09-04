#pragma once

#include "script_scanner.h"

namespace PFF {

	struct PFF_variable {
		std::string						type;
		std::string						identifier;
	};

	struct PFF_class {
		std::string						class_name;
		std::string						parent_class_name;
		std::string						class_name_without_namespace;
		std::filesystem::path			full_filepath;
		std::list<PFF_variable>			variables;
	};

	struct PFF_struct {
		std::string						struct_name;
		std::string						struct_name_without_namespace;
		const std::filesystem::path&	full_filepath;
		std::list<PFF_variable>			variables;
	};

	class script_parser {
	public:

		script_parser(std::vector<token>& tokens, const std::filesystem::path& full_filepath)
			: m_Tokens(tokens), m_full_filepath(full_filepath) {}

		std::string generate_header_file();
		void debug_print();
		void parse();

		PFF_DEFAULT_GETTER(std::vector<PFF_class>&, classes)
		static std::string get_filename_as_class_name(std::string filename);

		FORCEINLINE bool can_generate_header_file() const { return m_structs.size() != 0 || m_classes.size() != 0; }

	private:

		void parse_class();
		void parse_struct();
		PFF_variable parse_variable();
		const token& expect(token_type type);
		bool match(token_type type);

		std::vector<token>::iterator	m_Current_iter;
		u64								m_current_token;
		std::filesystem::path			m_full_filepath;

		std::vector<token>&				m_Tokens;
		std::vector<PFF_class>			m_classes;
		std::vector<PFF_struct>			m_structs;
		std::string						m_current_namespace;
	};

}
