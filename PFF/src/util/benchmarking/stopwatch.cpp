
#include "util/pffpch.h"
#include "util/math/math.h"

#include "stopwatch.h"


namespace PFF::benchmarking {


    bool simple_bencharking::add_value() {

        if (single_duration == -1.f)
            return true;

        if (number_of_values >= num_of_tests)
            return false;

        durations += single_duration;
        number_of_values++;

        if (number_of_values >= num_of_tests) {

            f64 duration_average = durations / (f64)number_of_values;
            CORE_LOG(Info, message << " => sample count: " << number_of_values << "average duration: " << duration_average << " ms");
        }

        return true;
    }

}


namespace PFF {


	f32 stopwatch::stop() {

        std::chrono::system_clock::time_point end_point = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_point - m_start_point);      //convert to microseconds (integer), and then come back to miliseconds
        return (*m_result_pointer = elapsed.count() / 1000.f);
    }

    void stopwatch::restart() {

        _start();
    }


    void stopwatch::_start() { m_start_point = std::chrono::system_clock::now(); }


}
