
#include "util/pch_editor.h"

#include "PFF_editor.h"
#include "code_generator.h"

namespace PFF::code_generator {
	

	// TODO: should only be used on project creation/update

	void generate_premake_file(const std::filesystem::path& filepath, const std::string_view project_name) {

		LOG(Trace, "generate_premake_file(filepath = " << filepath << ", project_name = " << project_name << ")")

		const std::filesystem::path engine_install_dir = std::filesystem::path(ENGINE_INSTALL_DIR).make_preferred();

		std::ostringstream stream;
		stream << "include \"" << (engine_install_dir / "dependencies.lua").generic_string() << "\"\n";
		stream << "\nworkspace \"" << project_name << "\"\n";
		stream << R"(	architecture "x64"
	configurations { "Debug", "Release" }
	flags { "MultiProcessorCompile" }
)";

		stream << "\tstartproject \"" << project_name << "\"\n\n";
		stream << "\tproject \"" << project_name << "\"\n";

		stream << R"(		location "metadata/project_files"					-- Set the location for for workspace (make files / solution files)
		kind "SharedLib"									-- for development (needs to be StaticLib in exported game)
		language "C++"
		cppdialect "C++20"
		staticruntime "on"

	targetdir("bin/%{prj.name})";
		stream << PFF_PROJECT_TEMP_DLL_PATH << "\")";
		stream << R"(
	objdir("bin-int/%{prj.name}")

	--------------------------------------TODO: make PCH an option in the editor--------------------------------------
	--pchheader "pch.h"
	--pchsource "src/pch.cpp"

	files {
	
		"generated/**.h",
		"generated/**.hpp",
		"generated/**.cpp",
		"src/**.h",
		"src/**.hpp",
		"src/**.cpp",
	}

	includedirs {

		"content",
		"src",
)";
		stream << 
			"		\"" << engine_install_dir.generic_string() << "/PFF/src\",\n"
			"		\"" << engine_install_dir.generic_string() << "/vendor/entt\",\n"
			"		\"" << engine_install_dir.generic_string() << "/vendor/glm\",\n"
			"		\"" << engine_install_dir.generic_string() << "/vendor/imgui\",\n"
			"		\"%{IncludeDir.Vulkan}\",\n"
			"	}\n";

		stream << R"(
	symbolspath '$(OutDir)$(TargetName)-$([System.DateTime]::Now.ToString("HH_mm_ss_fff")).pdb'
)";

		stream << "\tdebugdir(\"" << (engine_install_dir / "PFF_editor").generic_string() << "\")\n";
		stream << "\t-- for passing arguments to game engine, use:								debugargs { \"arg1\", \"arg2\" }\n";
		stream << R"(
	libdirs {

)";

		stream << "\t\t\"" << (engine_install_dir / "PFF").generic_string() << "\",\n";
		stream << "\t\t\"" << (engine_install_dir / "vendor/imgui").generic_string() << "\",\n";
		stream << "\t\t\"" << (engine_install_dir / "vendor/glfw").generic_string() << "\",\n";
		stream << R"(	}

	links {

		"PFF",
		"imgui",
		"glfw3",
		"vulkan"
	}

	defines "PFF_PROJECT"

	cleancommands{
		"{RMDIR} %{cfg.buildtarget.directory}",
		"{RMDIR} %{cfg.objdir}",
		"{RMDIR} generated",
		"echo Cleaning completed for %{prj.name}"
	}

	filter "system:windows"
		defines "PFF_PLATFORM_WINDOWS"
		systemversion "latest"

		buildcommands { "del /S *.pdb" }
		)";
		stream << "debugcommand(\"" << (engine_install_dir / "PFF_editor" / "PFF_editor.exe").generic_string() << "\")";
		stream <<R"(
		prebuildcommands {
		)";
			stream << "\t\"cd " << (engine_install_dir / "PFF").generic_string() << " && \" ..\n";
			stream << "\t\t\t\"" << (engine_install_dir / "PFF_helper" / "PFF_helper.exe").generic_string() << " 0 0 0 " << application::get().get_project_path().generic_string() << "\",";

			stream <<R"(
		}
		rebuildcommands{
			"{RMDIR} %{cfg.buildtarget.directory}",
			"{RMDIR} %{cfg.objdir}",
			"premake5 --file=%{wks.location}premake5.lua vs2019",
			"msbuild /t:rebuild /p:configuration=%{cfg.buildcfg} %{wks.location}%{prj.name}.vcxproj",
			"echo Rebuild completed for %{prj.name}"
		}

	filter "system:linux"
		defines "PFF_PLATFORM_LINUX"
		systemversion "latest"

		includedirs {
			"/usr/include/x86_64-linux-gnu/qt5", 				-- Base Qt include path
			"/usr/include/x86_64-linux-gnu/qt5/QtCore",
			"/usr/include/x86_64-linux-gnu/qt5/QtWidgets",
			"/usr/include/x86_64-linux-gnu/qt5/QtGui",
			"%{IncludeDir.VulkanUtils}",
		}
	
		libdirs {
			"/usr/lib/x86_64-linux-gnu",
			"/usr/lib/x86_64-linux-gnu/qt5",
		}
	
    	buildcommands { "rm -f *.pdb || true" }
		)";
		stream << "debugcommand(\"" << (engine_install_dir / "PFF_editor" / "PFF_editor").generic_string() << "\")\n";
		stream << "\t\tprebuildcommands {\n";
		stream << "\t\t\t\"cd " << filepath.parent_path().generic_string() << " && \" ..\n";
		stream << "\t\t\t\"" << (engine_install_dir / "PFF_helper" / "PFF_helper").generic_string() << " 0 0 0 " << application::get().get_project_path().generic_string() << "\",";

		stream <<R"(
		}
		rebuildcommands{
			"{RMDIR} %{cfg.buildtarget.directory}",
			"{RMDIR} %{cfg.objdir}",
			"make clean && make -j",
			"echo Rebuild completed for %{prj.name}"
		}


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

REM Create metadata and project_files directories
mkdir metadata 2>nul
mkdir metadata\project_files 2>nul

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

		io::write_to_file(stream.str().c_str(), filepath);

		//std::ofstream outStream(filepath.string());
		//outStream << stream.str().c_str();
		//outStream.close();
	}


}
