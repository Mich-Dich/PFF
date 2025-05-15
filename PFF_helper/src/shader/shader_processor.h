#pragma once

namespace shader_processor {

	enum class shader_operation {

		compile_shaders_in_dir		= 0,
		compile_shader				= 1,

		END_OF_ENUM					= 2,
	};

	FORCEINLINE std::string operation_to_string(shader_operation selected_operation) {

		switch (selected_operation) {
		case shader_operation::compile_shaders_in_dir:		return "compile shaders in dir";
		case shader_operation::compile_shader:				return "compile shader";
		case shader_operation::END_OF_ENUM:
		default:											return "INVALID OPERATION VALUE";
		}
	}

	// GUIDE:
	//  PARAM 3: (FIXED)        enum							operation
	//  PARAM 4: (FIXED)        const std::filesystem::path		path_to_dir
	//  PARAM 5: (OPTIONAL)     const bool						compile_subdirectory  (only needed when compiling a directorty)
	//  PARAM 6: (OPTIONAL)		bool							print directly to std::out
	bool start(int argc, char* argv[]);

}
