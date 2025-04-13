
#include <util/pffpch.h>

#include <util/system.h>
#include <util/data_structures/string_manipulation.h>

#include "shader_processor.h"

namespace shader_processor {
	
	static std::set<std::string> posible_shader_extensions{ ".frag", ".vert", ".comp" };


	static bool compile_shader(const std::filesystem::path path) {

		std::filesystem::path absolute_path = std::filesystem::absolute(path);
		if (!std::filesystem::is_regular_file(absolute_path) || posible_shader_extensions.find(absolute_path.extension().string()) == posible_shader_extensions.end())
			return false;

		std::string system_command;
		std::string compield_file{};
		compield_file = absolute_path.string() + ".spv";
		system_command = absolute_path.string() + " -o " + compield_file;
		if (std::filesystem::exists(compield_file)) {
			if (std::filesystem::last_write_time(absolute_path).time_since_epoch().count() < std::filesystem::last_write_time(compield_file).time_since_epoch().count()) {
				LOG(Trace, "shader is already compield: [" << EXTRACT_AFTER_PFF(absolute_path.string()) << "]");
				return false;
			}
		}

		// LOG(Trace, "compiling shader: [" << EXTRACT_AFTER_PFF(absolute_path.string()) << "]");
		LOG(Trace, "compiling shader: [" << system_command << "]");

		bool result = PFF::util::run_program(GLSLC_PATH, system_command, false, true, true);
		VALIDATE(result, , "", "shader could not be compiled: [" << absolute_path.string() << "]");
		return result;
	}

	static bool compile_shaders_in_dir(const std::filesystem::path path_to_dir, bool compile_subdirectory) {

		bool found_error = false;		// invert bool for easier concatonation
		std::filesystem::path absolute_path = std::filesystem::absolute(path_to_dir);
		for (const auto& entry : std::filesystem::directory_iterator(absolute_path)) {

			if (std::filesystem::is_directory(entry) && compile_subdirectory)		// recursive call for all directorys
				compile_shaders_in_dir(entry, compile_subdirectory);
			else
				found_error |= !compile_shader(entry.path());
		}

		return !found_error;
	}


	bool start(int argc, char* argv[]) {

		PFF::logger::use_previous_format();
		if (std::stoi(argv[1]))
			PFF::logger::set_format("[$B$T:$J$E] [$B$L$X $I - $P:$G$E] $C$Z");
		else
			PFF::logger::set_format("[$T:$J PFF_helper/shader_processor] $C$Z");

		int operation_number = std::stoi(argv[3]);
		shader_operation operation = static_cast<shader_operation>(operation_number);
		std::filesystem::path path_to_dir = argv[4];
		LOG(Trace, "called for [" << operation_to_string(operation) << "] project path [" << path_to_dir << "]");

		bool result = false;
		switch (operation) {
			case shader_operation::compile_shader:					result = compile_shader(path_to_dir); break;
			case shader_operation::compile_shaders_in_dir:			result = compile_shaders_in_dir(path_to_dir, ((argc >= 6) ? (bool)argv[5] : false)); break;
			case shader_operation::END_OF_ENUM:
			default:												result = false; break;
		}

		return result;
	}
}
