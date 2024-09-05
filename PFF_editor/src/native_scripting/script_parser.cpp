
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


	std::string script_parser::generate_display_function(
		
		const std::string& name,const std::string& name_without_namespace, const std::string& specifiers, const std::filesystem::path& full_filepath, const std::list<PFF_variable>& variables) {
		std::stringstream file{};

		bool rebuild_on_change = (specifiers.find("rebuild_on_property_change") != std::string::npos);
		file << "\tvoid display_properties(" << name.c_str() << "* script) {\n\n";

		file << "\t\t// class specifiers [" << specifiers.c_str() << "]\n";

		if (rebuild_on_change)
			file << "\t\tbool changed = false;\n";

		// Sort propertyies based on category
		std::vector<sorting_category>		vars_sorted_by_category{};
		for (auto var : variables) {

			auto buffer = parse_specifiers_value(var.specifiers, "category", "data");
			bool founde = false;
			for (auto& category : vars_sorted_by_category) {

				if (category.category_name != buffer)
					continue;

				category.variables.push_back(var);
				founde = true;
				break;
			}

			if (!founde) {

				sorting_category sorting_buffer = { buffer, {} };
				sorting_buffer.variables.push_back(var);
				vars_sorted_by_category.push_back(sorting_buffer);
			}

		}

		// display sorted properties
		for (auto sorted_vars : vars_sorted_by_category) {
			file << "\t\tif (UI::begin_collapsing_header_section(\"" << sorted_vars.category_name << "\")) {\n";
			file << "\t\t\tUI::begin_table(\"entity_component\", false);\n";

			for (auto var : sorted_vars.variables) {

				if (rebuild_on_change)
					file << "\t\t\tchanged |= ";
				else
					file << "\t\t\t";

				auto var_lable = parse_specifiers_value(var.specifiers, "display_name", var.identifier);
				std::replace(var_lable.begin(), var_lable.end(), '_', ' ');
				var_lable += "##" + name + "::" + var.identifier;
				//std::replace(var_lable.begin(), var_lable.end(), ':', ' ');

				if (var.specifiers.find("UI_slider_drag_speed") == std::string::npos
					&& var.specifiers.find("min_value") == std::string::npos
					&& var.specifiers.find("max_value") == std::string::npos) {

					file << "UI::table_row(\"" << var_lable.c_str() << "\", script->" << var.identifier.c_str() << ");\n";
					continue;
				}

				file << "UI::table_row(\"" << var_lable.c_str() << "\", script->" << var.identifier.c_str() << ", ";
				file << "static_cast<f32>(" << parse_specifiers_value(var.specifiers, "UI_slider_drag_speed", "0.01f") << "), ";
				file << "static_cast<" << var.type << ">(" << parse_specifiers_value(var.specifiers, "min_value", "0.f") << "), ";
				file << "static_cast<" << var.type << ">(" << parse_specifiers_value(var.specifiers, "max_value", "1.f") << ")";
				file << ");\n"; //" << var.metadata << " \n";

			}

			file << "\t\t\tUI::end_table();\n";
			file << "\t\t}\n";
			file << "\t\tUI::end_collapsing_header_section();\n\n";

		}

		if (rebuild_on_change)
			file << "\t\tif (changed)\n\t\t\tscript->on_rebuild();\n";

		file << "\t}\n\n";

		return file.str();
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
				for (auto uvar : m_structs[x].variables)
					file << "\n\t\t\"" << m_structs[x].struct_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs,";
			}

			for (size_t x = 0; x < m_classes.size(); x++) {
				for (auto uvar : m_classes[x].variables)
					file << "\n\t\t\"" << m_classes[x].class_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs,";
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

		// append string_to_map						// TODO: create variable string to type id mapper...
		{
			file << "\tstd::map<std::string, entt::id_type> string_to_map = {\n";
			for (size_t x = 0; x < m_structs.size(); x++)
				file << "\n\t\t{ \"" << m_structs[x].struct_name.c_str() << "\", entt::type_hash<" << m_structs[x].struct_name.c_str() << ">::value() },";

			for (size_t x = 0; x < m_classes.size(); x++)
				file << "\n\t\t{ \"" << m_classes[x].class_name.c_str() << "\", entt::type_hash<" << m_classes[x].class_name.c_str() << ">::value() },";

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
				file << "\t\t\t.type(entt::type_hash<" << ustruct.struct_name.c_str() << ">::value()); \n\n";
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
			// TODO: update this to display the variables also sorted based on category (see below)
			/*for (auto ustruct : m_structs) {

				file << "\tvoid display_properties(" << ustruct.struct_name.c_str() << "* script) {\n\n";
				for (auto var : ustruct.variables) {

					file << "\t\tUI::table_row(\"" << var.identifier.c_str() << "\", script->" << var.identifier.c_str() << ");\n";
				}
				file << "\t}\n\n";
			}*/

			for (auto structure : m_structs)
				file << generate_display_function(structure.struct_name, structure.struct_name_without_namespace, structure.specifiers, structure.full_filepath, structure.variables);

			for (auto clazz : m_classes)
				file << generate_display_function(clazz.class_name, clazz.class_name_without_namespace, clazz.specifiers, clazz.full_filepath, clazz.variables);

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


	std::string script_parser::parse_specifiers_value(const std::string& specifiers, const std::string& key, std::string default_value) {

		size_t key_pos = specifiers.find(key);
		if (key_pos == std::string::npos) 
			return default_value;

		size_t value_start = specifiers.find(':', key_pos);
		if (value_start == std::string::npos) 
			return default_value;

		value_start++; // Move past the colon

		// Skip whitespace
		while (value_start < specifiers.length() && std::isspace(specifiers[value_start]))
			value_start++;

		size_t value_end = value_start;
		while (value_end < specifiers.length() && specifiers[value_end] != ',' && specifiers[value_end] != ')') {

			value_end++;
		}

		if (value_end != value_start && specifiers[value_end] == ' ')
			value_end--;

		// Extract the substring
		std::string result = specifiers.substr(value_start, value_end - value_start);

		// Remove leading and trailing double quotes
		if (result.length() >= 2 && result.front() == '"' && result.back() == '"') {
			result = result.substr(1, result.length() - 2);
		}

		// Trim any remaining whitespace
		result.erase(0, result.find_first_not_of(" \t\n\r\f\v"));
		result.erase(result.find_last_not_of(" \t\n\r\f\v") + 1);

		return result;
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

				std::string specifiers;
				parse_property_specifiers(specifiers);

				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON);
				expect(token_type::STRUCT_KW);
				parse_struct(specifiers);

			} else if (match(token_type::CLASS_PROP)) {
				
				expect(token_type::LEFT_PAREN);

				std::string specifiers;
				parse_property_specifiers(specifiers);

				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON);
				expect(token_type::CLASS_KW);
				parse_class(specifiers);

			} else if (match(token_type::RIGHT_BRACKET)) {
				
				m_current_namespace = "";

			} else
				advance();

		} while (m_current_token < m_Tokens.size() && m_Current_iter->m_type != token_type::END_OF_FILE);
	}

	void script_parser::parse_class(const std::string& specifiers) {

		token class_type = expect(token_type::IDENTIFIER);
		
		PFF_class clazz = {};
		clazz.class_name = m_current_namespace.empty() ? class_type.m_lexeme : m_current_namespace + "::" + class_type.m_lexeme;
		clazz.parent_class_name = "";
		clazz.class_name_without_namespace = class_type.m_lexeme;
		clazz.specifiers = specifiers;
		clazz.full_filepath = m_full_filepath;

		if (match(token_type::COLON)) {
			while (!match(token_type::LEFT_BRACKET)) {
				if (m_Current_iter->m_type == token_type::IDENTIFIER) {

					if (!clazz.parent_class_name.empty())
						clazz.parent_class_name += "::"; // For nested namespaces
					
					clazz.parent_class_name += m_Current_iter->m_lexeme;
				}
				m_current_token++;
				m_Current_iter++;
			}
		} else
			expect(token_type::LEFT_BRACKET);

		//PFF_class clazz = PFF_class{ full_class_name, parent_class, class_type.m_lexeme, m_full_filepath, std::list<PFF_variable>()};

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
				
				std::string specifiers;
				parse_property_specifiers(specifiers);

				expect(token_type::RIGHT_PAREN);
				match(token_type::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation

				PFF_variable var = parse_variable();
				var.specifiers = specifiers;
				clazz.variables.push_back(var);
			} else {
				m_current_token++;
				m_Current_iter++;
			}
		}

		m_classes.push_back(clazz);
	}


	void script_parser::advance_to_token(const token_type type) {

		while (m_Current_iter->m_type != token_type::END_OF_FILE && m_Current_iter->m_type != type)
			advance();
		advance();
	}

	void script_parser::parse_struct(const std::string& specifiers) {

		token structType = expect(token_type::IDENTIFIER);

		PFF_struct structure = {};
		structure.struct_name = m_current_namespace.empty() ? structType.m_lexeme : m_current_namespace + "::" + structType.m_lexeme;
		structure.struct_name_without_namespace = structType.m_lexeme;
		structure.specifiers = specifiers;
		structure.full_filepath = m_full_filepath;

		expect(token_type::LEFT_BRACKET);
		int level = 1;
		while (m_Current_iter->m_type != token_type::END_OF_FILE) {

			if (match(token_type::LEFT_BRACKET))
				level++;

			m_Current_iter->m_lexeme;		// DEV-ONLY

			if (m_Current_iter->m_type == token_type::IDENTIFIER && m_Current_iter->m_lexeme == structType.m_lexeme) {

				advance();
				expect(token_type::LEFT_PAREN);
				advance_to_token(token_type::RIGHT_PAREN);

				if (match(token_type::COLON)) {								// inline initalizer
					advance_to_token(token_type::LEFT_BRACKET);					// variable initalizer
					advance_to_token(token_type::RIGHT_BRACKET);				// implementation
				}

				else if (match(token_type::LEFT_BRACKET)) 					// implemanetation
					advance_to_token(token_type::RIGHT_BRACKET);

			}

			else if (match(token_type::RIGHT_BRACKET)) {
				level--;
				if (level <= 1) {
					expect(token_type::SEMICOLON);
					break;
				}
			
			} else if (match(token_type::PROPERTY)) {

				expect(token_type::LEFT_PAREN);
				std::string specifiers;
				parse_property_specifiers(specifiers);
				expect(token_type::RIGHT_PAREN);

				match(token_type::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
				PFF_variable var = parse_variable();
				var.specifiers = specifiers;
				structure.variables.push_back(var);

			} else {		// TODO: This might need another else statement like class parser
				m_current_token++;
				m_Current_iter++;
			}
		}

		m_structs.push_back(structure);
	}

	PFF_variable script_parser::parse_variable() {

		std::vector<token> all_tokens_before_semi_colon;
		int type_end_index = -1;
		int identifier_index = -1;
		
		// Collect all tokens until semicolon
		while (m_Current_iter->m_type != token_type::END_OF_FILE &&
			m_Current_iter->m_type != token_type::SEMICOLON) {

			all_tokens_before_semi_colon.push_back(*m_Current_iter);
			if (m_Current_iter->m_type == token_type::IDENTIFIER) {
				if ((m_Current_iter + 1)->m_type == token_type::EQUAL
					|| (m_Current_iter + 1)->m_type == token_type::LEFT_BRACKET) {

					identifier_index = (int)all_tokens_before_semi_colon.size() - 1;
					type_end_index = identifier_index - 1;
				}
			}

			m_current_token++;
			m_Current_iter++;
		}

		// Handle semicolon
		if (m_Current_iter->m_type == token_type::SEMICOLON) {
			m_current_token++;
			m_Current_iter++;
		}

		PFF_variable loc_pff_variable;

		// Parse type (including namespace)
		std::string type;
		for (int i = 0; i <= type_end_index; ++i) 
			type += all_tokens_before_semi_colon[i].m_lexeme;
		loc_pff_variable.type = type;

		PFF_variable error_variable{ "ERROR", "ERROR" };
		CORE_VALIDATE((identifier_index != -1), return error_variable, "", "Cannot find variable identifier. line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");

		loc_pff_variable.identifier = all_tokens_before_semi_colon[identifier_index].m_lexeme;
		return loc_pff_variable;
	}

	void script_parser::parse_property_specifiers(std::string& specifiers) {

		u32 nesting = 1;
		while (nesting > 0 && m_Current_iter != m_Tokens.end()) {

			if (m_Current_iter->m_type == token_type::LEFT_PAREN)
				nesting++;
			if (m_Current_iter->m_type == token_type::RIGHT_PAREN)
				nesting--;

			specifiers+= m_Current_iter->m_lexeme;
			
			// Add space after certain tokens for readability
			if (m_Current_iter->m_type == token_type::NUMBER ||
				m_Current_iter->m_type == token_type::COLON) {
				specifiers += " ";
			}

			m_current_token++;
			m_Current_iter++;

			if (m_Current_iter == m_Tokens.end()) {
				CORE_LOG(Error, "Reached end of tokens unexpectedly");
				break;
			}
		}

		// Remove the last character if it's a ')'
		if (!specifiers.empty() && specifiers.back() == ')') {
			specifiers.pop_back();

			m_current_token--;
			m_Current_iter--;
		}
	}

	const token& script_parser::expect(token_type type) {

		CORE_VALIDATE(m_Current_iter->m_type == type, return s_error_token, "",
			"Error: Expected [" << type << "] but instead got [" << m_Current_iter->m_type << "] . line [" << m_Current_iter->m_line << "], column [" << m_Current_iter->m_column << "]");

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
