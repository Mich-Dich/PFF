
#include "util/pffpch.h"

// the gracefull signal handeling was inspired by reckless_log: https://github.com/mattiasflodin/reckless

#if defined(PLATFORM_WINDOWS)
    #include <Windows.h>
#elif defined(PLATFORM_LINUX)
    #include <time.h>
    #include <sys/time.h>
    #include <ctime>
#endif

#include "util/util.h"
#include "logger.h"

namespace PFF::logger {

#define SETW(width)                                             std::setw(width) << std::setfill('0')
#define SHORT_FILE(text)                                        (strrchr(text, "\\") ? strrchr(text, "\\") + 1 : text)
#define SHORTEN_FUNC_NAME(text)                                 (strstr(text, "::") ? strstr(text, "::") + 2 : text)

    // const after init() and bevor shutdown()
    static bool                                                 is_init = false;
    static bool                                                 write_logs_to_console = false;
    static std::filesystem::path                                main_log_dir = "";
    static std::filesystem::path                                main_log_file_path = "";
    static std::thread                                          worker_thread;

    // thread savety related
    static std::condition_variable                              cv{};
    static std::mutex                                           queue_mutex{};                      // only queue related
    static std::mutex                                           general_mutex{};                    // for everything else
    static std::atomic<bool>                                    ready = false;
    static std::atomic<bool>                                    stop = false;

    // always const variables
    static std::string_view                                     severity_names[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
    static std::string_view                                     console_reset = "\x1b[0m";
    static std::string_view                                     console_color_table[] = {
        "\x1b[38;5;246m",                                           // Trace: Gray
        "\x1b[94m",                                                 // Debug: Blue
        "\x1b[92m",                                                 // Info: Green
        "\x1b[33m",                                                 // Warn: Yellow
        "\x1b[31m",                                                 // Error: Red
        "\x1b[41m\x1b[30m",                                         // Fatal: Red Background
    };

    static std::string                                          format_current = "";
    static std::string                                          format_prev = "";
    static std::ofstream                                        main_file;
    static std::queue<message_format>                           log_queue{};
    static std::unordered_map<std::thread::id, std::string>     thread_lable_map = {};

    void process_queue();
    void process_log_message(const message_format message);
    void detach_crash_handler();

    inline const char* get_filename(const char* filepath) {

        const char* filename = std::strrchr(filepath, '\\');
        if (filename == nullptr)
            filename = std::strrchr(filepath, '/');

        if (filename == nullptr)
            return filepath;  // No path separator found, return the whole string

        return filename + 1;  // Skip the path separator
    }


#define OPEN_MAIN_FILE(append)              { if (!main_file.is_open()) {                                                                               \
                                                main_file = std::ofstream(main_log_file_path, (append) ? std::ios::app : std::ios::out);                \
                                                if (!main_file.is_open())                                                                               \
                                                    DEBUG_BREAK("FAILED to open log main_file") } }

