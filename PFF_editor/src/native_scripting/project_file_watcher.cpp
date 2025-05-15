
#include "util/pch_editor.h"

#include "PFF_editor.h"
#include "util/io/io.h"
//#include "script_parser.h"
#include "project/file_watcher_system.h"
#include "code_generator.h"
#include "cpp_build.h"
#include "project/script_system.h"

#include "project_file_watcher.h"

namespace PFF {

	static file_watcher_system		s_file_watcher;
	static std::filesystem::path	s_root_directory = "";
	static std::filesystem::path	s_project_premake_lua = "";

	static void file_changed(const std::filesystem::path& file) {

		const std::filesystem::path path_to_build_script = util::get_executable_path() / ".." / "PFF_helper" / "PFF_helper.exe";
		std::string cmdArgs = "1 0 1 " + s_root_directory.generic_string();
		LOG(Info, "CMD Args: " << cmdArgs.c_str());
		util::run_program(path_to_build_script, cmdArgs);
	}

	static void compile_project() {

		LOG(Trace, "compiling client project");
#if 1

#if defined(PFF_PLATFORM_WINDOWS)
		const std::filesystem::path path_to_build_script = util::get_executable_path() / ".." / "PFF_helper" / "PFF_helper.exe";
#elif defined(PFF_PLATFORM_LINUX)
		const std::filesystem::path path_to_build_script = util::get_executable_path() / ".." / "PFF_helper" / "PFF_helper";
#endif

		std::string cmdArgs = "1 0 0 " + s_root_directory.generic_string();
		LOG(Info, "CMD Args: " << cmdArgs.c_str());
		util::run_program(path_to_build_script, cmdArgs);

// 		LOG(Trace, "project DLL not found, recompiling")
// #if defined(PFF_PLATFORM_WINDOWS)
// 		const std::filesystem::path path_to_build_script = util::get_executable_path().parent_path() / "PFF_helper" / "PFF_helper.exe";
// #elif defined(PFF_PLATFORM_LINUX)
// 		const std::filesystem::path path_to_build_script = util::get_executable_path().parent_path() / "PFF_helper" / "PFF_helper";
// #endif

// 		std::string cmdArgs = "1 0 0 " + PROJECT_PATH.generic_string();
// 		LOG(Info, "CMD Args: " << cmdArgs.c_str());
// 		VALIDATE(util::run_program(path_to_build_script, cmdArgs), , "project files generated succesfully", "Failed to generate project files")

// 		const auto gmake_command = "python3";
// 		std::string compile_args = (PROJECT_PATH / "setup.py").generic_string() + " --compile";
// 		LOG(Fatal, "calling [" << gmake_command << "] with [" << compile_args << "]")
// 		std::string output;
// 		const bool compile_result = util::run_program(gmake_command, compile_args, false, false, true, &output);
// 		VALIDATE(compile_result, , "project files generated succesfully", "Failed to generate project files [" << output << "]")
		
#else
		generate_initial_class_information(s_root_directory / SOURCE_DIR);
		code_generator::generate_init_file_header(s_classes, generated_dir / "init.h");
		code_generator::generate_init_file_implemenation(s_classes, generated_dir / "init.cpp");

		cpp_build::build(s_root_directory);
		cpp_build::compile(s_root_directory);
		LOG(Debug, "DLL was recompiled");
		script_system::reload(false);
#endif

	}

	project_file_watcher::project_file_watcher() {
		
		s_root_directory = PROJECT_PATH;
		VALIDATE(io::is_directory(s_root_directory), return, "", "[" << s_root_directory.string().c_str() << "] is not a directory. project_file_watcher is not starting.");

		// ================================== TODO: Move to project creation ==================================
		s_project_premake_lua = s_root_directory / "premake5.lua";
		code_generator::generate_premake_file(s_project_premake_lua, "test_project");
		const std::filesystem::path pathToBuildScript = s_root_directory / "build.bat";
		const std::filesystem::path pathToPremakeExe = util::get_executable_path() / ".." / "vendor" / "premake" / "premake5.exe";
		code_generator::generate_build_file(pathToBuildScript, pathToPremakeExe);
		// ================================== TODO: Move to project creation ==================================

		LOG(Trace, "Monitoring directory " << io::get_absolute_path(s_root_directory / SOURCE_DIR));
		s_file_watcher.path = s_root_directory / SOURCE_DIR;
		s_file_watcher.p_notify_filters = notify_filters::LastAccess | notify_filters::LastWrite | notify_filters::FileName | notify_filters::DirectoryName;
		s_file_watcher.filter = "*.h";
		s_file_watcher.include_sub_directories = true;
		s_file_watcher.on_changed = file_changed;
		s_file_watcher.on_created = file_changed;
		s_file_watcher.on_renamed = file_changed;
		s_file_watcher.compile = compile_project;
		s_file_watcher.start();
	}

	project_file_watcher::~project_file_watcher() { s_file_watcher.stop(); }

}
