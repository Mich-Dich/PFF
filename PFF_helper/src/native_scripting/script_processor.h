#pragma once

namespace script_processor {

	enum class native_scripting_operation {

		parse_directory		= 0,
		parse_file			= 1,
		create_script		= 2,
		delete_file			= 3,

		END_OF_ENUM			= 4,
	};

	FORCEINLINE std::string operation_to_string(native_scripting_operation selected_operation) {

		switch (selected_operation) {
		case native_scripting_operation::parse_directory:	return "parse directory";
		case native_scripting_operation::parse_file:		return "parse file";
		case native_scripting_operation::create_script:		return "create script";
		case native_scripting_operation::delete_file:		return "delete file";
		case native_scripting_operation::END_OF_ENUM:
		default:											return "INVALID OPERATION VALUE";
		}
	}

	// GUIDE:
	//  PARAM 3: (FIXED)        enum                        operation
	//  PARAM 4: (FIXED)        std::filesystem::path       path to projects root
	//  PARAM 5: (OPTIONAL)     std::filesystem::path       path to file of interest
	bool start(int argc, char* argv[]);

	std::filesystem::path get_root_directory();

}
