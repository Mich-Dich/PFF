
namespace PFF::script_processor {

	std::filesystem::path get_root_directory();
	void set_root_directory(std::filesystem::path root_path);

	bool process_file(std::filesystem::path file);

	bool process_project_directory();

}
