
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

	std::vector<Token> script_scanner::scan_tokens() {

		CORE_LOG(Trace, "Scanning file: " << m_filepath);
		auto tokens = std::vector<Token>();

		m_cursor = 0;
		while (!at_end()) {
			m_start = m_cursor;
			Token token = scan_token();
			if (token.m_Type != TokenType::ERROR_TYPE)
				tokens.push_back(token);
		}

		tokens.emplace_back(Token{ m_line, m_column, TokenType::END_OF_FILE, "EOF" });
		return tokens;
	}






	Token script_scanner::scan_token() {

		char c = advance();
		switch (c) {
			// Single character tokens
		case '<': return generate_token(TokenType::LEFT_ANGLE_BRACKET, "<");
		case '>': return generate_token(TokenType::RIGHT_ANGLE_BRACKET, "<");
		case '*': return generate_token(TokenType::STAR, "*");
		case '&': return generate_token(TokenType::REF, "&");
		case '(': return generate_token(TokenType::LEFT_PAREN, "(");
		case ')': return generate_token(TokenType::RIGHT_PAREN, ")");
		case '#': return generate_token(TokenType::HASHTAG, "#");
		case '{': return generate_token(TokenType::LEFT_BRACKET, "{");
		case '}': return generate_token(TokenType::RIGHT_BRACKET, "}");
		case ';': return generate_token(TokenType::SEMICOLON, ";");
		case '=': return generate_token(TokenType::EQUAL, "=");
		case ':': return generate_token(TokenType::COLON, ":");
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
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace
			return generate_error_token();
		case '\n':
			m_column = 0;
			m_line++;
			return generate_error_token();
		default:
			if (is_alpha(c)) {
				return property_identifier();
			}
			break;
		}

		return generate_error_token();
	}

	Token script_scanner::property_identifier() {

		while (is_alpha_numeric(peek()) || peek() == '_')
			advance();

		std::string text = m_file_contents.substr(m_start, m_cursor - m_start);
		TokenType type = TokenType::IDENTIFIER;
		auto iter = keywords.find(text);
		if (iter != keywords.end()) {
			type = iter->second;
		}

		return Token{ m_line, m_column, type, text };
	}

	Token script_scanner::number() {

		while (is_digit(peek()))
			advance();

		bool hasE = false;
		if (peek() == '.' && (is_digit(peek_next()) || (peek_next() == 'e' && is_digit(peek_next_next()))
			|| (peek_next() == 'E' && is_digit(peek_next_next())))) {
			advance();

			while (is_digit(peek())) {
				advance();
			}

			if ((peek() == 'e' || peek() == 'E') && (is_digit(peek_next()) ||
				((peek_next() == '-' && is_digit(peek_next_next())) || (peek_next() == '+' && is_digit(peek_next_next()))))) {
				advance();
				while (is_digit(peek())) advance();

				if ((peek() == '-' || peek() == '+') && is_digit(peek_next())) {
					advance();
					while (is_digit(peek())) advance();
				}
				
				CORE_VALIDATE(peek() != '.', return generate_error_token(), "", "Unexpected number literal at [" << m_line << "] col[" << m_column << "]");
			}
		}

		if ((peek() == 'e' || peek() == 'E') && (is_digit(peek_next()) ||
			((peek_next() == '-' && is_digit(peek_next_next())) || (peek_next() == '+' && is_digit(peek_next_next()))))) {
			advance();
			while (is_digit(peek())) advance();

			if ((peek() == '-' || peek() == '+') && is_digit(peek_next())) {
				advance();
				while (is_digit(peek())) advance();
			}

			CORE_VALIDATE(peek() != '.', return generate_error_token(), "", "Unexpected number literal at [" << m_line << "] col[" << m_column << "]");
		}

		return Token{ m_line, m_column, TokenType::NUMBER, m_file_contents.substr(m_start, m_cursor - m_start) };
	}

	Token script_scanner::string() {
		
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
		return Token{ m_column, m_line, TokenType::STRING_LITERAL, value };
	}

	char script_scanner::advance() {
		
		char c = m_file_contents[m_cursor];
		m_cursor++;
		m_column++;
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
