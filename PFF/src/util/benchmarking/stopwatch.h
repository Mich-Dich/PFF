#pragma once


namespace PFF {

    namespace benchmarking {

#define PFF_SCOPED_BENCHMARK(num_of_tests, message)     static PFF::benchmarking::simple_bencharking loc_simple_bencharking = PFF::benchmarking::simple_bencharking(num_of_tests, message);         \
                                                        PFF::stopwatch loc_stopwatch_for_benchmarking(&loc_simple_bencharking.single_duration);                                                     \
                                                        if (loc_simple_bencharking.add_value())                                                                                                     \
                                                            loc_stopwatch_for_benchmarking.restart();
        class simple_bencharking {
        public:

            simple_bencharking(u64 num_of_tests, std::string message)
                : num_of_tests(num_of_tests), message(message) {}

            bool add_value();
            f32     single_duration = -1.f;

        private:
            std::string     message{};
            u64             num_of_tests{};
            f64             durations{};
            u32             number_of_values{};
        };

    }

    // @brief This is a lightweit stopwatch that is automaticlly started when creating an instance.
    //        It can either store the elapsed time in a provided float pointer when the stopwatch is stopped or when it is destroyed, 
    //        or it can allow retrieval of the elapsed time by manually calling [stop()] method.
    //        The time is measured in milliseconds.
    class PFF_API stopwatch {
    public:

        stopwatch() { _start(); }
        stopwatch(f32* result_pointer)
            : m_result_pointer(result_pointer) { _start(); }

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

        std::chrono::system_clock::time_point       m_start_point{};
        f32                                         m_result = 0.f;
        f32*                                        m_result_pointer = &m_result;
    };

}
