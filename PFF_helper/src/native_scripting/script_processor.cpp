
#include "util/pffpch.h"

#include "script_scanner.h"
#include "script_parser.h"
#include "code_generator.h"

#include "script_processor.h"

namespace PFF::script_processor {
	
	
	static std::filesystem::path								s_root_directory = "";
	std::filesystem::path get_root_directory()					{ return s_root_directory; }
	void set_root_directory(std::filesystem::path root_path)	{ s_root_directory = root_path; }




	static std::filesystem::path		s_project_premake_lua = "";
	static auto							s_classes = std::vector<PFF_class>();
	static bool							s_file_modified = false;
	static bool							s_building_code = false;

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

	static bool process_file_internal(const std::filesystem::path file) {

		if (!io_handler::is_file(file) || io_handler::is_hidden(file) || !is_header_file(file) || file.filename().replace_extension() == "project")		// check if file.filename() == "project" OR file.filename() == "project.h"
			return false;
		
		script_scanner fileScanner = script_scanner(file);
		std::vector<token> fileTokens = fileScanner.scan_tokens();
		script_parser fileParser = script_parser(fileTokens, file);
		fileParser.parse();
		merge_new_classes(fileParser.get_classes(), file);

		const auto relatice_filepath = util::extract_path_from_directory(file, "src");
		std::string generatedHFilename = file.filename().replace_extension("").string() + "-generated" + file.extension().string();
		const std::filesystem::path path = s_root_directory / "generated" / relatice_filepath.parent_path() / generatedHFilename;
		io_handler::create_directory(path.parent_path());
		io_handler::write_to_file(fileParser.generate_header_file().c_str(), path);
		return true;
	}

	static void file_changed(const std::filesystem::path& file) {

	}

	static void generate_initial_class_information(const std::filesystem::path& directory) {

		for (const auto& entry : std::filesystem::directory_iterator(directory)) {

			if (std::filesystem::is_directory(entry.path())) 			// path is a directory
				generate_initial_class_information(entry.path());
			else 														// path is file
				process_file_internal(entry.path());
		}
	}






    

	bool process_file(std::filesystem::path file) {

		LOG(Info, "Files changed: " << file.generic_string());
		process_file_internal(file);
		return true;
	}

	bool process_project_directory() {
		
		std::filesystem::path generated_dir = s_root_directory / "generated";
		io_handler::create_directory(generated_dir);

		generate_initial_class_information(s_root_directory / SOURCE_DIR);
		code_generator::generate_init_file_header(s_classes, generated_dir / "init.h");
		code_generator::generate_init_file_implemenation(s_classes, generated_dir / "init.cpp");

		return false;
	}

}