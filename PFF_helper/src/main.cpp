
#include <util/pffpch.h>

#include "native_scripting/script_processor.h"


// Define an enum for the first parameter
enum class operation {
    parse_directory     = 0,
    parse_file          = 1,
    create_script       = 2,
    delete_file         = 3,
    END_OF_ENUM         = 4,
};

FORCEINLINE std::string operation_to_string(operation selected_operation) {

    switch (selected_operation) {
    case operation::parse_directory:    return "parse directory";
    case operation::parse_file:         return "parse directory";
    case operation::create_script:      return "parse directory";
    case operation::delete_file:        return "parse directory";
    case operation::END_OF_ENUM:
    default:                            return "parse directory";
    }
}


// GUIDE:
//  PARAM 1: (FIXED)        int                         use_color_in_log
//  PARAM 2: (FIXED)        enum                        operation
//  PARAM 3: (FIXED)        std::filesystem::path       path to projects root
//  PARAM 4: (OPTIONAL)     std::filesystem::path       path to file of interest

int main(int argc, char* argv[]) {

    VALIDATE(argc >= 4, return 1, "", "Usage: " << argv[0] << " <operation_number> <file_path>");

    int result = 0;

    bool use_color_in_log = std::stoi(argv[1]);
    if (use_color_in_log)
        PFF::logger::set_format("[$B$T:$J$E] [$B$L$X $I - $P:$G$E] $C$Z");
    else
        PFF::logger::set_format("[$T:$J] [$L$X $I - $P:$G] $C$Z");


    int operation_number = std::stoi(argv[2]);
    operation loc_operation = static_cast<operation>(operation_number);
    std::filesystem::path path_to_project_root = argv[3];
    LOG(Trace, "PFF_helper called for [" << operation_to_string(loc_operation) << "] project path [" << path_to_project_root << "]");
        
    PFF::script_processor::set_root_directory(path_to_project_root);

    switch (loc_operation) {
    case operation::parse_directory:        PFF::script_processor::process_project_directory(); break;
    case operation::parse_file:
            
        ASSERT(argc == 4, "", "Supplyed arguments are invalid for selected option [operation::parse_file] argument count [" << argc << "]");
        PFF::script_processor::process_file(static_cast<std::filesystem::path>(argv[4]));
        break;

    case operation::create_script: {

        LOG(Trace, "create directory: " << path_to_project_root);
        break;
    }
    case operation::delete_file: {

        LOG(Trace, "delete file: " << path_to_project_root);
        break;
    }
    case operation::END_OF_ENUM:
    default:
        LOG(Trace, "Invalid operation number [" << operation_number << "]");
        result = 1;
    }
    
    LOG(Trace, "PFF_helper finished ==== " << ((result) ? "FAILED" : "SUCCESS"));

    PFF::logger::use_format_backup();
    return result;
}