#define CLOSE_MAIN_FILE()                   main_file.close();



    // ====================================================================================================================================
    // init / shutdown
    // ====================================================================================================================================

    bool init(const std::string& format, const bool log_to_console, const std::filesystem::path log_dir, const std::string& main_log_file_name, const bool use_append_mode) {

        if (is_init)
            DEBUG_BREAK("Tryed to init lgging system multiple times")

        format_current = format;
        format_prev = format;
        write_logs_to_console = log_to_console;

        if (!std::filesystem::is_directory(log_dir))                            // if not already created
            if (!std::filesystem::create_directory(log_dir))                    // try to create dir
                DEBUG_BREAK("FAILED to create directory for log files")

        main_log_dir = log_dir;
        main_log_file_path = log_dir / main_log_file_name;

        OPEN_MAIN_FILE(use_append_mode)

        if (use_append_mode)
            main_file << "\n=============================================================================\n";

        // add some general info to beginning of file
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        main_file << "Log initialized at [" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "]\n"
            << "Inital Log Format: '" << format << "' \nEnabled Log Levels: ";

        const char* log_sev_strings[] = { "Fatal", " + Error", " + Warn", " + Info", " + Debug", " + Trace"};
        for (int x = 0; x < LOG_LEVEL_ENABLED +2; x++)
            main_file << log_sev_strings[x];
        main_file << "\n=============================================================================\n";

        worker_thread = std::thread(&process_queue);

        CLOSE_MAIN_FILE()
        is_init = true;
        return true;
    }

    void shutdown() {

        if (!is_init)
            DEBUG_BREAK("logger::shutdown() was called bevor logger was initalized")

        stop = true;
        cv.notify_all();
        detach_crash_handler();

        if (worker_thread.joinable())
            worker_thread.join();

        if (main_file.is_open())
            CLOSE_MAIN_FILE()

        is_init = false;
    }

    // ====================================================================================================================================
    // signal handeling         need to catch signals related to termination to flash remaining log messages
    // ====================================================================================================================================

    const std::initializer_list<int> signals = {
        SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGFPE, SIGKILL, SIGSEGV, SIGPIPE, SIGALRM, SIGTERM, SIGUSR1, SIGUSR2,    // POSIX.1-1990 signals
        SIGBUS, SIGPOLL, SIGPROF, SIGSYS, SIGTRAP, SIGVTALRM, SIGXCPU, SIGXFSZ,                                             // SUSv2 + POSIX.1-2001 signals
        SIGIOT, SIGSTKFLT, SIGIO, SIGPWR,                                                                                   // Various other signals
    };
    std::vector<std::pair<int, struct sigaction>> g_old_sigactions;

    void signal_handler(int) {

        logger::shutdown();
    }

    void detach_crash_handler() {

        while(!g_old_sigactions.empty()) {
            auto const& p = g_old_sigactions.back();
            auto signal = p.first;
            auto const& oldact = p.second;
            if(0 != sigaction(signal, &oldact, nullptr))
                throw std::system_error(errno, std::system_category());
            g_old_sigactions.pop_back();
        }
    }

    // ====================================================================================================================================
    // settings
    // ====================================================================================================================================

    void set_format(const std::string& new_format) {

        std::lock_guard<std::mutex> lock(general_mutex);
        OPEN_MAIN_FILE(true)
        format_prev = format_current;
        format_current = new_format;
        main_file << "[LOGGER] Changing log-format. From [" << format_prev << "] to [" << format_current << "]\n";
        CLOSE_MAIN_FILE()
    }

    void use_previous_format() {

        std::lock_guard<std::mutex> lock(general_mutex);
        OPEN_MAIN_FILE(true);
        const std::string buffer = format_current;
        format_current = format_prev;
        format_prev = buffer;
        main_file << "[LOGGER] Changing to previous log-format. From [" << format_prev << "] to [" << format_current << "]\n";
        CLOSE_MAIN_FILE()
    }

    const std::string get_format() { return format_current; }

    void register_label_for_thread(const std::string& thread_lable, std::thread::id thread_id) {

        std::lock_guard<std::mutex> lock(general_mutex);
        OPEN_MAIN_FILE(true);

        if (thread_lable_map.find(thread_id) != thread_lable_map.end())
            main_file << "[LOGGER] Thread with ID: [" << thread_id << "] already has lable [" << thread_lable_map[thread_id] << "] registered. Overriding with the lable: [" << thread_lable << "]\n";
        else
            main_file << "[LOGGER] Registering Thread-ID: [" << thread_id << "] with the lable: [" << thread_lable << "]\n";

        thread_lable_map[thread_id] = thread_lable;
        CLOSE_MAIN_FILE()
    }

    void unregister_label_for_thread(std::thread::id thread_id) {

        if (thread_lable_map.find(thread_id) == thread_lable_map.end()) {

            std::lock_guard<std::mutex> lock(general_mutex);
            OPEN_MAIN_FILE(true)
            main_file << "[LOGGER] Tried to unregister lable for Thread-ID: [" << thread_id << "]. IGNORED\n";
            CLOSE_MAIN_FILE()
            return;
        }

        std::lock_guard<std::mutex> lock(general_mutex);
        OPEN_MAIN_FILE(true)
        main_file << "[LOGGER] Unregistering Thread-ID: [" << thread_id << "] with the lable: [" << thread_lable_map[thread_id] << "]\n";
        CLOSE_MAIN_FILE()

        thread_lable_map.erase(thread_id);
    }

    // ====================================================================================================================================
    // log message handeling
    // ====================================================================================================================================

    void process_queue() {

        struct sigaction act;
        std::memset(&act, 0, sizeof(act));
        act.sa_handler = &signal_handler;
        sigfillset(&act.sa_mask);
        act.sa_flags = SA_RESETHAND;

        // Some signals are synonyms for each other. Some are explictly specified
        // as such, but others may just be implemented that way on specific
        // systems. So we'll remove duplicate entries here before we loop through
        // all the signal numbers.
        std::vector<int> unique_signals(signals);
        sort(begin(unique_signals), end(unique_signals));
        unique_signals.erase(unique(begin(unique_signals), end(unique_signals)),
                end(unique_signals));
        try {
            g_old_sigactions.reserve(unique_signals.size());
            for(auto signal : unique_signals) {
                struct sigaction oldact;
                if(0 != sigaction(signal, nullptr, &oldact))
                    throw std::system_error(errno, std::system_category());
                if(oldact.sa_handler == SIG_DFL) {
                    if(0 != sigaction(signal, &act, nullptr))
                    {
                        if(errno == EINVAL)             // If we get EINVAL then we assume that the kernel does not know about this particular signal number.
                            continue;

                        throw std::system_error(errno, std::system_category());
                    }
                    g_old_sigactions.push_back({signal, oldact});
                }
            }
        } catch(...) {
            detach_crash_handler();
            throw;
        }


        while (!stop || !log_queue.empty()) { // Continue until stop is true and the queue is empty

            std::unique_lock<std::mutex> lock(queue_mutex);
            cv.wait(lock, [] { return !log_queue.empty(); });

            while (!log_queue.empty()) {

                // get message from queue
                if (!lock.owns_lock())
                    lock.lock();
                message_format message = std::move(log_queue.front());
                log_queue.pop();
                lock.unlock();

                process_log_message(std::move(message)); // Process the message (format and write to file)
            }

            if (lock.owns_lock())                               // savety check
                lock.unlock();
        }
    }

    void log_msg(const severity msg_sev , const char* file_name, const char* function_name, const int line, const std::thread::id thread_id, const std::string& message) {

        if (message.empty())
            return;                      // dont log empty lines

        if (!is_init) {

    		std::cerr << "Tryed to log message bevor logger was initalized. file_name[" << file_name << "] function_name[" << function_name << "] line[" << line << "] thread_id[" << thread_id << "]  MESSAGE: [" << message << "] " << std::endl;
            return;
        }

        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            log_queue.emplace(msg_sev, file_name, function_name, line, thread_id, std::move(message));
        }
        cv.notify_all();
    }

    void process_log_message(const message_format message) {

        // Create Buffer vars
        std::ostringstream Format_Filled;
        Format_Filled.flush();
        char Format_Command;

        system_time loc_system_time = util::get_system_time();

        // Loop over Format string and build Final Message
        int FormatLen = static_cast<int>(format_current.length());
        for (int x = 0; x < FormatLen; x++) {

            if (format_current[x] == '$' && x + 1 < FormatLen) {

                Format_Command = format_current[x + 1];
                switch (Format_Command) {

                // ------------------------------------  Basic Info  -------------------------------------------------------------------------------
                case 'B':   Format_Filled << console_color_table[(u8)message.msg_sev]; break;                                                                                               // Color Start
                case 'E':   Format_Filled << console_reset; break;                                                                                                                          // Color End
                case 'C':   Format_Filled << message.message; break;                                                                                                                        // input text (message)
                case 'L':   Format_Filled << severity_names[(u8)message.msg_sev]; break;                                                                                                    // Log Level
                case 'X':   if (message.msg_sev == severity::Info || message.msg_sev == severity::Warn) { Format_Filled << " "; } break;                                                    // Alignment
                case 'Z':   Format_Filled << std::endl; break;                                                                                                                              // Alignment

                // ------------------------------------  Source  -------------------------------------------------------------------------------
                case 'Q':   if (thread_lable_map.find(message.thread_id) != thread_lable_map.end()) {Format_Filled << thread_lable_map[message.thread_id]; } else { Format_Filled << message.thread_id; } break;      // Thread id or asosiated lable
                case 'F':   Format_Filled << message.function_name; break;                                                                                                                  // Function Name
                case 'P':   Format_Filled << SHORTEN_FUNC_NAME(message.function_name); break;                                                                                               // Function Name
                case 'A':   Format_Filled << message.file_name; break;                                                                                                                      // File Name
                case 'I':   Format_Filled << get_filename(message.file_name); break;                                                                                                        // Only File Name
                case 'G':   Format_Filled << message.line; break;                                                                                                                           // Line

                // ------------------------------------  Time  -------------------------------------------------------------------------------
                case 'T':   Format_Filled << SETW(2) << (u16)loc_system_time.hour << ":" << SETW(2) << (u16)loc_system_time.minute << ":" << SETW(2) << (u16)loc_system_time.secund; break; // Clock hh:mm:ss
                case 'H':   Format_Filled << SETW(2) << (u16)loc_system_time.hour; break;                                                                                                   // Clock secunde
                case 'M':   Format_Filled << SETW(2) << (u16)loc_system_time.minute; break;                                                                                                 // Clock minute
                case 'S':   Format_Filled << SETW(2) << (u16)loc_system_time.secund; break;                                                                                                 // Clock second
                case 'J':   Format_Filled << SETW(3) << (u16)loc_system_time.millisecends; break;                                                                                           // Clock millisec.

                // ------------------------------------  Date  -------------------------------------------------------------------------------
                case 'N':   Format_Filled << SETW(4) << (u16)loc_system_time.year << "/" << SETW(2) << (u16)loc_system_time.month << "/" << SETW(2) << (u16)loc_system_time.day; break;     // Data yyyy/mm/dd
                case 'Y':   Format_Filled << SETW(4) << (u16)loc_system_time.year; break;                                                                                                   // Year
                case 'O':   Format_Filled << SETW(2) << (u16)loc_system_time.month; break;                                                                                                  // Month
                case 'D':   Format_Filled << SETW(2) << (u16)loc_system_time.day; break;                                                                                                    // Day

                // ------------------------------------  Default  -------------------------------------------------------------------------------
                default: break;
                }

                x++;
            }

            else
                Format_Filled << format_current[x];
        }

        std::lock_guard<std::mutex> file_lock(general_mutex);
        OPEN_MAIN_FILE(true);
        main_file << Format_Filled.str();
        CLOSE_MAIN_FILE()

        if (write_logs_to_console)
            std::cout << Format_Filled.str();
    }

}
