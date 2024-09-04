#pragma once

namespace PFF {

	enum token_type {

		PROPERTY, FUNCTION_PROP, CLASS_PROP, STRUCT_PROP,
		STRUCT_KW, CLASS_KW, COLON,

		LEFT_PAREN, RIGHT_PAREN, HASHTAG, LEFT_BRACKET, RIGHT_BRACKET,
		SEMICOLON, EQUAL, PLUS_EQUAL, MINUS_EQUAL, TIMES_EQUAL, DIV_EQUAL,
		MODULO_EQUAL, CARET_EQUAL, AMPERSAND_EQUAL, BAR_EQUAL, RIGHT_SHIFT_EQUAL,
		LEFT_SHIFT_EQUAL, EQUAL_EQUAL, BANG_EQUAL, LESS_THAN_EQUAL, GREATER_THAN_EQUAL,

		CONST_KW, IDENTIFIER, AUTO_KW,

		NAMESPACE, PUBLIC, PROTECTED, PRIVATE,

		STAR, REF, LEFT_ANGLE_BRACKET, RIGHT_ANGLE_BRACKET,

		STRING_LITERAL, NUMBER, TRUE_KW, FALSE_KW,

		END_OF_FILE,
		ERROR_TYPE
	};

	struct token {
		
		int m_line;
		int m_column;
		token_type m_type;
		std::string m_lexeme;
	};

	class script_scanner {
	public:
		
		script_scanner(const std::filesystem::path& filepath);

		std::vector<token> scan_tokens();

	private:
		
		FORCEINLINE bool is_digit(char c) const { return c >= '0' && c <= '9'; }
		FORCEINLINE bool is_alpha_numeric(char c) const { return is_alpha(c) || is_digit(c); }
		FORCEINLINE bool is_alpha(char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
		FORCEINLINE bool at_end() const { return m_cursor == m_file_contents.size(); }
		FORCEINLINE token generate_token(token_type m_Type, std::string lexeme) { return token{ m_line, m_column, m_Type, lexeme }; }
		FORCEINLINE token generate_error_token() { return token{ -1, -1, token_type::ERROR_TYPE, "" }; }

		token scan_token();
		token property_identifier();
		token number();
		token string();
		
		char advance();
		char peek();
		char peek_next();
		char peek_next_next();
		bool match(char expected);


		const std::map<std::string, token_type> keywords = {
			{ "PFF_PROPERTY",   token_type::PROPERTY },
			{ "PFF_CLASS",      token_type::CLASS_PROP },
			{ "PFF_STRUCT",     token_type::STRUCT_PROP },
			{ "PFF_FUNCTION",   token_type::FUNCTION_PROP },
			{ "namespace",      token_type::NAMESPACE },
			{ "public",			token_type::PUBLIC },
			{ "protected",      token_type::PROTECTED },
			{ "private",		token_type::PRIVATE },
			{ "auto",			token_type::AUTO_KW },
			{ "const",			token_type::CONST_KW },
			{ "class",			token_type::CLASS_KW },
			{ "struct",			token_type::STRUCT_KW }
		};

		std::string				m_file_contents;
		std::filesystem::path	m_filepath;
		int						m_cursor = 0;
		int						m_line = 1;
		int						m_column = 0;
		int						m_start = 0;
	};
}
