
#include <util/pffpch.h>

#include <util/data_structures/path_manipulation.h>

#include "script_processor.h"
#include "script_parser.h"

#include "code_generator.h"

namespace PFF::code_generator {

	static std::filesystem::path visited_class_buffer[100];
	static int num_visited = 0;

	static bool visited_source_file(PFF_class clazz) {

		for (int i = 0; i < num_visited; i++) {
			if (clazz.full_filepath == visited_class_buffer[i])
				return true;
		}
		return false;
	}


	void generate_init_file_header(const std::vector<PFF_class>& classes, const std::filesystem::path& filepath) {

		//LOG(Trace, " generating init-files for project ");

		std::ostringstream source;
		source << "#pragma once\n\n";
		source << "#include <PFF.h>\n";										// main engine header
		source << "#define ENTT_STANDARD_CPP\n";
		source << "#include <entt/entt.hpp>\n\n";

		const auto base = script_processor::get_root_directory() / "generated";
		for (auto clazz : classes) {

			source << "#include \"" << PFF::util::extract_path_from_directory(clazz.full_filepath, "src").generic_string() << "\"\n";
			source << "#include \"" << PFF::util::extract_path_from_directory(clazz.full_filepath, "src").replace_extension().generic_string() + "-generated" + clazz.full_filepath.extension().string() << "\"\n";
		}
		source << R"(
#define RETURN_CHAR_ARRAY(array_name) 				if (count != nullptr) *count = sizeof(array_name) / sizeof(array_name[0]) - 1;		\
														return array_name;

)";
		source << "extern \"C\" namespace PFF::init {\n\n";

		// ------------------------------------------------------ static variables ------------------------------------------------------
		source << "\tconst char* procedural_mesh_scripts[] = {\n";
		for (auto clazz : classes) {
			if (clazz.parent_class_name.find("procedural_mesh_script") != std::string::npos)
				source << "\t\t\"" << clazz.class_name.c_str() << "\",\n";
		}
		source << "\t\tnullptr\n\t};\n\n";


		source << "\tconst char* scripts[] = {\n";
		for (auto clazz : classes) {
			if (clazz.parent_class_name.find("entity_script") != std::string::npos)
				source << "\t\t\"" << clazz.class_name.c_str() << "\",\n";
		}
		source << "\t\tnullptr\n\t};\n\n";


		// generate	simple getters
		{
			source << R"(
	PROJECT_API void init_scripts(entt::registry* registry);

	PROJECT_API void add_component(std::string class_name, PFF::entity entity);

	PROJECT_API void display_properties(std::string class_name, entity_script* script);

	PROJECT_API void serialize_script(std::string class_name, entity_script* script, serializer::yaml& serializer);

	PROJECT_API const char** get_all_procedural_mesh_scripts(u32* count) { RETURN_CHAR_ARRAY(procedural_mesh_scripts); }

	PROJECT_API const char** get_all_scripts(u32 * count) { RETURN_CHAR_ARRAY(scripts); }

)";
		}


		// Generate Init ImGui function
		/*{
			source << "\n";
			source << "\t\tPROJECT_API void InitImGui(void* ctx) {\n\n";
			source << "\t\t\tLOG(Info, \"Initializing ImGui\");\n";
			source << "\t\t\tImGui::SetCurrentContext((ImGuiContext*)ctx);\n";
			source << "\t\t}\n";
		}*/

		// Generate ImGui function
		/*{
			source << "\n";
			source << "\t\tPROJECT_API void ImGui(entt::registry& registryRef, Entity entity) {\n\n";

			num_visited = 0;
			for (auto clazz : classes) {
				if (!visited_source_file(clazz)) {
					std::string namespaceName = "reflect_" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::ImGui(entity, registryRef);\n";

					visited_class_buffer[num_visited] = clazz.full_filepath;
					num_visited++;
				}
			}
			source << "\t\t}\n";
		}*/

		// Generate Delete Scripts function
		/*{
			source << "\n";
			source << "\t\tPROJECT_API void DeleteScripts() {\n\n";
			source << "\t\t\tLOG(Info, \"Deleting Scripts\");\n";

			num_visited = 0;
			for (auto clazz : classes) {
				if (!visited_source_file(clazz)) {
					std::string namespaceName = "reflect_" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
					source << "\t\t\t" << namespaceName.c_str() << "::delete_scripts();\n";

					visited_class_buffer[num_visited] = clazz.full_filepath;
					num_visited++;
				}
			}
			source << "\t\t}\n";
		}*/

		source << "}\n";

		std::ofstream outStream(filepath.string());
		outStream << source.str().c_str();
		outStream.close();
	}

	void generate_init_file_implemenation(const std::vector<PFF_class>& classes, const std::filesystem::path& filepath) {

		//LOG(Trace, " generating init-files for project ");

		std::ostringstream source;
		source << "\n#include \"init.h\"\n";										// main engine header

		source << R"(
#define PFF_ADD_COMPONENT_GENERATED_MACRO(name)					for (auto str_class : reflect_##name::string_to_map) {					\
																	if (str_class.first != class_name) continue;						\
																	reflect_##name::add_component(class_name, entity);					\
																	return;}

