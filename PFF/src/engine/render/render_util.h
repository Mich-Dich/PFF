#pragma once

namespace PFF::render::util {

#define PFF_RENDER_API_VULKAN

	// @brief It iterates through each file in the directory and compiles any file recognized as a shader.
	//        If [compile_subdirectory] is set to true, it recursively searches and compiles shaders in subdirectories.
	// @param [path_to_dir] The path to the directory containing shaders.
	// @param [compile_subdirectory] If true, shaders in subdirectories will also be compiled; otherwise, only shaders in the specified directory will be compiled.
	// @return None.
	void compile_shaders_in_dir(const std::filesystem::path path_to_dir = "../PFF/shaders", bool compile_subdirectory = true);

	// @brief Compiles a single shader file into SPIR-V bytecode using the Vulkan GLSL compiler.
	//        If the shader file does not exist or is not recognized as a shader file based on its extension,
	//        the function returns without further action. If the compiled file already exists and its modification
	//        time is later than that of the source shader file, also returns without further action.
	//		  Otherwise, it compiles the shader
	// @param [path] The path to the shader file to compile.
	// @return None.
	void compile_shader(const std::filesystem::path path);


	//GLuint LoadTextureFromEmbeddedData(const uint8_t* data, int data_size, int* out_width, int* out_height);

}
