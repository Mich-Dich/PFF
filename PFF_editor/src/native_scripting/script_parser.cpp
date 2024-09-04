
#include "util/pch_editor.h"

#include "script_parser.h"

namespace PFF {
	
	static token							s_error_token = token{ -1, -1, token_type::ERROR_TYPE, "" };

	FORCEINLINE token generate_error_token() { return token{ -1, -1, token_type::ERROR_TYPE, "" }; }
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
				file << "\t\tauto " << ustruct.struct_name_without_namespace.c_str() << "_factory = entt::meta<" << ustruct.struct_name.c_str() << ">()\n";

				for (auto var : ustruct.variables) {
					file << "\t\t\t.data<&" << ustruct.struct_name.c_str() << "::" << var.identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
					id++;
				}
				file << "\t\t\t.type(entt::type_hash<" << ustruct.struct_name.c_str() << ">::value(); \n\n";
			}

			for (auto uclass : m_classes) {
				std::string string_class_name = uclass.class_name;
				string_class_name[0] = tolower(string_class_name[0]);
				file << "\t\tauto " << uclass.class_name_without_namespace.c_str() << "_factory = entt::meta<" << uclass.class_name.c_str() << ">()\n";

				for (auto var : uclass.variables) {
					file << "\t\t\t.data<&" << uclass.class_name.c_str() << "::" << var.identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
					id++;
				}

				file << "\t\t\t.type(entt::type_hash<" << uclass.class_name.c_str() << ">::value()); \n\n";
			}
			file << "\t}\n\n";
		}

		// imgui display function
		{
			for (auto ustruct : m_structs) {
				
				file << "\tvoid display_properties(" << ustruct.struct_name.c_str() << "* script) {\n\n";
				for (auto var : ustruct.variables) {

					file << "\t\tUI::table_row(\"" << var.identifier.c_str() << "\", script->" << var.identifier.c_str() << ");\n";
				}
				file << "\t}\n\n";
			}

			for (auto uclass : m_classes) {

				file << "\tvoid display_properties(" << uclass.class_name.c_str() << "* script) {\n\n";
				for (auto var : uclass.variables) {

					file << "\t\tUI::table_row(\"" << var.identifier.c_str() << "\", script->" << var.identifier.c_str() << ");\n";
				}
				file << "\t}\n\n";
			}
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
				file << "(class_name == \"" << m_classes[x].class_name.c_str() << "\")";

				if (m_classes[x].parent_class_name.find("procedural_mesh_script") != std::string::npos) 
					file << "\n\t\t\tentity.add_procedural_mesh_component<" << m_classes[x].class_name.c_str() << ">(\"" << m_classes[x].class_name.c_str() << "\");\n";

				else if (m_classes[x].parent_class_name.find("entity_script") != std::string::npos)
					file << "\n\t\t\tentity.add_script_component<" << m_classes[x].class_name.c_str() << ">();\n";

				else {

					file << " {\n\n\t\t\t// could not auto detect parent class\n";
					file << "\t\t\tif constexpr (std::is_base_of_v<procedural_mesh_script, " << m_classes[x].class_name.c_str() << ">)\n";
					file << "\t\t\t\tentity.add_procedural_mesh_component<" << m_classes[x].class_name.c_str() << ">(\"" << m_classes[x].class_name.c_str() << "\");\n\n";

					file << "\t\t\telse if constexpr (std::is_base_of_v<entity_script, " << m_classes[x].class_name.c_str() << ">)\n";
					file << "\t\t\t\tentity.add_script_component<" << m_classes[x].class_name.c_str() << ">();\n\n";
					file << "\t\t}\n";
				}
							
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
		m_current_namespace = "";

		do {
			if (match(token_type::NAMESPACE)) {

				token namespaceToken = expect(token_type::IDENTIFIER);
				m_current_namespace = namespaceToken.m_lexeme;
				expect(token_type::LEFT_BRACKET);

			} else if (match(token_type::STRUCT_PROP)) {
				
				expect(token_type::LEFT_PAREN);
				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON);
				expect(token_type::STRUCT_KW);
				parse_struct();

			} else if (match(token_type::CLASS_PROP)) {
				
				expect(token_type::LEFT_PAREN);
				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON);
				expect(token_type::CLASS_KW);
				parse_class();

			} else if (match(token_type::RIGHT_BRACKET)) {
				
				m_current_namespace = "";

			} else {
				
				m_current_token++;
				m_Current_iter++;
			}
		} while (m_current_token < m_Tokens.size() && m_Current_iter->m_type != token_type::END_OF_FILE);
	}

	void script_parser::parse_class() {

		token class_type = expect(token_type::IDENTIFIER);
		std::string full_class_name = m_current_namespace.empty() ? class_type.m_lexeme : m_current_namespace + "::" + class_type.m_lexeme;
		std::string parent_class = "";

		if (match(token_type::COLON)) {
			while (!match(token_type::LEFT_BRACKET)) {
				if (m_Current_iter->m_type == token_type::IDENTIFIER) {

					if (!parent_class.empty())
						parent_class += "::"; // For nested namespaces
					
					parent_class += m_Current_iter->m_lexeme;
				}
				m_current_token++;
				m_Current_iter++;
			}
		} else
			expect(token_type::LEFT_BRACKET);

		PFF_class clazz = PFF_class{ full_class_name, parent_class, class_type.m_lexeme, m_full_filepath, std::list<PFF_variable>()};

		int level = 1;
		while (m_Current_iter->m_type != token_type::END_OF_FILE) {
			if (match(token_type::LEFT_BRACKET)) {
				level++;
			} else if (match(token_type::RIGHT_BRACKET)) {
				level--;
				if (level <= 0) {
					expect(token_type::SEMICOLON);
					break;
				}
			} else if (match(token_type::PROPERTY)) {
				expect(token_type::LEFT_PAREN);
				match(token_type::IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
				clazz.variables.push_back(parse_variable());
			} else {
				m_current_token++;
				m_Current_iter++;
			}
		}

		m_classes.push_back(clazz);
	}

	void script_parser::parse_struct() {

		token structType = expect(token_type::IDENTIFIER);
		expect(token_type::LEFT_BRACKET);
		std::string full_struct_name = m_current_namespace.empty() ? structType.m_lexeme : m_current_namespace + "::" + structType.m_lexeme;
		PFF_struct structure = PFF_struct{ full_struct_name, structType.m_lexeme, m_full_filepath, std::list<PFF_variable>() };
		int level = 1;
		while (m_Current_iter->m_type != token_type::END_OF_FILE) {

			if (match(token_type::LEFT_BRACKET))
				level++;

			else if (match(token_type::RIGHT_BRACKET)) {
				level--;
				if (level <= 1) {
					expect(token_type::SEMICOLON);
					break;
				}
			
			} else if (match(token_type::PROPERTY)) {
				expect(token_type::LEFT_PAREN);
				match(token_type::IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
				structure.variables.push_back(parse_variable());
			}
			// TODO: This might need another else statement like class parser
		}

		m_structs.push_back(structure);
	}

	PFF_variable script_parser::parse_variable() {

		std::vector<token> all_tokens_before_semi_colon = std::vector<token>();
		std::vector<token>::iterator current;
		int after_identifier_index = -1;

		do {
			current = m_Tokens.begin();
			std::advance(current, m_current_token);
			
			if (m_Current_iter->m_type == token_type::SEMICOLON && after_identifier_index == -1)
				after_identifier_index = static_cast<int>(all_tokens_before_semi_colon.size());
			else if (m_Current_iter->m_type == token_type::EQUAL)
				after_identifier_index = static_cast<int>(all_tokens_before_semi_colon.size());
			
			all_tokens_before_semi_colon.push_back(*current);
			m_current_token++;
			m_Current_iter++;
		} while (current->m_type != token_type::END_OF_FILE && current->m_type != token_type::SEMICOLON);

		token& variable_identifier = generate_error_token();

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

		token& type_identifier = generate_error_token();
		auto endIter = all_tokens_before_semi_colon.begin();
		std::advance(endIter, after_identifier_index - 1);
		for (auto iter = all_tokens_before_semi_colon.begin(); iter != endIter; iter++) {
			if (iter->m_type == token_type::IDENTIFIER) {
				type_identifier = *iter;
				break;
			}
		}

		return PFF_variable{ type_identifier.m_lexeme, variable_identifier.m_lexeme };
	}

	const token& script_parser::expect(token_type type) {

		if (m_Current_iter->m_type != type) {

			CORE_LOG(Error, "Error: Expected [" << type << "] but instead got [" << m_Current_iter->m_type << "] . line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");
			return s_error_token;
		}

		auto tokenToReturn = m_Current_iter;
		m_current_token++;
		m_Current_iter++;
		return *tokenToReturn;
	}

	bool script_parser::match(token_type type) {

		if (m_Current_iter->m_type == type) {
			m_current_token++;
			m_Current_iter++;
			return true;
		}

		return false;
	}
}
