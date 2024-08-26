#pragma once

namespace PFF {

	enum TokenType {

		PROPERTY, FUNCTION_PROP, CLASS_PROP, STRUCT_PROP,
		STRUCT_KW, CLASS_KW, COLON,

		LEFT_PAREN, RIGHT_PAREN, HASHTAG, LEFT_BRACKET, RIGHT_BRACKET,
		SEMICOLON, EQUAL, PLUS_EQUAL, MINUS_EQUAL, TIMES_EQUAL, DIV_EQUAL,
		MODULO_EQUAL, CARET_EQUAL, AMPERSAND_EQUAL, BAR_EQUAL, RIGHT_SHIFT_EQUAL,
		LEFT_SHIFT_EQUAL, EQUAL_EQUAL, BANG_EQUAL, LESS_THAN_EQUAL, GREATER_THAN_EQUAL,

		CONST_KW, IDENTIFIER, AUTO_KW,

		STAR, REF, LEFT_ANGLE_BRACKET, RIGHT_ANGLE_BRACKET,

		STRING_LITERAL, NUMBER, TRUE_KW, FALSE_KW,

		END_OF_FILE,
		ERROR_TYPE
	};

	struct Token {
		
		int m_Line;
		int m_Column;
		TokenType m_Type;
		std::string m_Lexeme;
	};

	class script_scanner {
	public:
		
		script_scanner(const std::filesystem::path& filepath);

		std::vector<Token> scan_tokens();

	private:
		
		FORCEINLINE bool is_digit(char c) const { return c >= '0' && c <= '9'; }
		FORCEINLINE bool is_alpha_numeric(char c) const { return is_alpha(c) || is_digit(c); }
		FORCEINLINE bool is_alpha(char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
		FORCEINLINE bool at_end() const { return m_cursor == m_file_contents.size(); }
		FORCEINLINE Token generate_token(TokenType m_Type, std::string lexeme) { return Token{ m_line, m_column, m_Type, lexeme }; }
		FORCEINLINE Token generate_error_token() { return Token{ -1, -1, TokenType::ERROR_TYPE, "" }; }

		Token scan_token();
		Token property_identifier();
		Token number();
		Token string();
		
		char advance();
		char peek();
		char peek_next();
		char peek_next_next();
		bool match(char expected);


		const std::map<std::string, TokenType> keywords = {
			{ "PFF_PROPERTY",   TokenType::PROPERTY },
			{ "PFF_CLASS",      TokenType::CLASS_PROP },
			{ "PFF_STRUCT",     TokenType::STRUCT_PROP },
			{ "PFF_FUNCTION",   TokenType::FUNCTION_PROP },
			{ "auto",			TokenType::AUTO_KW },
			{ "const",			TokenType::CONST_KW },
			{ "class",			TokenType::CLASS_KW },
			{ "struct",			TokenType::STRUCT_KW }
		};

		std::string				m_file_contents;
		std::filesystem::path	m_filepath;
		int						m_cursor = 0;
		int						m_line = 1;
		int						m_column = 0;
		int						m_start = 0;
	};
}
