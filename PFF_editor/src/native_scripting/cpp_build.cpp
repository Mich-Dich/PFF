
#include "util/pch_editor.h"

#include "cpp_build.h"

namespace PFF::cpp_build {

	static bool building_code = false;
	static bool compiling = false;

	void build(const std::filesystem::path& project_directory) {

		if (building_code || compiling) {

			CORE_LOG(Warn, "building currently not posible => is currently building: " << util::to_string(building_code) << " / is currently compiling: " << util::to_string(compiling));
			return;
		}

		building_code = true;
		const std::filesystem::path path_to_build_script = project_directory / "build.bat";
		std::string cmdArgs = "";
		CORE_LOG(Info, "CMD Args: " << cmdArgs.c_str());
		util::run_program(path_to_build_script, cmdArgs);
		building_code = false;
	}

	void compile(const std::filesystem::path& project_directory) {

		if (building_code || compiling) {

			CORE_LOG(Warn, "compiling currently not posible => is currently building: " << util::to_string(building_code) << " / is currently compiling: " << util::to_string(compiling));
			return;
		}

		compiling = true;
		std::filesystem::path path_to_build_script = project_directory / "build.bat";
		std::string cmdArgs = "compile";
		CORE_LOG(Info, "CMD Args: " << cmdArgs.c_str());
		util::run_program(path_to_build_script, cmdArgs);
		compiling = false;
	}
}
 