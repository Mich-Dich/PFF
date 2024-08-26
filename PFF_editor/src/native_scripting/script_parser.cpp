
#include "util/pch_editor.h"

#include "script_parser.h"

namespace PFF {


	static Token							s_error_token = Token{ -1, -1, TokenType::ERROR_TYPE, "" };


	FORCEINLINE Token generate_error_token() { return Token{ -1, -1, TokenType::ERROR_TYPE, "" }; }

	FORCEINLINE static bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
	
	FORCEINLINE static bool is_alpha_numeric(char c) { return is_alpha(c) || (c >= '0' && c <= '9'); }
	
	FORCEINLINE static bool is_variable_friendly(char c) { return is_alpha_numeric(c) || c == '_'; }




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

		std::ostringstream file;
		file << "#pragma once\n\n";
		file << "#define ENTT_STANDARD_CPP\n";
		file << "#include <entt/core/hashed_string.hpp>\n";
		file << "#include <entt/core/type_info.hpp>\n";
		file << "#include <entt/meta/factory.hpp>\n";
		file << "#include <entt/meta/meta.hpp>\n";
		file << "#include <entt/meta/resolve.hpp>\n\n";

		// file << "#include <nlohmann/json.hpp>\n";			// TODO: Use own version
		file << "#include <imgui.h>\n";
		file << "#include <map>\n\n";
		//file << "#include \"ImGuiExtended.h\"\n";
		file << "#include \"../" << m_full_filepath.filename().string() << ".h\"\n\n";

		file << "namespace PFF {\n";
		file << "\tnamespace Reflect" << get_filename_as_class_name(m_full_filepath.filename().string()) << " \n\t{\n";

		file << "\t\tusing namespace entt::literals;\n";
		file << "\t\tbool initialized = false;\n\n";

		// Append ids as entt hash strings
		file << "\t\tstd::vector<entt::id_type> ids = {\n";

		int i = 0;
		for (auto ustruct : m_structs) {
			int j = 0;
			for (auto uvar : ustruct.variables) {
				if (i == m_structs.size() - 1 && j == ustruct.variables.size() - 1 && m_classes.size() == 0)
					file << "\t\t\t\"" << ustruct.struct_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << ustruct.struct_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs,\n";

				j++;
			}

			i++;
		}

