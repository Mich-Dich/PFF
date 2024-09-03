
#include "util/pch_editor.h"

#include "script_parser.h"

namespace PFF {
	
	static Token							s_error_token = Token{ -1, -1, TokenType::ERROR_TYPE, "" };

	FORCEINLINE Token generate_error_token() { return Token{ -1, -1, TokenType::ERROR_TYPE, "" }; }
	FORCEINLINE static bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
	FORCEINLINE static bool is_alpha_numeric(char c) { return is_alpha(c) || (c >= '0' && c <= '9'); }
	FORCEINLINE static bool is_variable_friendly(char c) { return is_alpha_numeric(c) || c == '_'; }

#define ADD_KOMA(condition)					if (condition)				\
												file << ",";

	std::string script_parser::get_filename_as_class_name(std::string filename) {
		
		for (int i = 0; i < filename.length(); i++) {
			if (i == 0 && !is_alpha(filename[i]))
				filename[i] = '_';
			else if (!is_variable_friendly(filename[i]))
				filename[i] = '_';
		}

		return filename;
	}

	std::string script_parser::generate_header_file() {

		using namespace entt::literals;
		CORE_LOG(Trace, "generating header for: [" << util::extract_path_from_directory(m_full_filepath, "src").generic_string() << "]");

		std::ostringstream file;
		file << R"(
#pragma once

#include <PFF.h>				// main engine header
#define ENTT_STANDARD_CPP
#include <entt/core/hashed_string.hpp>
#include <entt/entt.hpp>

)";
		file << "#include \"" << util::extract_path_from_directory(m_full_filepath, "src").generic_string() << "\"\n\n";

		// begin namespace
		file << "namespace PFF::reflect_" << get_filename_as_class_name(m_full_filepath.filename().string()) << " {\n";

