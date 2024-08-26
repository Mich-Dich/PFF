#pragma once

#include "script_parser.h"

namespace PFF::code_generator {

	void generate_init_file(const std::vector<PFF_class>& classes, const std::filesystem::path& filepath);

	void generate_premake_file(const std::filesystem::path& filepath, const std::string_view project_name);

	void generate_build_file(const std::filesystem::path& filepath, const std::filesystem::path& premakeFilepath);

}
