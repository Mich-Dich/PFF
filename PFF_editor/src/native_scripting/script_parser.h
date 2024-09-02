#pragma once

#include "script_scanner.h"

namespace PFF {

	struct PFF_variable {
		std::string						type;
		std::string						identifier;
	};

	struct PFF_class {
		std::string						class_name;
		std::filesystem::path			full_filepath;
		std::list<PFF_variable>			variables;
	};

	struct PFF_struct {
		std::string						struct_name;
		const std::filesystem::path&	full_filepath;
		std::list<PFF_variable>			variables;
	};

	class script_parser {
	public:

		script_parser(std::vector<Token>& tokens, const std::filesystem::path& full_filepath)
			: m_Tokens(tokens), m_full_filepath(full_filepath) {}

		std::string generate_header_file();
		void debug_print();
		void parse();

		PFF_DEFAULT_GETTER(std::vector<PFF_class>&, classes)
		static std::string get_filename_as_class_name(std::string filename);

		FORCEINLINE bool can_generate_header_file() const { return m_structs.size() != 0 || m_classes.size() != 0; }

	private:

		void ParseClass();
		void ParseStruct();
		PFF_variable ParseVariable();
		const Token& Expect(TokenType type);
		bool Match(TokenType type);

		std::vector<Token>::iterator	m_Current_iter;
		u64								m_current_token;
		std::filesystem::path			m_full_filepath;

		std::vector<Token>&				m_Tokens;
		std::vector<PFF_class>			m_classes;
		std::vector<PFF_struct>			m_structs;
	};

}
