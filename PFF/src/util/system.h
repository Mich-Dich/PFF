#pragma once


namespace PFF::util {

    bool run_program(const std::filesystem::path& path_to_exe, const std::string& cmd_args = "", bool open_console = false);

    bool run_program(const std::filesystem::path& path_to_exe, const char* cmd_args = "", bool open_console = false);

    // @brief Pauses the execution of the current thread for a specified duration with high precision.
    //          This function first uses `std::this_thread::sleep_for` to sleep for nearly the entire duration,
    //          adjusting for an estimated deviation to account for inaccuracies in sleep timing. 
    //          Following this, it performs a busy wait to ensure that the total sleep time is as accurate as possible,
    //          considering that the operating system's sleep function may not be perfectly precise.
    // @param [duration_in_milliseconds] The duration for which the thread should be paused. The function converts this
    //          value to milliseconds and adjusts for an estimated deviation before performing a busy wait until
    //          the desired wake-up time.
    void high_precision_sleep(f32 duration_in_milliseconds);
    
    system_time get_system_time();

    const std::vector<std::pair<std::string, std::string>> default_filters = {
        {"Text Files", "*.txt"},
        {"C++ Files", "*.cpp;*.h;*.hpp"},
        {"All Files", "*.*"}
    };

	std::filesystem::path file_dialog(const std::string& title = "Open", const std::vector<std::pair<std::string, std::string>>& filters = default_filters);

	std::filesystem::path get_executable_path();

}
