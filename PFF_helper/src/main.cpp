
#include <util/pffpch.h>

#include "native_scripting/script_processor.h"
#include "shader/shader_processor.h"

// Define an enum for the first parameter
enum class operation {

    native_scripting        = 0,
    shaders                 = 1,

    END_OF_ENUM             = 2,
};

inline std::string operation_to_string(operation selected_operation) {

    switch (selected_operation) {
    case operation::native_scripting:   return "native scripting";
    case operation::shaders:            return "shaders";
    case operation::END_OF_ENUM:
    default:                            return "INVALID OPERATION VALUE";
    }
}


// GUIDE:
//  PARAM 1: (FIXED)        bool                        use_color_in_log
//  PARAM 2: (FIXED)        enum                        operation

int main(int argc, char* argv[]) {

    PFF::logger::init("[$B$T:$J  $L$X  $I $P:$G$E] $C$Z", true);
    VALIDATE(argc >= 3, return 1, "", "Usage: " << argv[0] << " <operation_number> <file_path>");

    if (!std::stoi(argv[1]))
    	PFF::logger::set_format("[$T:$J  $L$X  $I $P:$G] $C$Z");

    operation loc_operation = static_cast<operation>(std::stoi(argv[2]));

    f32 time_result = 0;
    PFF::stopwatch loc_stopwatch{ &time_result, PFF::duration_precision::seconds };

    bool result = false;
    switch (loc_operation) {
        case operation::native_scripting:           result = script_processor::start(argc, argv); break;
        case operation::shaders:                    result = shader_processor::start(argc, argv); break;
        default:                                    break;
    }
        
    loc_stopwatch.stop();
    LOG(Trace, "========== PFF_helper " << ((result) ? "FAILED" : "SUCCEDED") << " and took [" << time_result << " s] ==========");
    
    PFF::logger::shutdown();
    return result;
}
