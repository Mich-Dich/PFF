
#include "util/pffpch.h"
#include "util/math/math.h"

#include "stopwatch.h"

namespace PFF {

#ifdef PFF_DEBUG

    namespace Profiling {

        bool simple_profiler::add_value() {

            if (single_duration == -1.f)
                return true;

            if (m_number_of_values >= m_num_of_tests)
                return false;

            m_durations += single_duration;
            m_number_of_values++;
            if (m_number_of_values >= m_num_of_tests) {

                std::string precition_string;
                switch (m_presition) {
                    case PFF::duration_precision::microseconds:     precition_string = " microseconds"; break;
                    case PFF::duration_precision::seconds:          precition_string = " seconds"; break;
                    default:
                    case PFF::duration_precision::milliseconds:     precition_string = " milliseconds"; break;
                }
                LOG(Info, m_message << " => sample count: " << m_number_of_values << " average duration: " << (m_durations / (f64)m_number_of_values) << precition_string);
            }
            return true;
        }
    }

#endif


    f32 stopwatch::stop() {

        std::chrono::system_clock::time_point end_point = std::chrono::system_clock::now();
        switch (m_presition) {
            case PFF::duration_precision::microseconds: return *m_result_pointer = std::chrono::duration_cast<std::chrono::nanoseconds>(end_point - m_start_point).count() / 1000.f;
            case PFF::duration_precision::seconds:      return *m_result_pointer = std::chrono::duration_cast<std::chrono::milliseconds>(end_point - m_start_point).count() / 1000.f;
            default:
            case PFF::duration_precision::milliseconds: return *m_result_pointer = std::chrono::duration_cast<std::chrono::microseconds>(end_point - m_start_point).count() / 1000.f;
        }
    }

    void stopwatch::restart() {

        _start();
    }


    void stopwatch::_start() { m_start_point = std::chrono::system_clock::now(); }

}
