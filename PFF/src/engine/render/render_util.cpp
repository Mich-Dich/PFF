
#include "util/pffpch.h"

#include "render_util.h"

#include "util/ui/window_images.embed"
#include "stb_image.h"

namespace PFF::render::util {

	static std::set<std::string> posible_shader_extensions{ ".frag", ".vert", ".comp" };

	// ----------------------------------- functions ---------------------------------------

	//void compile_shaders_in_dir(const std::filesystem::path path_to_dir, bool compile_subdirectory) {

	//	CORE_LOG(Trace, "Compiling shaders");
	//	std::filesystem::path absolute_path = std::filesystem::absolute(path_to_dir);
	//	for (const auto& entry : std::filesystem::directory_iterator(absolute_path)) {

	//		if (std::filesystem::is_directory(entry) && compile_subdirectory)		// recursive call for all directorys
	//			compile_shaders_in_dir(entry);

	//		compile_shader(entry.path());
	//	}
	//}

	//void compile_shader(const std::filesystem::path path) {

	//	std::filesystem::path absolute_path = std::filesystem::absolute(path);
	//	if (!std::filesystem::is_regular_file(absolute_path) || posible_shader_extensions.find(absolute_path.extension().string()) == posible_shader_extensions.end())
	//		return;

	//	std::string system_command;
	//	std::string compield_file{};
	//	compield_file = absolute_path.string() + ".spv";
	//	std::filesystem::path path_to_glsl_exe = std::filesystem::path("..") / "PFF" / "vendor" / "vulkan-glslc" / "glslc.exe";
	//	system_command = path_to_glsl_exe.string() + " " + absolute_path.string() + " -o " + compield_file;

	//	if (std::filesystem::exists(compield_file)) {
	//		if (std::filesystem::last_write_time(absolute_path).time_since_epoch().count() < std::filesystem::last_write_time(compield_file).time_since_epoch().count()) {
	//			CORE_LOG(Trace, "shader is already compield: [" << EXTRACT_AFTER_PFF(absolute_path.string()) << "]");
	//			return;
	//		}
	//	}

	//	CORE_LOG(Trace, "compiling shader: [" << EXTRACT_AFTER_PFF(absolute_path.string()) << "]");
	//	CORE_ASSERT(system(system_command.c_str()) == 0, "", "shader could not be compiled: [" << absolute_path.string() << "]");
	//}


	//GLuint LoadTextureFromEmbeddedData(const uint8_t* data, int data_size, int* out_width, int* out_height) {
	//	int width, height, channels;

	//	unsigned char* image_data = stbi_load_from_memory(data, data_size, &width, &height, &channels, 0);
	//	if (!image_data) {
	//		std::cerr << "Failed to load image from memory" << std::endl;
	//		return 0;
	//	}

	//	GLuint texture_id = CreateTextureFromData(image_data, width, height, channels);
	//	stbi_image_free(image_data);

	//	if (out_width) *out_width = width;
	//	if (out_height) *out_height = height;

	//	return texture_id;
	//}

}