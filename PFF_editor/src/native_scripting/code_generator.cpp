
#include "util/pch_editor.h"

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




	void generate_init_file(const std::vector<PFF_class>& classes, const std::filesystem::path& filepath) {

		std::ostringstream source;

		source << "#pragma once\n";
		source << "#include <PFF.h>\n";										// main engine header

		const std::filesystem::path base = filepath.parent_path();
		for (auto clazz : classes) {
			const std::filesystem::path otherPath = clazz.full_filepath;
			source << "#include \"" << std::filesystem::relative(otherPath, base).generic_string().c_str() << "\"\n";

			std::string genFilename = clazz.full_filepath.filename().string() + "-generated" + clazz.full_filepath.extension().string();
			const std::filesystem::path otherGenCPath = clazz.full_filepath.parent_path() / "generated" / genFilename;
			const std::filesystem::path otherGenPath = otherGenCPath.parent_path();
			source << "#include \"" << std::filesystem::relative(otherGenPath, base).generic_string().c_str() << "\"\n\n";
		}

		source << "\n";
		source << "#define ENTT_STANDARD_CPP\n";
		source << "#include <entt.hpp>\n\n";

		source << "extern \"C\" namespace PFF {\n";
		source << "\textern \"C\" namespace Init {\n\n";

		// Init Component Id's -------------------------------------------------------------------------------------------
		source << "\t\tstatic void InitComponentIds(SceneData & scene)\n";
		source << "\t\t{\n";

		for (auto clazz : classes) {
			source << "\t\t\tNEntity::RegisterComponentType<" << clazz.class_name.c_str() << ">();\n";
		}

		source << "\t\t}\n";

		// AddComponent function
		source << "\t\textern \"C\" PFF_SCRIPT void AddComponent(entt::registry& registryRef, std::string className, entt::entity entity)\n";
		source << "\t\t{\n";
		source << "\t\t\tLog::Assert(registryRef.valid(entity), \"Invalid entity in script\");\n";

		num_visited = 0;
		for (auto clazz : classes) {
			if (!visited_source_file(clazz)) {
				std::string namespaceName = "Reflect" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
				source << "\t\t\tfor (auto strClass : " << namespaceName.c_str() << "::stringToMap)\n";
				source << "\t\t\t{\n";
				source << "\t\t\t\tif (strClass.first == className)\n";
				source << "\t\t\t\t{\n";
				source << "\t\t\t\t\t" << namespaceName << "::AddComponent(className, entity, registryRef);\n";
				source << "\t\t\t\t\treturn;\n";
				source << "\t\t\t\t}\n";
				source << "\t\t\t}\n";

				visited_class_buffer[num_visited] = clazz.full_filepath;
				num_visited++;
			}
		}

		source << "\t\t}\n\n";

		// Generate UpdateScripts function
		source << "\t\textern \"C\" PFF_SCRIPT void UpdateScripts(entt::registry& registryRef, float dt)\n";
		source << "\t\t{\n";
		for (auto clazz : classes) {
			source << "\t\t\t{\n";
			source << "\t\t\t\tauto view = registryRef.view<" << clazz.class_name.c_str() << ">();\n";
			source << "\t\t\t\tfor (auto entity : view)\n";
			source << "\t\t\t\t{\n";
			source << "\t\t\t\t\tauto comp = registryRef.get<" << clazz.class_name.c_str() << ">(entity);\n";
			source << "\t\t\t\t\tcomp.Update(NEntity::CreateEntity(entity), dt);\n";
			source << "\t\t\t\t}\n";
			source << "\t\t\t}\n";
		}
		source << "\t\t}\n";

		// Generate EditorUpdateScripts function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void EditorUpdateScripts(entt::registry& registryRef, float dt)\n";
		source << "\t\t{\n";
		for (auto clazz : classes) {
			source << "\t\t\t{\n";
			source << "\t\t\t\tauto view = registryRef.view<" << clazz.class_name.c_str() << ">();\n";
			source << "\t\t\t\tfor (auto entity : view)\n";
			source << "\t\t\t\t{\n";
			source << "\t\t\t\t\tauto comp = registryRef.get<" << clazz.class_name.c_str() << ">(entity);\n";
			source << "\t\t\t\t\tcomp.EditorUpdate(NEntity::CreateEntity(entity), dt);\n";
			source << "\t\t\t\t}\n";
			source << "\t\t\t}\n";
		}
		source << "\t\t}\n";

		// Generate NotifyBeginContact function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void NotifyBeginContact(Entity a, Entity b)\n";
		source << "\t\t{\n";
		for (auto clazz : classes) {
			source << "\t\t\t{\n";
			source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.class_name.c_str() << ">(a))\n";
			source << "\t\t\t\t{\n";
			source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.class_name.c_str() << ">(a).BeginContact(b);\n";
			source << "\t\t\t\t}\n";
			source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.class_name.c_str() << ">(b))\n";
			source << "\t\t\t\t{\n";
			source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.class_name.c_str() << ">(b).BeginContact(a);\n";
			source << "\t\t\t\t}\n";
			source << "\t\t\t}\n";
		}
		source << "\t\t}\n";

		// Generate NotifyEndContact function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void NotifyEndContact(Entity a, Entity b)\n";
		source << "\t\t{\n";
		for (auto clazz : classes) {
			source << "\t\t\t{\n";
			source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.class_name.c_str() << ">(a))\n";
			source << "\t\t\t\t{\n";
			source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.class_name.c_str() << ">(a).EndContact(b);\n";
			source << "\t\t\t\t}\n";
			source << "\t\t\t\tif (NEntity::HasComponent<" << clazz.class_name.c_str() << ">(b))\n";
			source << "\t\t\t\t{\n";
			source << "\t\t\t\t\tNEntity::GetComponent<" << clazz.class_name.c_str() << ">(b).EndContact(a);\n";
			source << "\t\t\t\t}\n";
			source << "\t\t\t}\n";
		}
		source << "\t\t}\n";

		// Generate SaveScript function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void SaveScripts(entt::registry& registryRef, json& j, SceneData* sceneData)\n";
		source << "\t\t{\n";
		source << "\t\t\tLog::Info(\"Saving scripts\");\n";
		num_visited = 0;
		for (auto clazz : classes) {
			if (!visited_source_file(clazz)) {
				std::string namespaceName = "Reflect" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
				source << "\t\t\t" << namespaceName.c_str() << "::SaveScripts(j, registryRef, sceneData);\n";

				visited_class_buffer[num_visited] = clazz.full_filepath;
				num_visited++;
			}
		}
		source << "\t\t}\n";

		// Generate Load Scripts function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void LoadScript(entt::registry& registryRef, const json& j, Entity entity)\n";
		source << "\t\t{\n";
		num_visited = 0;
		for (auto clazz : classes) {
			if (!visited_source_file(clazz)) {
				std::string namespaceName = "Reflect" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
				source << "\t\t\t" << namespaceName.c_str() << "::TryLoad(j, entity, registryRef);\n";

				visited_class_buffer[num_visited] = clazz.full_filepath;
				num_visited++;
			}
		}
		source << "\t\t}\n";

		// Generate Init Scripts function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void InitScripts(SceneData* sceneData)\n";
		source << "\t\t{\n";
		source << "\t\t\tLog::Info(\"Initializing scripts\");\n";
		source << "\t\t\tInitComponentIds(*sceneData);\n";

		num_visited = 0;
		for (auto clazz : classes) {
			if (!visited_source_file(clazz)) {
				std::string namespaceName = "Reflect" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
				source << "\t\t\t" << namespaceName.c_str() << "::Init();\n";

				visited_class_buffer[num_visited] = clazz.full_filepath;
				num_visited++;
			}
		}
		source << "\t\t}\n";

		// Generate Init ImGui function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void InitImGui(void* ctx)\n";
		source << "\t\t{\n";
		source << "\t\t\tLog::Info(\"Initializing ImGui\");\n";
		source << "\t\t\tImGui::SetCurrentContext((ImGuiContext*)ctx);\n";
		source << "\t\t}\n";

		// Generate ImGui function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void ImGui(entt::registry& registryRef, Entity entity)\n";
		source << "\t\t{\n";

		num_visited = 0;
		for (auto clazz : classes) {
			if (!visited_source_file(clazz)) {
				std::string namespaceName = "Reflect" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
				source << "\t\t\t" << namespaceName.c_str() << "::ImGui(entity, registryRef);\n";

				visited_class_buffer[num_visited] = clazz.full_filepath;
				num_visited++;
			}
		}
		source << "\t\t}\n";

		// Generate Delete Scripts function
		source << "\n";
		source << "\t\textern \"C\" PFF_SCRIPT void DeleteScripts()\n";
		source << "\t\t{\n";
		source << "\t\t\tLog::Info(\"Deleting Scripts\");\n";

		num_visited = 0;
		for (auto clazz : classes) {
			if (!visited_source_file(clazz)) {
				std::string namespaceName = "Reflect" + script_parser::get_filename_as_class_name(clazz.full_filepath.filename().string());
				source << "\t\t\t" << namespaceName.c_str() << "::DeleteScripts();\n";

				visited_class_buffer[num_visited] = clazz.full_filepath;
				num_visited++;
			}
		}
		source << "\t\t}\n";

		source << "\t}\n";
		source << "}\n";

		std::ofstream outStream(filepath.string());
		outStream << source.str().c_str();
		outStream.close();
	}











	// TODO: should only be used on project creation/update

	void generate_premake_file(const std::filesystem::path& filepath, const std::string_view project_name) {

		const std::filesystem::path engineSource = std::filesystem::path("C:\\CustomGameEngine\\PFF").make_preferred();
		const std::filesystem::path engineExeDir = std::filesystem::path("C:\\CustomGameEngine\\PFF\\bin\\Debug-windows-x86_64").make_preferred();

		std::ostringstream stream;
		stream << "include \"" << (engineSource / "dependencies.lua").generic_string() << "\"\n";
		stream << R"(
workspace "PFF_project"
	architecture "x64"
	configurations { "Debug", "Release" }
	flags { "MultiProcessorCompile" }
)";

		stream << "\tstartproject \"" << project_name << "\"\n\n";
		stream << "\tproject \"" << project_name << "\"\n";

		stream << R"(		kind "SharedLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

	targetdir("bin/%{prj.name}")
	objdir("bin-int/%{prj.name}")

	--------------------------------------TODO: make PCH an option in the editor--------------------------------------
	--pchheader "pch.h"
	--pchsource "src/pch.cpp"

	files
	{
		"**.h",
		"**.hpp",
		"**.cpp",
	}

	includedirs
	{
		"%{prj.name}/content",
)";
		stream << 
			"		\"" << engineSource.generic_string() << "/PFF/src\",\n"
			"		\"" << engineSource.generic_string() << "/%{vendor_path.entt}\",\n"
			"		\"" << engineSource.generic_string() << "/%{vendor_path.glm}\",\n"
			"		\"" << engineSource.generic_string() << "/%{vendor_path.ImGui}\",\n"
			"		\"C:/VulkanSDK/1.3.250.1/Include\",\n"
			"	}\n";

		stream << R"(
	symbolspath '$(OutDir)$(TargetName)-$([System.DateTime]::Now.ToString("HH_mm_ss_fff")).pdb'

	libdirs 
	{
)";

		stream << "\t\t\"" << (engineExeDir / "PFF").generic_string() << "\",\n";
		stream << "\t\t\"" << (engineExeDir / "imgui").generic_string() << "\",\n";
		stream << R"(	}

	links
	{
		"PFF"
	}

	defines "PFF_PROJECT"

	filter "system:windows"
		defines "PFF_PLATFORM_WINDOWS"
		systemversion "latest"

	filter "configurations:Debug"
		defines "PROJECT_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PROJECT_RELEASE"
		runtime "Release"
		optimize "on"
)";

		std::ofstream outStream(filepath.string());
		outStream << stream.str().c_str();
		outStream.close();
	}

	void generate_build_file(const std::filesystem::path& filepath, const std::filesystem::path& premakeFilepath) {

		std::filesystem::path projectPremakeLua = filepath.parent_path() / "premake5.lua";

		std::ostringstream stream;
		stream << R"(
@echo OFF
SETLOCAL EnableExtensions DisableDelayedExpansion
for /F %%a in ('echo prompt $E ^| cmd') do (
  set "ESC=%%a"
)
SETLOCAL EnableDelayedExpansion


echo.
echo ======== Building PFF Project
echo.

echo -------- Compile VS 2022 Solution:)";

		stream << "\ncall " << premakeFilepath.generic_string() << " vs2022";
		stream << R"(
echo.

echo -------- Compile Result:
if %errorlevel% neq 0 (
    echo !ESC![31mBUILD FAILED!ESC![0m the premake script encountered [%errorlevel%] errors
) else (
    echo !ESC![32mBUILD SUCCESSFULL!ESC![0m
)

@echo on
)";

		std::ofstream outStream(filepath.string());
		outStream << stream.str().c_str();
		outStream.close();
	}
}