		i = 0;
		for (auto uclass : m_classes) {
			int j = 0;
			for (auto uvar : uclass.variables) {
				if (i == m_classes.size() - 1 && j == uclass.variables.size() - 1)
					file << "\t\t\t\"" << uclass.class_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << uclass.class_name.c_str() << "::" << uvar.identifier.c_str() << "\"_hs,\n";

				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// Append debug names map
		file << "\t\tstd::map<entt::id_type, const char*> debugNames;\n";

		// Append stringToTypeMap
		file << "\t\tstd::map<std::string, entt::id_type> stringToMap;\n";

		// Create Init function ---------------------------------------------------------------
		file << "\t\tvoid init()\n";
		file << "\t\t{\n";
		file << "\t\t\tif (initialized) return;\n";
		file << "\t\t\tinitialized = true;\n\n";

		// Debug Names
		file << "\t\tdebugNames = \n\t\t{ \n";

		int id = 0;
		i = 0;
		for (auto ustruct : m_structs) {
			int j = 0;
			file << "\t\t\t{ entt::type_seq<" << ustruct.struct_name.c_str() << ">::value(), \"" << ustruct.struct_name.c_str() << "\"},\n";
			for (auto uvar : ustruct.variables) {
				if (i == m_structs.size() - 1 && j == ustruct.variables.size() - 1 && m_classes.size() == 0)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.identifier.c_str() << "\"}\n";
				else
					file << "\t\t\t{ids[" << id << "], \"" << uvar.identifier.c_str() << "\"},\n";
				id++;
				j++;
			}

			i++;
		}

		i = 0;
		for (auto uclass : m_classes) {
			int j = 0;
			file << "\t\t\t{ entt::type_seq<" << uclass.class_name.c_str() << ">::value(), \"" << uclass.class_name.c_str() << "\"},\n";
			for (auto uvar : uclass.variables) {
				if (i == m_classes.size() - 1 && j == uclass.variables.size() - 1)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.identifier.c_str() << "\"}\n";
				else
					file << "\t\t\t{ids[" << id << "], \"" << uvar.identifier.c_str() << "\"},\n";

				id++;
				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// String to map
		file << "\t\tstringToMap = \n\t\t{ \n";

		i = 0;
		for (auto ustruct : m_structs) {
			int j = 0;
			if (i == m_structs.size() - 1 && m_classes.size() == 0)
				file << "\t\t\t{ \"" << ustruct.struct_name.c_str() << "\", entt::type_seq<" << ustruct.struct_name.c_str() << ">::value() }\n";
			else
				file << "\t\t\t{ \"" << ustruct.struct_name.c_str() << "\", entt::type_seq<" << ustruct.struct_name.c_str() << ">::value() },\n";
			// TODO: Create variable string to type id mapper...
			i++;
		}

		i = 0;
		for (auto uclass : m_classes) {
			if (i == m_classes.size() - 1)
				file << "\t\t\t{ \"" << uclass.class_name.c_str() << "\", entt::type_seq<" << uclass.class_name.c_str() << ">::value() }\n";
			else
				file << "\t\t\t{ \"" << uclass.class_name.c_str() << "\", entt::type_seq<" << uclass.class_name.c_str() << ">::value() },\n";

			i++;
		}

		file << "\t\t};\n\n";

		// Meta initialization
		id = 0;
		for (auto ustruct : m_structs) {
			std::string camelCaseStructName = ustruct.struct_name;
			camelCaseStructName[0] = tolower(camelCaseStructName[0]);
			file << "\t\t\tauto " << camelCaseStructName.c_str() << "Factory = entt::meta<" << ustruct.struct_name.c_str() << ">()\n";

			for (auto var : ustruct.variables) {
				file << "\t\t\t\t.data<&" << ustruct.struct_name.c_str() << "::" << var.identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}

		for (auto uclass : m_classes) {
			std::string camelCaseClassName = uclass.class_name;
			camelCaseClassName[0] = tolower(camelCaseClassName[0]);
			file << "\t\t\tauto " << camelCaseClassName.c_str() << "Factory = entt::meta<" << uclass.class_name.c_str() << ">()\n";

			for (auto var : uclass.variables) {
				file << "\t\t\t\t.data<&" << uclass.class_name.c_str() << "::" << var.identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}
		file << "\t\t}\n";

		// Create AddComponent function
		file << "\t\tvoid add_component(std::string class_name, entt::entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";

		i = 0;
		for (auto uclass : m_classes) {
			if (i == 0)
				file << "\t\t\tif";
			else
				file << "\t\t\telse if";
			file << "(className == \"" << uclass.class_name.c_str() << "\")\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\tEntity e = NEntity::CreateEntity(entity);\n";
			file << "\t\t\t\tNEntity::AddComponent<" << uclass.class_name.c_str() << ">(e);\n";
			file << "\t\t\t}\n";
			i++;
		}
		file << "\t\t}\n";

		// SaveScript function
		file << "\n"
			"		void SaveScript(entt::meta_any any, json& j, Entity entity)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			\n"
			"			int size = j[\"Components\"].size();\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"			json compJson;\n"
			"			compJson[typeName] = {};\n"
			"			compJson[typeName][\"Entity\"] = NEntity::GetID(entity);\n"
			"\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"\n"
			"				if (data.type().is_floating_point())\n"
			"				{\n"
			"					entt::meta_handle handle = entt::meta_handle(any);\n"
			"					float val = data.get(handle).cast<float>();\n"
			"					compJson[typeName][name->second] = val;\n"
			"				}\n"
			"				else if (data.type().is_integral())\n"
			"				{\n"
			"					entt::meta_handle handle = entt::meta_handle(any);\n"
			"					int val = data.get(handle).cast<int>();\n"
			"					compJson[typeName][name->second] = val;\n"
			"				}\n"
			"			}\n"
			"\n"
			"			j[\"Components\"][size] = compJson;\n"
			"		}\n"
			"\n";

		// Save Scripts function
		file << "\t\tvoid SaveScripts(json& j, entt::registry& registry, SceneData* sceneData)\n";
		file << "\t\t{\n";

		for (auto uclass : m_classes) {
			file << "\t\t\t{\n";

			file << "\t\t\t\tauto view = registry.view<" << uclass.class_name.c_str() << ">();\n";
			file << "\t\t\t\tfor (auto entity : view)\n";
			file << "\t\t\t\t{\n";
			file << "\t\t\t\t\tauto comp = NEntity::GetComponent<" << uclass.class_name.c_str() << ">(NEntity::CreateEntity(entity));\n";
			file << "\t\t\t\t\tentt::meta_any any = { comp };\n";
			file << "\t\t\t\t\tSaveScript(any, j, Entity{ entity });\n";
			file << "\t\t\t\t}\n";

			file << "\t\t\t}\n";
		}

		file << "\t\t}\n";

		// Load Script function
		file << "\n"
			"		void LoadScript(entt::meta_any any, entt::meta_handle handle, json& j)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"\n"
			"				if (data.type().is_floating_point())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.set<float>(handle, j[typeName][name->second]);\n"
			"				}\n"
			"				else if (data.type().is_integral())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.set<int>(handle, j[typeName][name->second]);\n"
			"				}\n"
			"			}\n"
			"		}\n"
			"\n";

		// Try Load Script function
		file << "\t\tvoid TryLoad(json& j, Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tjson::iterator it = j.begin();\n";
		file << "\t\t\tentt::entity e = entity.Handle;\n";
		file << "\t\t\tif (!registry.valid(e))\n";
		file << "\t\t\t{\n";
		file << "\t\t\t\tLogger::Error(\"Invalid entity, entity does not exist.\");\n";
		file << "\t\t\t\treturn;\n";
		file << "\t\t\t}\n";
		file << "\n";

		i = 0;
		for (auto uclass : m_classes) {
			if (i == 0)
				file << "\t\t\tif";
			else
				file << "\t\t\telse if";

			file << " (it.key() == \"" << uclass.class_name.c_str() << "\")\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.class_name.c_str() << "& comp = NEntity::AddComponent<" << uclass.class_name.c_str() << ">(entity);\n";
			file << "\t\t\t\tLoadScript({ comp }, comp, j);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// ImGuiAny function
		file << "\n"
			"		void ImGuiAny(entt::meta_any any, entt::meta_handle handle)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"\n"
			"			if (ImGui::CollapsingHeader(typeName))\n"
			"			{\n"
			"				CImGui::BeginCollapsingHeaderGroup();\n"
			"				for (auto data : typeData.data())\n"
			"				{\n"
			"					auto name = debugNames.find(data.id());\n"
			"					auto type = debugNames.find(data.type().id());\n"
			"					if (name == debugNames.end() && type == debugNames.end())\n"
			"						continue;\n"
			"\n"
			"					if (data.type().is_floating_point())\n"
			"					{\n"
			"						float* val = (float*)data.get(handle).data();\n"
			"						CImGui::UndoableDragFloat(name->second, *val);\n"
			"					}\n"
			"					else if (data.type().is_integral())\n"
			"					{\n"
			"						int* val = (int*)data.get(handle).data();\n"
			"						CImGui::UndoableDragInt(name->second, *val);\n"
			"					}\n"
			"				}\n"
			"				CImGui::EndCollapsingHeaderGroup();\n"
			"			}\n"
			"		}\n"
			"\n";

		// ImGui function
		file << "\t\tvoid ImGui(Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::entity e = entity.Handle;\n";
		file << "\t\t\tif (!registry.valid(e)) return;\n";

		i = 0;
		for (auto uclass : m_classes) {
			file << "\t\t\tif (entity::has_component<" << uclass.class_name.c_str() << ">())\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.class_name.c_str() << "& comp = entity.get_component<" << uclass.class_name.c_str() << ">();\n";
			file << "\t\t\t\tImGuiAny({ comp }, comp);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// Delete Scripts function
		file << "\t\tvoid delete_scripts()\n";
		file << "\t\t{\n";

		for (auto uclass : m_classes) {
			file << "\t\t\t{\n";

			file << "\t\t\t\tNEntity::Clear<" << uclass.class_name.c_str() << ">();\n";

			file << "\t\t\t}\n";
		}

		file << "\t\t}\n";

		// Tabs function
		file << "\n"
			"		int tabs = 0;\n"
			"		void printTabs()\n"
			"		{\n"
			"			for (int i = 0; i < tabs; i++)\n"
			"			{\n"
			"				printf(\"\\t\");\n"
			"			}\n"
			"		}\n"
			"\n";

		// Debug printAny function
		file << "\n"
			"		void debugPrintAny(entt::meta_any any)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"		\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				tabs++;\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"		\n"
			"				if (data.type().is_class())\n"
			"				{\n"
			"					printTabs();\n"
			"					printf(\"%s<%s>\\n\", name->second, type->second);\n"
			"					tabs++;\n"
			"					entt::meta_handle handle = entt::meta_handle(any);\n"
			"					entt::meta_any resolvedData = data.get(handle);\n"
			"					debugPrintAny(resolvedData);\n"
			"					tabs--;\n"
			"				}\n"
			"				else\n"
			"				{\n"
			"					if (data.type().is_floating_point())\n"
			"					{\n"
			"						printTabs();\n"
			"						entt::meta_handle handle = entt::meta_handle(any);\n"
			"						float val = data.get(handle).cast<float>();\n"
			"						printf(\"%s<float>: %2.3f\\n\", name->second, val);\n"
			"					}\n"
			"		 			else if (data.type().is_integral())\n"
			"					{\n"
			"						printTabs();\n"
			"						entt::meta_handle handle = entt::meta_handle(any);\n"
			"						int val = data.get(handle).cast<int>();\n"
			"						printf(\"%s<int>: %d\\n\", name->second, val);\n"
			"					}\n"
			"				}\n"
			"				tabs--;\n"
			"			}\n"
			"		}\n";

		// Begin template print functions
		file << "\t\ttemplate<typename T>\n";
		file << "\t\tvoid debugPrint(const T& obj)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::meta_any any = entt::meta_any{ obj };\n";
		file << "\t\t\tdebug_print_any(any);\n";
		file << "\t\t}\n";

		// End namespace
		file << "\t}\n";

		// End namespace
		file << "}\n";

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
	
		m_CurrentToken = 0;
		m_CurrentIter = m_Tokens.begin();
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
				m_CurrentToken++;
				m_CurrentIter++;
			}
		} while (m_CurrentToken < m_Tokens.size() && m_CurrentIter->m_Type != TokenType::END_OF_FILE);
	}

	void script_parser::ParseClass() {

		Token classType = Expect(TokenType::IDENTIFIER);

		if (Match(TokenType::COLON)) {
			while (!Match(TokenType::LEFT_BRACKET)) {
				m_CurrentToken++;
				m_CurrentIter++;
			}
		} else {
			Expect(TokenType::LEFT_BRACKET);
		}

		PFF_class clazz = PFF_class{ classType.m_Lexeme, m_FullFilepath, std::list<PFF_variable>() };

		int level = 1;
		while (m_CurrentIter->m_Type != TokenType::END_OF_FILE) {
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
				m_CurrentToken++;
				m_CurrentIter++;
			}
		}

		m_Classes.push_back(clazz);
	}

	void script_parser::ParseStruct() {

		Token structType = Expect(TokenType::IDENTIFIER);
		Expect(TokenType::LEFT_BRACKET);

		PFF_struct structure = PFF_struct{ structType.m_Lexeme, m_FullFilepath, std::list<PFF_variable>() };

		int level = 1;
		while (m_CurrentIter->m_Type != TokenType::END_OF_FILE) {
			if (Match(TokenType::LEFT_BRACKET)) {
				level++;
			} else if (Match(TokenType::RIGHT_BRACKET)) {
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

		m_Structs.push_back(structure);
	}

	PFF_variable script_parser::ParseVariable() {

		std::vector<Token> allTokensBeforeSemiColon = std::vector<Token>();
		std::vector<Token>::iterator current;
		int afterIdentifierIndex = -1;

		do {
			current = m_Tokens.begin();
			std::advance(current, m_CurrentToken);
			if (m_CurrentIter->m_Type == TokenType::SEMICOLON && afterIdentifierIndex == -1) {
				afterIdentifierIndex = static_cast<int>(allTokensBeforeSemiColon.size());
			} else if (m_CurrentIter->m_Type == TokenType::EQUAL) {
				afterIdentifierIndex = static_cast<int>(allTokensBeforeSemiColon.size());
			}
			allTokensBeforeSemiColon.push_back(*current);
			m_CurrentToken++;
			m_CurrentIter++;
		} while (current->m_Type != TokenType::END_OF_FILE && current->m_Type != TokenType::SEMICOLON);

		Token& variableIdentifier = generate_error_token();
		if (afterIdentifierIndex != -1) {
			if (afterIdentifierIndex <= 1) 
				CORE_LOG(Error, "Weird equal sign placement. line [" << m_CurrentIter->m_Line << "], column [" << m_CurrentIter->m_Column << "]");
			
			auto it = allTokensBeforeSemiColon.begin();
			std::advance(it, afterIdentifierIndex - 1);
			variableIdentifier = *it;
		} else {
			CORE_LOG(Error, "Cannot find variable identifier. line [" << m_CurrentIter->m_Line << "], column [" << m_CurrentIter->m_Column << "]");
			return PFF_variable{ "ERROR", variableIdentifier.m_Lexeme };
		}

		Token& typeIdentifier = generate_error_token();
		auto endIter = allTokensBeforeSemiColon.begin();
		std::advance(endIter, afterIdentifierIndex - 1);
		for (auto iter = allTokensBeforeSemiColon.begin(); iter != endIter; iter++) {
			if (iter->m_Type == TokenType::IDENTIFIER) {
				typeIdentifier = *iter;
				break;
			}
		}

		return PFF_variable{ typeIdentifier.m_Lexeme, variableIdentifier.m_Lexeme };
	}

	const Token& script_parser::Expect(TokenType type) {

		if (m_CurrentIter->m_Type != type) {

			CORE_LOG(Error, "Error: Expected [" << type << "] but instead got [" << m_CurrentIter->m_Type << "] . line [" << m_CurrentIter->m_Line << "], column [" << m_CurrentIter->m_Column << "]");
			return s_error_token;
		}

		auto tokenToReturn = m_CurrentIter;
		m_CurrentToken++;
		m_CurrentIter++;
		return *tokenToReturn;
	}

	bool script_parser::Match(TokenType type) {

		if (m_CurrentIter->m_Type == type) {
			m_CurrentToken++;
			m_CurrentIter++;
			return true;
		}

		return false;
	}
}