		file << R"(
	using namespace entt::literals;
	bool initialized = false;

)";

		// append ids as entt hash strings
		{
			file << "\tstd::vector<entt::id_type> ids = {\n";

			for (size_t x = 0; x < m_structs.size(); x++) {
				int y = 0;
				for (auto uvar : m_structs[x].variables) {

					file << "\n\t\t\"" << m_structs[x].struct_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs";
					ADD_KOMA((x != m_structs.size() - 1 || y != m_structs[x].variables.size() - 1) && m_classes.size() != 0);
						
					y++;
				}
			}
			for (size_t x = 0; x < m_classes.size(); x++) {
				int y = 0;
				for (auto uvar : m_classes[x].variables) {

					file << "\n\t\t\"" << m_classes[x].class_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs";
					ADD_KOMA(x != m_classes.size() - 1 || y != m_classes[x].variables.size() - 1);
						
					y++;
				}
			}

			file << "\n\t};\n\n";
		}

		// append debug names map
		{
			file << "\tstd::map<entt::id_type, const char*> debug_names = {\n";

			int id = 0;
			for (size_t x = 0; x < m_structs.size(); x++) {
				int j = 0;
				file << "\n\t\t{ entt::type_hash<" << m_structs[x].struct_name.c_str() << ">::value(), \"" << m_structs[x].struct_name.c_str() << "\"},";
				for (auto uvar : m_structs[x].variables) {

					file << "\n\t\t{ids[" << id << "], \"" << uvar.identifier.c_str() << "\"}";
					ADD_KOMA((x != m_structs.size() - 1 || x != m_structs[x].variables.size() - 1) && m_classes.size() != 0);

					id++;
					j++;
				}
			}
			for (size_t x = 0; x < m_classes.size(); x++) {
				int j = 0;
				file << "\n\t\t{ entt::type_hash<" << m_classes[x].class_name.c_str() << ">::value(), \"" << m_classes[x].class_name.c_str() << "\"},";
				for (auto uvar : m_classes[x].variables) {

					file << "\n\t\t{ids[" << id << "], \"" << uvar.identifier.c_str() << "\"}";
					ADD_KOMA(x != m_classes.size() - 1 || j != m_classes[x].variables.size() - 1);

					id++;
					j++;
				}
			}

			file << "\n\t};\n\n";
		}
		
		// append string_to_TypeMap
		// TODO: Create variable string to type id mapper...
		{
			file << "\tstd::map<std::string, entt::id_type> string_to_map = {\n";
			for (size_t x = 0; x < m_structs.size(); x++) {
				
				file << "\n\t\t{ \"" << m_structs[x].struct_name.c_str() << "\", entt::type_hash<" << m_structs[x].struct_name.c_str() << ">::value() }";
				ADD_KOMA(x != m_structs.size() - 1 && m_classes.size() == 0);
			}

			for (size_t x = 0; x < m_classes.size(); x++) {

				file << "\n\t\t{ \"" << m_classes[x].class_name.c_str() << "\", entt::type_hash<" << m_classes[x].class_name.c_str() << ">::value() }";
				ADD_KOMA(x != m_classes.size() - 1);
			}

			file << "\n\t};\n";
		}

		// init function
		{
			file << R"(
	void init() {

		if (initialized)
			return;

		initialized = true;
)";
			int id = 0;
			for (auto ustruct : m_structs) {
				std::string string_class_name = ustruct.struct_name;
				string_class_name[0] = tolower(string_class_name[0]);
				file << "\t\tauto " << string_class_name.c_str() << "_factory = entt::meta<" << ustruct.struct_name.c_str() << ">()\n";

				for (auto var : ustruct.variables) {
					file << "\t\t\t.data<&" << ustruct.struct_name.c_str() << "::" << var.identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
					id++;
				}
				file << "\t\t\t.type(entt::type_hash<" << ustruct.struct_name.c_str() << ">::value(); \n\n";
			}

			for (auto uclass : m_classes) {
				std::string string_class_name = uclass.class_name;
				string_class_name[0] = tolower(string_class_name[0]);
				file << "\t\tauto " << string_class_name.c_str() << "_factory = entt::meta<" << uclass.class_name.c_str() << ">()\n";

				for (auto var : uclass.variables) {
					file << "\t\t\t.data<&" << uclass.class_name.c_str() << "::" << var.identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
					id++;
				}

				file << "\t\t\t.type(entt::type_hash<" << uclass.class_name.c_str() << ">::value()); \n\n";
			}
			file << "\t}\n\n";
		}

		// save a scripts function
		{
			for (size_t x = 0; x < m_classes.size(); x++) {

				file << "\t// " << m_classes[x].class_name.c_str() << "\n";
				file << "\tvoid serialize_script(" << m_classes[x].class_name.c_str() << "& component, const std::filesystem::path filepath, serializer::option option) { ";

				if (m_classes[x].variables.size() == 0) {

					file << "}\n\n";
					continue;
				}

				file << "\n\n\t\tserializer::yaml(filepath, \"" << m_classes[x].class_name.c_str() << "\", option)";
				for (auto uvar : m_classes[x].variables)
					file << "\n\t\t\t.entry(KEY_VALUE(component." << uvar.identifier.c_str() << "))";
				file << ";";

				file << "\n\t}\n\n";
			}
		}

		// save scripts function
		/*{
			file << "\tvoid serialize_scripts(const std::filesystem::path filepath, entt::registry& registry, serializer::option option) {\n\n";
			for (auto uclass : m_classes) {
				file << "\t\t{ // " << uclass.class_name.c_str() << "\n";

				file << "\t\t\tauto view = registry.view<" << uclass.class_name.c_str() << ">();\n";
				file << "\t\t\tfor (auto entity : view)\n";
				file << "\t\t\t{\n";
				file << "\t\t\t\tauto comp = registry.get<" << uclass.class_name.c_str() << ">(entity);\n";
				file << "\t\t\t\tserialize_script(comp, filepath, option);\n";
				file << "\t\t\t}\n";

				file << "\t\t}\n";
			}
			file << "\t}\n\n";
		}*/

		// delete scripts function
		{
			file << "\tvoid delete_scripts(entt::registry& registry) {\n\n";
			for (auto uclass : m_classes) {

				file << "\t\tregistry.clear<" << uclass.class_name.c_str() << ">();\n";
			}
			file << "\t}\n\n";
		}
		
		// add_component function
		{
			file << "\tvoid add_component(std::string class_name, PFF::entity entity) {\n\n";
			for (size_t x = 0; x < m_classes.size(); x++) {

				file << ((x == 0) ? "\t\tif" : "\t\telse if");
				file << "(class_name == \"" << m_classes[x].class_name.c_str() << "\") {\n\n";

				file << "\t\t\tif constexpr (std::is_base_of_v<procedural_mesh_script, " << m_classes[x].class_name.c_str() << ">)\n";
				file << "\t\t\t\tentity.add_procedural_mesh_component<" << m_classes[x].class_name.c_str() << ">(\"" << m_classes[x].class_name.c_str() << "\");\n\n";

				file << "\t\t\telse if constexpr (std::is_base_of_v<entity_script, " << m_classes[x].class_name.c_str() << ">)\n";
				file << "\t\t\t\tentity.add_script_component<" << m_classes[x].class_name.c_str() << ">();\n\n";
							
				file << "\t\t}\n";
			}
			file << "\t}\n\n";
		}

		file << "}\n";				// end namespace

		return file.str();
	}

	void script_parser::debug_print() {

		for (auto structIter = m_structs.begin(); structIter != m_structs.end(); structIter++) {
			CORE_LOG(Info, structIter->struct_name.c_str() << " {");
			for (auto varIter = structIter->variables.begin(); varIter != structIter->variables.end(); varIter++)
				CORE_LOG(Info, "    Type<" << varIter->type.c_str() << "> " << varIter->identifier.c_str());
			
		}

		for (auto classIter = m_classes.begin(); classIter != m_classes.end(); classIter++) {
			CORE_LOG(Info, classIter->class_name.c_str() << " {");
			for (auto varIter = classIter->variables.begin(); varIter != classIter->variables.end(); varIter++)
				CORE_LOG(Info, "    Type<" << varIter->type.c_str() << "> " << varIter->identifier.c_str());

			CORE_LOG(Info, "}");
		}
	}





	void script_parser::parse() { 
	
		m_current_token = 0;
		m_Current_iter = m_Tokens.begin();
		do {
			if (Match(TokenType::STRUCT_PROP)) {
				Expect(TokenType::LEFT_PAREN);
				Expect(TokenType::RIGHT_PAREN);
				Match(TokenType::SEMICOLON);
				Expect(TokenType::STRUCT_KW);
				ParseStruct();
			} else if (Match(TokenType::CLASS_PROP)) {
				Expect(TokenType::LEFT_PAREN);
				Expect(TokenType::RIGHT_PAREN);
				Match(TokenType::SEMICOLON);
				Expect(TokenType::CLASS_KW);
				ParseClass();
			} else {
				m_current_token++;
				m_Current_iter++;
			}
		} while (m_current_token < m_Tokens.size() && m_Current_iter->m_type != TokenType::END_OF_FILE);
	}

	void script_parser::ParseClass() {

		Token classType = Expect(TokenType::IDENTIFIER);

		if (Match(TokenType::COLON)) {
			while (!Match(TokenType::LEFT_BRACKET)) {
				m_current_token++;
				m_Current_iter++;
			}
		} else {
			Expect(TokenType::LEFT_BRACKET);
		}

		PFF_class clazz = PFF_class{ classType.m_lexeme, m_full_filepath, std::list<PFF_variable>() };

		int level = 1;
		while (m_Current_iter->m_type != TokenType::END_OF_FILE) {
			if (Match(TokenType::LEFT_BRACKET)) {
				level++;
			} else if (Match(TokenType::RIGHT_BRACKET)) {
				level--;
				if (level <= 0) {
					Expect(TokenType::SEMICOLON);
					break;
				}
			} else if (Match(TokenType::PROPERTY)) {
				Expect(TokenType::LEFT_PAREN);
				Match(TokenType::IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
				Expect(TokenType::RIGHT_PAREN);
				Match(TokenType::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
				clazz.variables.push_back(ParseVariable());
			} else {
				m_current_token++;
				m_Current_iter++;
			}
		}

		m_classes.push_back(clazz);
	}

	void script_parser::ParseStruct() {

		Token structType = Expect(TokenType::IDENTIFIER);
		Expect(TokenType::LEFT_BRACKET);
		PFF_struct structure = PFF_struct{ structType.m_lexeme, m_full_filepath, std::list<PFF_variable>() };
		int level = 1;
		while (m_Current_iter->m_type != TokenType::END_OF_FILE) {

			if (Match(TokenType::LEFT_BRACKET))
				level++;

			else if (Match(TokenType::RIGHT_BRACKET)) {
				level--;
				if (level <= 1) {
					Expect(TokenType::SEMICOLON);
					break;
				}
			
			} else if (Match(TokenType::PROPERTY)) {
				Expect(TokenType::LEFT_PAREN);
				Match(TokenType::IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
				Expect(TokenType::RIGHT_PAREN);
				Match(TokenType::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
				structure.variables.push_back(ParseVariable());
			}
			// TODO: This might need another else statement like class parser
		}

		m_structs.push_back(structure);
	}

	PFF_variable script_parser::ParseVariable() {

		std::vector<Token> all_tokens_before_semi_colon = std::vector<Token>();
		std::vector<Token>::iterator current;
		int after_identifier_index = -1;

		do {
			current = m_Tokens.begin();
			std::advance(current, m_current_token);
			
			if (m_Current_iter->m_type == TokenType::SEMICOLON && after_identifier_index == -1)
				after_identifier_index = static_cast<int>(all_tokens_before_semi_colon.size());
			else if (m_Current_iter->m_type == TokenType::EQUAL)
				after_identifier_index = static_cast<int>(all_tokens_before_semi_colon.size());
			
			all_tokens_before_semi_colon.push_back(*current);
			m_current_token++;
			m_Current_iter++;
		} while (current->m_type != TokenType::END_OF_FILE && current->m_type != TokenType::SEMICOLON);

		Token& variable_identifier = generate_error_token();

//#define NEW_VERSION
#ifndef NEW_VERSION
		if (after_identifier_index != -1) {
			if (after_identifier_index <= 1) 
				CORE_LOG(Error, "Weird equal sign placement. line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");
			
			auto it = all_tokens_before_semi_colon.begin();
			std::advance(it, after_identifier_index - 1);
			variable_identifier = *it;
		} else {
			CORE_LOG(Error, "Cannot find variable identifier. line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");
			return PFF_variable{ "ERROR", variable_identifier.m_lexeme };
		}
#else
		auto error_var = PFF_variable{ "ERROR", variable_identifier.m_lexeme };
		CORE_VALIDATE(after_identifier_index != -1, return error_var, "", "Cannot find variable identifier. line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");

		if (after_identifier_index <= 1)
			CORE_LOG(Error, "Weird equal sign placement. line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");

		auto it = all_tokens_before_semi_colon.begin();
		std::advance(it, after_identifier_index - 1);
		variable_identifier = *it;
#endif

		Token& type_identifier = generate_error_token();
		auto endIter = all_tokens_before_semi_colon.begin();
		std::advance(endIter, after_identifier_index - 1);
		for (auto iter = all_tokens_before_semi_colon.begin(); iter != endIter; iter++) {
			if (iter->m_type == TokenType::IDENTIFIER) {
				type_identifier = *iter;
				break;
			}
		}

		return PFF_variable{ type_identifier.m_lexeme, variable_identifier.m_lexeme };
	}

	const Token& script_parser::Expect(TokenType type) {

		if (m_Current_iter->m_type != type) {

			CORE_LOG(Error, "Error: Expected [" << type << "] but instead got [" << m_Current_iter->m_type << "] . line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");
			return s_error_token;
		}

		auto tokenToReturn = m_Current_iter;
		m_current_token++;
		m_Current_iter++;
		return *tokenToReturn;
	}

	bool script_parser::Match(TokenType type) {

		if (m_Current_iter->m_type == type) {
			m_current_token++;
			m_Current_iter++;
			return true;
		}

		return false;
	}
}
