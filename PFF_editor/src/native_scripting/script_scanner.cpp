
#include "util/pch_editor.h"

#include "script_scanner.h"

namespace PFF {

	script_scanner::script_scanner(const std::filesystem::path& filepath)
		: m_filepath(filepath) {

		//std::stringstream buffer;
		//buffer << file.rdbuf();
		//m_FileContents = buffer.str();

		std::ifstream file(filepath, std::ios::binary);
		CORE_VALIDATE(file.is_open(), return, "", "Faled to open file; " << filepath)
		std::stringstream buffer;
		buffer << file.rdbuf();
		m_file_contents = buffer.str();
		file.close();
	}

	std::vector<token> script_scanner::scan_tokens() {

		//CORE_LOG(Trace, "scanning file [" << util::extract_path_from_directory(m_filepath, "src").generic_string() << "]");
		auto tokens = std::vector<token>();

		m_cursor = 0;
		while (!at_end()) {
			m_start = m_cursor;
			token token = scan_token();
			if (token.m_type != token_type::ERROR_TYPE)
				tokens.push_back(token);
		}

		tokens.emplace_back(token{ m_line, m_column, token_type::END_OF_FILE, "EOF" });
		return tokens;
	}






	token script_scanner::scan_token() {

		char c = advance();
		switch (c) {
			// Single character tokens
		case '<': return generate_token(token_type::LEFT_ANGLE_BRACKET, "<");
		case '>': return generate_token(token_type::RIGHT_ANGLE_BRACKET, "<");
		case '*': return generate_token(token_type::STAR, "*");
		case '&': return generate_token(token_type::REF, "&");
		case '(': return generate_token(token_type::LEFT_PAREN, "(");
		case ')': return generate_token(token_type::RIGHT_PAREN, ")");
		case '#': return generate_token(token_type::HASHTAG, "#");
		case '{': return generate_token(token_type::LEFT_BRACKET, "{");
		case '}': return generate_token(token_type::RIGHT_BRACKET, "}");
		case ';': return generate_token(token_type::SEMICOLON, ";");
		case '=': return generate_token(token_type::EQUAL, "=");
		case ':': return generate_token(token_type::COLON, ":");
		case ',': return generate_token(token_type::COMA, ",");
		case '"': return string();
			// Whitespace
		case '/':
		{
			if (match('/')) {
				while (peek() != '\n' && !at_end())
					advance();
				return generate_error_token();
			} else if (match('*')) {
				while (!at_end() && peek() != '*' && peek_next() != '/') {
					c = advance();
					if (c == '\n') {
						m_column = 0;
						m_line++;
					}
				}

				// Consume */
				if (!at_end()) match('*');
				if (!at_end()) match('/');
				return generate_error_token();
			}
			break;
		}
		case ' ': // return generate_token(token_type::SPACE, " ");
		case '\r':
		case '\t':
			// Ignore whitespace
			return generate_error_token();
		case '\n':
			m_column = 0;
			m_line++;
			return generate_error_token();
		default:
			if (is_alpha(c)) 
				return property_identifier();

			if (is_digit(c))
				return number();
			
			break;
		}

		return generate_error_token();
	}

	token script_scanner::property_identifier() {

		while (is_alpha_numeric(peek()) || peek() == '_')
			advance();

		std::string text = m_file_contents.substr(m_start, m_cursor - m_start);
		token_type type = token_type::IDENTIFIER;
		auto iter = keywords.find(text);
		if (iter != keywords.end()) {
			type = iter->second;
		}

		return token{ m_line, m_column, type, text };
	}

	token script_scanner::number() {

		while (is_digit(peek()))
			advance();

		if (peek() == '.' && is_digit(peek_next())
			|| (peek() == '.' && peek_next() == 'f')) {

			advance();
			while (is_digit(peek()) || peek() == 'f')
				advance();

			CORE_VALIDATE(peek() != '.', return generate_error_token(), "", "Unexpected number literal at [" << m_line << "] col[" << m_column << "]");
		}

		bool hasE = false;
		if (peek() == '.' && (is_digit(peek_next())
			|| (peek_next() == 'e' && is_digit(peek_next_next()))
			|| (peek_next() == 'E' && is_digit(peek_next_next())))) {

			advance();
			while (is_digit(peek()))
				advance();

			if ((peek() == 'e' || peek() == 'E') && (is_digit(peek_next()) 
				|| ((peek_next() == '-' && is_digit(peek_next_next())) 
				|| (peek_next() == '+' && is_digit(peek_next_next()))))) {

				advance();
				while (is_digit(peek()))
					advance();

				if ((peek() == '-' || peek() == '+') && is_digit(peek_next())) {

					advance();
					while (is_digit(peek())) advance();
				}
				CORE_VALIDATE(peek() != '.', return generate_error_token(), "", "Unexpected number literal at [" << m_line << "] col[" << m_column << "]");
			}
		}

		if ((peek() == 'e' || peek() == 'E') && (is_digit(peek_next()) || ((peek_next() == '-' && is_digit(peek_next_next())) || (peek_next() == '+' && is_digit(peek_next_next()))))) {

			advance();
			while (is_digit(peek()))
				advance();

			if ((peek() == '-' || peek() == '+') && is_digit(peek_next())) {

				advance();
				while (is_digit(peek()))
					advance();
			}
			CORE_VALIDATE(peek() != '.', return generate_error_token(), "", "Unexpected number literal at [" << m_line << "] col[" << m_column << "]");
		}

		return token{ m_line, m_column, token_type::NUMBER, m_file_contents.substr(m_start, m_cursor - m_start) };
	}

	token script_scanner::string() {
		
		while (peek() != '"' && !at_end()) {
			if (peek() == '\n') {
				m_line++;
				m_column = -1;
			}
			advance();
		}

		CORE_VALIDATE(!at_end(), return generate_error_token(), "", "Unexpected string literal at [" << m_line << "] col[" << m_column << "]");
		advance();

		std::string value = m_file_contents.substr(m_start, m_cursor - m_start);
		return token{ m_column, m_line, token_type::STRING_LITERAL, value };
	}

	char script_scanner::advance(int count) {
		
		char c = m_file_contents[m_cursor];
		m_cursor += count;
		m_column += count;
		return c;
	}

	char script_scanner::peek() {

		if (at_end())
			return '\0';
		return m_file_contents[m_cursor];
	}

	char script_scanner::peek_next() {
		
		if (at_end() || m_cursor == m_file_contents.size() - 1)
			return '\0';

		return m_file_contents[m_cursor + 1];
	}

	char script_scanner::peek_next_next() {
		
		if (at_end() || m_cursor == m_file_contents.size() - 1 || m_cursor == m_file_contents.size() - 2) 
			return '\0';

		return m_file_contents[m_cursor + 1];
	}

	bool script_scanner::match(char expected) {

		if (at_end())
			return false;
		if (m_file_contents[m_cursor] != expected)
			return false;

		m_cursor++;
		m_column++;
		return true;
	}
}
