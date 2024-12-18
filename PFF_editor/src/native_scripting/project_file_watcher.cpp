
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

	/*
	static auto						s_classes = std::vector<PFF_class>();
	static bool						s_file_modified = false;
	static bool						s_building_code = false;

	static bool is_header_file(const std::filesystem::path& file) { return file.extension().string() == ".h" || file.extension().string() == ".hpp"; }

	static bool Is_reflection_header_file(const std::filesystem::path& file) { return file.filename().string() == "Reflection"; }

	static void add_class_if_not_exist(PFF_class clazz) {

		for (int i = 0; i < s_classes.size(); i++) {
			PFF_class& c = s_classes.at(i);
			if (c.class_name == clazz.class_name) {
				s_classes[i].class_name = clazz.class_name;
				s_classes[i].full_filepath = clazz.full_filepath;
				s_classes[i].variables = clazz.variables;
				return;
			}
		}

		s_classes.push_back(clazz);
	}

	static std::vector<PFF_class>::iterator find_class(PFF_class& script_class, std::vector<PFF_class>& classes_to_search) {
		
		for (auto classIter = classes_to_search.begin(); classIter != classes_to_search.end(); classIter++) {
			if (classIter->class_name == script_class.class_name) 
				return classIter;
		}

		return classes_to_search.end();
	}

	static void merge_new_classes(std::vector<PFF_class>& classes_to_merge, const std::filesystem::path& filepath) {

		for (auto clazz : classes_to_merge) {
			auto c = find_class(clazz, s_classes);
			if (c != s_classes.end()) {
				c->class_name = clazz.class_name;
				c->full_filepath = clazz.full_filepath;
				c->variables = clazz.variables;
			} else
				s_classes.push_back(clazz);
		}

		for (auto classIter = s_classes.begin(); classIter != s_classes.end(); classIter++) {
			if (classIter->full_filepath == filepath) {
				if (find_class(*classIter, classes_to_merge) == classes_to_merge.end())
					classIter = s_classes.erase(classIter);
			}
		}
	}

	static bool process_file(std::filesystem::path& file, const std::filesystem::path& generatedDirPath) {

		if (!io::is_file(file) || io::is_hidden(file) || !is_header_file(file) || file.filename().replace_extension() == "project")
			return false;

		script_scanner fileScanner = script_scanner(file);
		std::vector<token> fileTokens = fileScanner.scan_tokens();
		script_parser fileParser = script_parser(fileTokens, file);
		fileParser.parse();
		merge_new_classes(fileParser.get_classes(), file);

		const auto relatice_filepath = util::extract_path_from_directory(file, "src");
		std::string generatedHFilename = file.filename().replace_extension("").string() + "-generated" + file.extension().string();
		const std::filesystem::path path = PFF_editor::get().get_project_path() / "generated" / relatice_filepath.parent_path() / generatedHFilename;
		io::create_directory(path.parent_path());
		io::write_to_file(fileParser.generate_header_file().c_str(), path);
		return true;
	}

	static void file_changed(const std::filesystem::path& file) {

		CORE_LOG(Info, "Files changed: " << file.generic_string());

		const std::filesystem::path generatedDirPath = file.parent_path().string() + "generated";
		std::filesystem::path filePath = generatedDirPath / file;
		std::filesystem::path generatedFilePath = s_root_directory / generatedDirPath;
		process_file(filePath, generatedFilePath);
	}

	static void generate_initial_class_information(const std::filesystem::path& directory) {

		auto subDirs = io::get_folders_in_dir(directory);
		for (auto dir : subDirs) 
			generate_initial_class_information(dir);

		auto files = io::get_files_in_dir(directory);
		const std::filesystem::path generatedDir = directory / ".." / "generated";
		for (auto file : files) 
			process_file(file, generatedDir);
	}
	*/

	static void file_changed(const std::filesystem::path& file) {

		const std::filesystem::path path_to_build_script = util::get_executable_path() / ".." / "PFF_helper" / "PFF_helper.exe";
		std::string cmdArgs = "1 0 1 " + s_root_directory.generic_string();
		CORE_LOG(Info, "CMD Args: " << cmdArgs.c_str());
		util::run_program(path_to_build_script, cmdArgs);
	}

	static void compile_project() {

		CORE_LOG(Error, "COMPILING PROJECT");
#if 1
		const std::filesystem::path path_to_build_script = util::get_executable_path() / ".." / "PFF_helper" / "PFF_helper.exe";
		std::string cmdArgs = "1 0 0 " + s_root_directory.generic_string();
		CORE_LOG(Info, "CMD Args: " << cmdArgs.c_str());
		util::run_program(path_to_build_script, cmdArgs);
#else
		generate_initial_class_information(s_root_directory / SOURCE_DIR);
		code_generator::generate_init_file_header(s_classes, generated_dir / "init.h");
		code_generator::generate_init_file_implemenation(s_classes, generated_dir / "init.cpp");

		cpp_build::build(s_root_directory);
		cpp_build::compile(s_root_directory);
		CORE_LOG(Debug, "DLL was recompiled");
		script_system::reload(false);
#endif

	}

	project_file_watcher::project_file_watcher() {
		
		s_root_directory = application::get().get_project_path();
		CORE_VALIDATE(io::is_directory(s_root_directory), return, "", "[" << s_root_directory.string().c_str() << "] is not a directory. project_file_watcher is not starting.");

		// ================================== TODO: Move to project creation ==================================
		s_project_premake_lua = s_root_directory / "premake5.lua";
		code_generator::generate_premake_file(s_project_premake_lua, "test_project");
		const std::filesystem::path pathToBuildScript = s_root_directory / "build.bat";
		const std::filesystem::path pathToPremakeExe = util::get_executable_path() / ".." / "vendor" / "premake" / "premake5.exe";
		code_generator::generate_build_file(pathToBuildScript, pathToPremakeExe);
		// ================================== TODO: Move to project creation ==================================

		CORE_LOG(Trace, "Monitoring directory " << io::get_absolute_path(s_root_directory / SOURCE_DIR));
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
