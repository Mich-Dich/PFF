
#include "util/pffpch.h"

#include "stopwatch.h"

namespace PFF {


	f32 stopwatch::stop() {

        std::chrono::system_clock::time_point end_point = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_point - m_start_point);      //convert to microseconds (integer), and then come back to miliseconds
        return (*m_result_pointer = elapsed.count() / 1000.f);
    }


    void stopwatch::_start() { m_start_point = std::chrono::system_clock::now(); }


}