#define PFF_DISPLAY_PROPERTIE_GENERATED_MACRO(name, cast)		for (auto str_class : reflect_##name::string_to_map) {					\
																	if (str_class.first != class_name) continue;						\
																	reflect_##name::display_properties((cast*) script);					\
																	return;}

#define PFF_SERIALIZE_SCRIPT_GENERATED_MACRO(name, cast)		for (auto str_class : reflect_##name::string_to_map) {					\
																	if (str_class.first != class_name) continue;						\
																	reflect_##name::serialize_script((cast*) script, serializer);		\
																	return;}

)";

		source << "extern \"C\" namespace PFF::init {\n\n";

		// Generate Init Scripts function
		{
			source << "\tvoid init_scripts(entt::registry* registry) {\n\n";
			source << "\t\tLOG(Info, \"PROJECT - initializing scripts\");\n\n";
			//
			//			source << R"(
			//		registry->storage<procedural_mesh_component>();
			//		LOG(Info, "Re-registered procedural_mesh_component with type ID: " << entt::type_hash<procedural_mesh_component>::value());
			//			
			//)";
			for (auto clazz : classes) {
				source << "\t\tregistry->storage<" << clazz.class_name.c_str() << ">();\n";
			}
			source << "\n";

			source << "\t\tImGui::SetCurrentContext(application::get().get_imgui_layer()->get_context());\n";
			num_visited = 0;
			for (auto clazz : classes) {
				if (!visited_source_file(clazz)) {
					std::string namespaceName = "reflect_" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
					source << "\t\t" << namespaceName.c_str() << "::init();\n";

					visited_class_buffer[num_visited] = clazz.full_filepath;
					num_visited++;
				}
			}
			source << "\n\t\tLOG(Info, \"PROJECT - successfully initalized scripts\");\n";

			source << "\t}\n\n";
		}

		// add_component function
		{
			source << "\tvoid add_component(std::string class_name, PFF::entity entity) {\n\n";
			source << "\t\tVALIDATE(entity.is_valid(), return, \"\", \"Invalid entity in script\");\n\n";
			num_visited = 0;
			for (auto clazz : classes) {
				if (!visited_source_file(clazz)) {

					source << "\t\tPFF_ADD_COMPONENT_GENERATED_MACRO(" << script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string()).c_str() << ");\n";
					visited_class_buffer[num_visited] = clazz.full_filepath;
					num_visited++;
				}
			}
			source << "\t}\n\n";
		}

		// display_properties function
		{
			source << "\tvoid display_properties(std::string class_name, entity_script* script) {\n\n";

			source << "\t\tif (!script) {\n";
			source << "\t\t\tImGui::Text(\"Script pointer is null\");\n";
			source << "\t\t\treturn;\n\t\t}\n\n";

			num_visited = 0;
			for (auto clazz : classes) {
				if (!visited_source_file(clazz)) {

					source << "\t\tPFF_DISPLAY_PROPERTIE_GENERATED_MACRO(" << script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string()).c_str() << ", " << clazz.class_name.c_str() << ");\n";
					visited_class_buffer[num_visited] = clazz.full_filepath;
					num_visited++;
				}
			}

			source << "\t}\n\n";
		}

		// serialize_script function
		{
			source << "\tvoid serialize_script(std::string class_name, entity_script* script, serializer::yaml& serializer) {\n\n";

			//source << "\t\tVALIDATE(!filepath.empty(), return, \"\", \"provided path is invalid[\" << filepath.generic_string() << \"]\");\n\n";

			num_visited = 0;
			for (auto clazz : classes) {
				if (!visited_source_file(clazz)) {

					source << "\t\tPFF_SERIALIZE_SCRIPT_GENERATED_MACRO(" << script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string()).c_str() << ", " << clazz.class_name.c_str() << ");\n";
					visited_class_buffer[num_visited] = clazz.full_filepath;
					num_visited++;
				}
			}

			source << "\t}\n\n";
		}
		
		source << "}\n";

		std::ofstream outStream(filepath.string());
		outStream << source.str().c_str();
		outStream.close();
	}

}
