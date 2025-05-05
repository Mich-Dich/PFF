#pragma once

namespace PFF::code_generator {

	void generate_init_file_header(const std::vector<PFF_class>& classes, const std::filesystem::path& filepath);

	void generate_init_file_implemenation(const std::vector<PFF_class>& classes, const std::filesystem::path& filepath, const std::filesystem::path& root_path);

}
