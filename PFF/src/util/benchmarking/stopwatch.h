#pragma once


namespace PFF {

    // @brief This class provides a simple stopwatch functionality.
    //        It can either store the elapsed time in a provided float pointer
    //        when the stopwatch is stopped or when it is destroyed, or it can
    //        allow retrieval of the elapsed time by manually calling [stop()] method.
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

    private:

        FORCEINLINE void _start();

        std::chrono::system_clock::time_point       m_start_point{};
        f32                                         m_result = 0.f;
        f32* m_result_pointer = &m_result;
    };

}