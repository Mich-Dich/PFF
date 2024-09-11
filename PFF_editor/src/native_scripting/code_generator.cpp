
#include "util/pch_editor.h"

#include "PFF_editor.h"
#include "code_generator.h"

namespace PFF::code_generator {
	

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

	targetdir("bin/%{prj.name})";
		stream << PFF_PROJECT_TEMP_DLL_PATH << "\")";
		stream << R"(
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
		"content",
		"src",
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
		stream << "\t\t\"" << (engineExeDir / "vendor/imgui").generic_string() << "\",\n";
		stream << R"(	}

	links
	{
		"PFF",
		"ImGui"
	}

	defines "PFF_PROJECT"

    prebuildcommands {
)";
		stream << "\t\t\"cd " << (engineExeDir / "PFF").generic_string() << " && \" ..\n";
		stream << "\t\t\"" << (engineExeDir / "PFF_helper" / "PFF_helper.exe").generic_string() << " 0 0 " << application::get().get_project_path().generic_string() << "\",";
		stream <<R"(
    }

	filter "system:windows"
		defines "PFF_PLATFORM_WINDOWS"
		systemversion "latest"
		buildcommands { "del /S *.pdb" }

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

REM Change to the project directory
cd /d %~dp0

echo.
if "%1" == "compile" (

	echo =================================== Compiling PFF Project ===================================

	echo	-------------------------------- Set up Visual Studio environment ------------------------
	call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64

	echo	---------------------------------------- compile -----------------------------------------
	msbuild PFF_project.sln /p:Configuration=Debug /p:Platform=x64
) else (
	echo =================================== Building PFF Project Solution ===========================)";
		stream << "\n\tcall " << premakeFilepath.generic_string() << " vs2022";
		stream << R"(
)
echo.

echo ========================================== Result ===========================================
if %errorlevel% neq 0 (
    echo !ESC![31mBUILD FAILED!ESC![0m the premake script encountered [%errorlevel%] errors
) else (
    echo !ESC![32mBUILD SUCCESSFULL!ESC![0m
)

@echo on
)";

		io_handler::write_to_file(stream.str().c_str(), filepath);

		//std::ofstream outStream(filepath.string());
		//outStream << stream.str().c_str();
		//outStream.close();
	}


}
