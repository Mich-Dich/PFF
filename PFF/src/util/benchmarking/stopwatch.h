#pragma once


namespace PFF {

    enum class duration_precision {
        microseconds,
        milliseconds,
        seconds,
    };

#ifdef PFF_DEBUG
    namespace benchmarking {

#define PFF_SCOPED_BENCHMARK(num_of_tests, message, duration_precision)     static PFF::benchmarking::simple_bencharking loc_simple_bencharking(num_of_tests, message, duration_precision);         \
                                                                            PFF::stopwatch loc_stopwatch_for_benchmarking(&loc_simple_bencharking.single_duration, duration_precision);      \
                                                                            if (loc_simple_bencharking.add_value())                                                                                 \
                                                                                loc_stopwatch_for_benchmarking.restart()

    
#define PFF_DURATION_microseconds       " microseconds"
#define PFF_DURATION_miliseconds        " milliseconds"
#define PFF_DURATION_seconds            " seconds"

#define PFF_BENCHMARK_LOOP(num_of_iterations, message, benchmark_duration_precision, func)                                                                                                                        \
            {                                                                                                                                                                                           \
                f32 duration = -1.f;                                                                                                                                                                    \
                {   PFF::stopwatch loc_stpowatch(&duration);                                                                                                                                            \
                    for (size_t x = 0; x < num_of_iterations; x++) { func }                                                                                                                             \
                } CORE_LOG(Info, message << " => sample count: " << num_of_iterations << " average duration: " << (duration / (f64)num_of_iterations) << PFF_DURATION_##benchmark_duration_precision);  \
            }

        class simple_bencharking {
        public:

            simple_bencharking(u64 num_of_tests, std::string message, PFF::duration_precision presition = PFF::duration_precision::milliseconds)
                : m_num_of_tests(num_of_tests), m_message(message), m_presition(presition) {}

            bool add_value();
            f32     single_duration = -1.f;

        private:
            duration_precision  m_presition{};
            std::string         m_message{};
            u64                 m_num_of_tests{};
            f64                 m_durations{};
            u32                 m_number_of_values{};
        };

    }
#endif

    // @brief This is a lightweit stopwatch that is automaticlly started when creating an instance.
    //        It can either store the elapsed time in a provided float pointer when the stopwatch is stopped/destroyed, 
    //        or it can allow retrieval of the elapsed time by manually calling [stop()] method.
    //        The time is measured in milliseconds.
    class PFF_API stopwatch {
    public:

        stopwatch(f32* result_pointer, duration_precision presition = duration_precision::milliseconds)
            : m_result_pointer(result_pointer), m_presition(presition) { _start(); }

        ~stopwatch() { stop(); }

        PFF_DEFAULT_GETTER_C(f32, result);

        // @brief Stops the stopwatch and calculates the elapsed time.
        //        If a result pointer was provided, it will be updated with the elapsed time.
        // @return The elapsed time in milliseconds since the stopwatch was started.
        FORCEINLINE f32 stop();

        // @breif Restarts the stopped stopwatch
        FORCEINLINE void restart();

    private:

        FORCEINLINE void _start();

        duration_precision                          m_presition;
        std::chrono::system_clock::time_point       m_start_point{};
        f32                                         m_result = 0.f;
        f32*                                        m_result_pointer = &m_result;
    };

}