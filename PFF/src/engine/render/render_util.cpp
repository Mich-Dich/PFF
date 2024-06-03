
#include "util/pffpch.h"

#include "render_util.h"

namespace PFF::render::util {

	static std::set<std::string> posible_shader_extensions{ ".frag", ".vert", ".comp" };

	// ----------------------------------- functions ---------------------------------------

	void compile_shaders_in_dir(const std::filesystem::path path_to_dir, bool compile_subdirectory) {

		CORE_LOG(Trace, "Compiling shaders");
		std::filesystem::path absolute_path = std::filesystem::absolute(path_to_dir);
		for (const auto& entry : std::filesystem::directory_iterator(absolute_path)) {

			if (std::filesystem::is_directory(entry) && compile_subdirectory)		// recursive call for all directorys
				compile_shaders_in_dir(entry);

			compile_shader(entry.path());
		}
	}

	void compile_shader(const std::filesystem::path path) {

		std::filesystem::path absolute_path = std::filesystem::absolute(path);
		if (!std::filesystem::is_regular_file(absolute_path) || posible_shader_extensions.find(absolute_path.extension().string()) == posible_shader_extensions.end())
			return;

		std::string system_command;
		std::string compield_file{};
		compield_file = absolute_path.string() + ".spv";

#if defined(PFF_PLATFORM_WINDOWS)
		system_command = "..\\PFF\\vendor\\vulkan-glslc\\glslc.exe " + absolute_path.string() + " -o " + compield_file;
#elif
		system_command = "../PFF/vendor/vulkan-glslc/glslc.exe " + absolute_path.string() + " -o " + compield_file;
#endif

		if (std::filesystem::exists(compield_file)) {
			if (std::filesystem::last_write_time(absolute_path).time_since_epoch().count() < std::filesystem::last_write_time(compield_file).time_since_epoch().count()) {
				CORE_LOG(Trace, "shader is already compield: [" << EXTRACT_AFTER_PFF(absolute_path.string()) << "]");
				return;
			}
		}

		CORE_LOG(Trace, "compiling shader: [" << EXTRACT_AFTER_PFF(absolute_path.string()) << "]");
		CORE_ASSERT(system(system_command.c_str()) == 0, "", "shader could not be compiled: [" << absolute_path.string() << "]");
	}

}