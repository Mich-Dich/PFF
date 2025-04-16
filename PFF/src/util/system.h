#pragma once


namespace PFF::util {

    void open_console(const char* title, const bool enable_anci_codes = false);

    // @brief Executes an external program with the specified command-line arguments.
    //          This function supports both Windows and Linux platforms, handling process creation
    //          and execution differently based on the operating system. On Windows, it uses
    //          `CreateProcessA`, while on Linux, it uses `fork` and `execv`.
    // @param [path_to_exe] The path to the executable file to be run.
    // @param [cmd_args] The command-line arguments to pass to the executable.
    // @param [open_console] If true, opens a new console window for the program (Windows) or
    //          uses a terminal emulator like `xterm` (Linux).
    // @return Returns true if the program was successfully executed, false otherwise.
    bool run_program(const std::filesystem::path& path_to_exe, const std::string& cmd_args = "", bool open_console = false, const bool display_output_on_succees = false, const bool display_output_on_failure = true, std::string* output = nullptr);

    // @brief Overload of `run_program` that accepts a C-style string for command-line arguments.
    // @param [path_to_exe] The path to the executable file to be run.
    // @param [cmd_args] The command-line arguments as a C-style string.
    // @param [open_console] If true, opens a new console window for the program (Windows) or
    //          uses a terminal emulator like `xterm` (Linux).
    // @return Returns true if the program was successfully executed, false otherwise.
    bool run_program(const std::filesystem::path& path_to_exe, const char* cmd_args = "", bool open_console = false, const bool display_output_on_succees = false, const bool display_output_on_failure = true, std::string* output = nullptr);

    // @brief Pauses the execution of the current thread for a specified duration with high precision.
    //          This function first uses `std::this_thread::sleep_for` to sleep for nearly the entire duration,
    //          adjusting for an estimated deviation to account for inaccuracies in sleep timing. 
    //          Following this, it performs a busy wait to ensure that the total sleep time is as accurate as possible,
    //          considering that the operating system's sleep function may not be perfectly precise.
    // @param [duration_in_milliseconds] The duration for which the thread should be paused. The function converts this
    //          value to milliseconds and adjusts for an estimated deviation before performing a busy wait until
    //          the desired wake-up time.
    void high_precision_sleep(f32 duration_in_milliseconds);
    
    // @brief Retrieves the current system time, including year, month, day, hour, minute, second, and millisecond.
    //          This function is platform-specific, using `GetLocalTime` on Windows and `gettimeofday` on Linux.
    // @return Returns a `system_time` struct containing the current system time.
    system_time get_system_time();

    // @brief Default file filters for use with file dialogs.
    //          This includes filters for text files, C++ source files, and all files.
    const std::vector<std::pair<std::string, std::string>> default_filters = {
        {"Text Files", "*.txt"},
        {"C++ Files", "*.cpp;*.h;*.hpp"},
        {"All Files", "*.*"}
    };

#if defined(PFF_PLATFORM_LINUX)
    
    // @brief Initializes the Qt application framework.
    //          This function is automaticly called by the PFF::aplication on startup
    //          It sets up the Qt message handler to route Qt log messages through the custom logging system.
    void init_qt();

    // @brief Shuts down the Qt application framework.
    //          This function should be called during application cleanup to properly release Qt resources.
    void shutdown_qt();
#endif

    // @brief Opens a file dialog to allow the user to select a file.
    //          This function is platform-specific, using `GetOpenFileNameW` on Windows and `QFileDialog` on Linux.
    // @param [title] The title of the file dialog window.
    // @param [filters] A list of file filters to display in the dialog. Each filter is a pair of
    //          a description and a file extension pattern (e.g., "Text Files (*.txt)").
    // @return Returns the path to the selected file, or an empty path if no file was selected.
	std::filesystem::path file_dialog(const std::string& title = "Open", const std::vector<std::pair<std::string, std::string>>& filters = default_filters);

    // @brief Retrieves the directory containing the currently running executable.
    //          This function is platform-specific, using `GetModuleFileNameW` on Windows and
    //          `/proc/self/exe` on Linux to determine the executable path.
    // @return Returns the path to the directory containing the executable, or an empty path on error.
	std::filesystem::path get_executable_path();

}
