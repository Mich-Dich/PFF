#include <util/pffpch.h>
#include "util/util.h"

#include "logger.h"



namespace PFF::logger {

    #define SETW(width)                                         std::setw(width) << std::setfill('0')
    #define INTERNAL_LOG(message)                               { std::ostringstream oss{}; oss << message; log_string(std::move(oss.str())); }

    #define LOGGER_UPDATE_FORMAT                                "LOGGER update format"
    #define LOGGER_REVERSE_FORMAT                               "LOGGER reverse format"
    #define LOGGER_CHANGE_THRESHHOLD                            "LOGGER change threshhold"
    #define LOGGER_CHANGE_BUFFER_SIZE                           "LOGGER change buffer size"
    #define LOGGER_REGISTER_THREAD_LABLE                        "LOGGER register thread label"
    #define LOGGER_UNREGISTER_THREAD_LABLE                      "LOGGER unregister thread label"
    #define QUEUE_MAX_SIZE                                      512
    
    #define OPEN_FILE                                           main_file = std::ofstream(main_log_file_path, std::ios::app);                       \
                                                                if (!main_file.is_open()) {                                                         \
                                                                    std::cerr << "Failed to open main log file path: [" << main_log_file_path << "]" << std::endl;                      \
                                                                    std::quick_exit(1);                                                             \
                                                                }

    #define CLOSE_FILE                                          if (main_file.is_open()) { main_file.close(); }
    #define WRITE_TO_FILE(message)                              { OPEN_FILE main_file << message; CLOSE_FILE}

    static bool                                                 is_init = false;
    static bool                                                 write_log_to_console = false;
    static std::string                                          format_current = "";
    static std::string                                          format_prev = "";

    static severity                                             severity_level_buffering_threshhold = severity::Trace;
    static size_t                                               buffer_size = 1024;
    static std::string                                          buffered_messages{};

    const std::string                                           severity_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    const std::string                                           console_rest = "\x1b[0m";
    const std::string                                           console_color_table[] = {
        "\x1b[38;5;246m",                                           // Trace: Gray
        "\x1b[94m",                                                 // Debug: Blue
        "\x1b[92m",                                                 // Info: Green
        "\x1b[33m",                                                 // Warn: Yellow
        "\x1b[31m",                                                 // Error: Red
        "\x1b[41m\x1b[30m",                                         // Fatal: Red Background
    };

    static std::filesystem::path                                main_log_dir = "";
    static std::filesystem::path                                main_log_file_path = "";
    static std::ofstream                                        main_file{};

    struct message_format {
        message_format(const logger::severity msg_sev, const char* file_name, const char* function_name, const int line, std::thread::id thread_id, const std::string& message) 
            : msg_sev(msg_sev), file_name(file_name), function_name(function_name), line(line), thread_id(thread_id), message(message) {};

        const logger::severity                                  msg_sev;
        const char*                                             file_name;
        const char*                                             function_name;
        const int                                               line;
        const std::thread::id                                   thread_id;
        const std::string                                       message;
    };

    static std::queue<message_format>                           log_queue{};
    static std::unordered_map<std::thread::id, std::string>     thread_labels{};
    static std::mutex                                           queue_mutex{};
    static std::mutex                                           general_mutex{};
    static std::condition_variable                              cv{};
    static std::atomic<bool>                                    stop = false;
    static std::thread                                          worker_thread{};

    void process_log_message(const message_format&& message);
    void process_queue();

    inline void log_string(std::string&& log_str, const bool force_direct_logging = true) {

        if (!(force_direct_logging || (buffered_messages.capacity() - buffered_messages.size()) <= log_str.size())) {

            buffered_messages.append(log_str);
            return;
        }
        
        OPEN_FILE
        main_file << buffered_messages << log_str;
        CLOSE_FILE

        if (write_log_to_console) 
            std::cout << buffered_messages << log_str;

        buffered_messages.clear();
    }

    inline const char* get_filename(const char* filepath) {

        const char* filename = std::strrchr(filepath, '\\');
        if (filename == nullptr)
            filename = std::strrchr(filepath, '/');

        if (filename == nullptr)
            return filepath;  // No path separator found, return the whole string

        return filename + 1;  // Skip the path separator
    }

    // ========================================================================================================================
    // init / shutdown
    // ========================================================================================================================

    bool init(const std::string& format, const bool log_to_console, const std::filesystem::path log_dir, const std::string& main_log_file_name, const bool use_append_mode) {

        if (is_init) {
            std::cerr << "Tryed to init logger system multiple times" << std::endl;
            std::quick_exit(1);
        }

        format_current = format;
        format_prev = format;
        write_log_to_console = log_to_console;

        main_log_dir = std::filesystem::absolute(log_dir);
        main_log_file_path = main_log_dir / main_log_file_name;

        if (!std::filesystem::is_directory(main_log_dir))
            if (!std::filesystem::create_directory(main_log_dir)) {
                std::cerr << "Failed to create the directory for log files" << std::endl;
                std::quick_exit(1);
            }

        OPEN_FILE
        main_file << "\n========================================================\n";
        auto now = std::time(nullptr);
        auto tm = *std::localtime(&now);
        main_file << "Log initalized at [" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "]\n";
        main_file << "========================================================\n";
        CLOSE_FILE

        buffered_messages.reserve(buffer_size);

        is_init = true;

        worker_thread = std::thread(&process_queue);                                                        // start after inital write to avoid using mutex

        return true;
    }

    void shutdown() {

        if (!is_init) {
            std::cerr << "Tried to shutdown logger bevor initilization" << std::endl;
            std::quick_exit(1);
        }

        stop = true;
        cv.notify_all();
        if (worker_thread.joinable())
            worker_thread.join();

        // Process any remaining messages in the queue after worker thread has stopped
        std::queue<message_format> remaining_messages;
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            remaining_messages = std::move(log_queue); // Take all remaining messages
        }

        while (!remaining_messages.empty()) {
            message_format msg = std::move(remaining_messages.front());
            remaining_messages.pop();
            process_log_message(std::move(msg)); // Process each message
        }

        if ( !buffered_messages.empty()) {
            OPEN_FILE
            main_file << buffered_messages;
            CLOSE_FILE

            if (write_log_to_console)
                std::cout << buffered_messages;
        }

        is_init = false;
    }    

    // ========================================================================================================================
    // settings
    // ========================================================================================================================


    void set_format(const std::string& new_format) {        // needed to insert this into the queue to preserve the order

        if (!is_init) {        
    		std::cerr << "Tryed to set logger format bevor logger was initalized" << std::endl;
            return;
        }
        
        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(severity::Trace, "", LOGGER_UPDATE_FORMAT, 0, std::thread::id(), new_format.c_str());
        cv.notify_all();
    }

    void use_previous_format() {
        
        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(severity::Trace, "", LOGGER_REVERSE_FORMAT, 0, std::thread::id(), "");
        cv.notify_all();
    }

    const std::string get_format() { return format_current; }

    void register_label_for_thread(const std::string& thread_lable, std::thread::id thread_id) {

        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(severity::Trace, "", LOGGER_REGISTER_THREAD_LABLE, 0, thread_id, thread_lable);
        cv.notify_all();
    }

    void unregister_label_for_thread(std::thread::id thread_id) {

        std::ostringstream loc_oss{};
        {
            std::lock_guard<std::mutex> lock(general_mutex);
            if (thread_labels.find(thread_id) == thread_labels.end())
                loc_oss << "[LOGGER] Tried to unregister lable for unknown thread with ID: [" << thread_id << "]. IGNORED";
        }

        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(severity::Trace, "", LOGGER_UNREGISTER_THREAD_LABLE, 0, thread_id, std::move(loc_oss.str()));
        cv.notify_all();
    }

    void set_buffer_threshhold(const severity new_threshhold) {

        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(new_threshhold, "", LOGGER_CHANGE_THRESHHOLD, 0, std::thread::id(), "[LOGGER] Changed buffering threshhold to [" + severity_names[static_cast<u8>(severity_level_buffering_threshhold)] + "]");
        cv.notify_all();
    }

    void set_buffer_size(const size_t new_size) {

        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(severity::Trace, "", LOGGER_CHANGE_BUFFER_SIZE, static_cast<int>(new_size), std::thread::id(), "[LOGGER] Changed buffer size to [" + std::to_string(new_size) + "]");
        cv.notify_all();
    }

    // ========================================================================================================================
    // message queue
    // ========================================================================================================================

    void process_queue() {

        std::unique_lock<std::mutex> lock(queue_mutex);
        while (!stop) {

            cv.wait(lock, [] { return !log_queue.empty() || stop; });
            // if (stop)
            //     break;
    
            std::queue<message_format> local_queue;
            while (!log_queue.empty()) {                                    // Move all current messages to a local queue
                local_queue.push(std::move(log_queue.front()));
                log_queue.pop();
            }
            lock.unlock();                                                  // Unlock while processing messages
    
            // Process each message from the local queue
            while (!local_queue.empty()) {
                message_format message = std::move(local_queue.front());
                local_queue.pop();
                // Process control messages and log messages

                if (strcmp(message.function_name, LOGGER_UPDATE_FORMAT) == 0) {

                    std::lock_guard<std::mutex> lock(general_mutex);
                    format_prev = format_current;
                    format_current = static_cast<std::string>(message.message);

                    WRITE_TO_FILE("[LOGGER] Changing log-format. From [" << format_prev << "] to [" << format_current << "]\n");
                
                } else if (strcmp(message.function_name, LOGGER_REVERSE_FORMAT) == 0) {
                    
                    std::lock_guard<std::mutex> lock(general_mutex);
                    const std::string buffer = format_current;
                    format_current = format_prev;
                    format_prev = buffer;

                } else if (strcmp(message.function_name, LOGGER_CHANGE_THRESHHOLD) == 0) {

                    std::lock_guard<std::mutex> lock(general_mutex);
                    severity_level_buffering_threshhold = static_cast<severity>(std::min(static_cast<u8>(message.msg_sev), static_cast<u8>(severity::Error)));   

                }
                else if (strcmp(message.function_name, LOGGER_CHANGE_BUFFER_SIZE) == 0) {

                    std::lock_guard<std::mutex> lock(general_mutex);
                    buffer_size = static_cast<size_t>(message.line);

                    OPEN_FILE
                    main_file << message.message;                    
                    if (is_init && buffered_messages.size() >= buffer_size) {                   // Handle buffer overflow if the new size is smaller than the current buffer content
                        
                        main_file << buffered_messages;
                        if (write_log_to_console)
                        std::cout << buffered_messages;
                        
                        buffered_messages.clear();
                    }
                    CLOSE_FILE
                
                    buffered_messages.shrink_to_fit();
                    buffered_messages.reserve(buffer_size);
                
                } else if (strcmp(message.function_name, LOGGER_REGISTER_THREAD_LABLE) == 0) {            // process_reverse_in_msg_format();

                    std::lock_guard<std::mutex> lock(general_mutex);
                    
                    if (thread_labels.find(message.thread_id) != thread_labels.end())
                    WRITE_TO_FILE("[LOGGER] Thread with ID: [" << message.thread_id << "] already has lable [" << thread_labels[message.thread_id] << "] registered. Overriding with the lable: [" << message.message << "]\n")
                    else
                    WRITE_TO_FILE("[LOGGER] Registering Thread-ID: [" << message.thread_id << "] with the lable: [" << message.message << "]\n")

                    thread_labels[message.thread_id] = message.message;
                
                } else if (strcmp(message.function_name, LOGGER_UNREGISTER_THREAD_LABLE) == 0) {

                    std::lock_guard<std::mutex> lock(general_mutex);
                    thread_labels.erase(message.thread_id);
                }

                else
                    process_log_message(std::move(message));
            }
    
            // Re-lock before next iteration
            lock.lock();
        }
    }

    // ========================================================================================================================
    // handle message
    // ========================================================================================================================

    void log_msg(const severity msg_sev, const char* file_name, const char* function_name, const int line, std::thread::id thread_id, std::string&& message) {

        if (message.empty())
            return;

        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.emplace(msg_sev, file_name, function_name, line, thread_id, message);

        if (static_cast<u8>(msg_sev) >= static_cast<u8>(severity_level_buffering_threshhold) || log_queue.size() >= QUEUE_MAX_SIZE)           // check if thread should be notified
            cv.notify_all();

    }

    void process_log_message(const message_format&& message) {

#define SHORTEN_FUNC_NAME(text)                                 (strstr(text, "::") ? strstr(text, "::") + 2 : text)

        // create helper vars
        std::ostringstream format_filled{};
        format_filled.flush();
        char format_command{};
        system_time loc_sys_time = util::get_system_time();

        // loop over format string and build final message
        std::unique_lock<std::mutex> lock(general_mutex);
        int format_length = static_cast<int>(format_current.length());
        for (int x = 0; x < format_length; x++) {

            if (format_current[x] == '$' && x+1 < format_length) {          // detected a format specifier prefix

                format_command = format_current[x + 1];
                switch (format_command) {

                // ------------------------ Basic info ------------------------
                case 'B': format_filled << console_color_table[(u8)message.msg_sev]; break;                                             // Color start
                case 'E': format_filled << console_rest; break;                                                                         // Color end
                case 'C': format_filled << message.message; break;                                                                      // input text (message)
                case 'L': format_filled << severity_names[(u8)message.msg_sev]; break;                                                  // log severity
                case 'X': if(message.msg_sev == severity::Info || message.msg_sev == severity::Warn) {format_filled << " "; } break;    // alignment
                case 'Z': format_filled << "\n"; break;                                                                                 // line breake
                
                // ------------------------ Basic info ------------------------
                case 'Q':   if (thread_labels.find(message.thread_id) != thread_labels.end()) {format_filled << thread_labels[message.thread_id]; } 
                            else { format_filled << message.thread_id; } break;                                                         // Thread id or asosiated lable
                case 'F': format_filled << message.function_name; break;                                                                // function name
                case 'P': format_filled << SHORTEN_FUNC_NAME(message.function_name); break;                                             // short function name
                case 'A': format_filled << message.file_name; break;                                                                    // file name
                case 'I': format_filled << get_filename(message.file_name); break;                                                      // short file name
                case 'G': format_filled << message.line; break;                                                                         // line
                
                // ------------------------ time ------------------------
                case 'T': format_filled << SETW(2) << (u16)loc_sys_time.hour << ":" << SETW(2) << (u16)loc_sys_time.minute << ":" << SETW(2) << (u16)loc_sys_time.secund; break;       // formated time
                case 'H': format_filled << SETW(2) << (u16)loc_sys_time.hour; break;                                                                                                    // hour
                case 'M': format_filled << SETW(2) << (u16)loc_sys_time.minute; break;                                                                                                  // minute
                case 'S': format_filled << SETW(2) << (u16)loc_sys_time.secund; break;                                                                                                 // second
                case 'J': format_filled << SETW(3) << (u16)loc_sys_time.millisecend; break;                                                                                            // miliseond

                // ------------------------ data ------------------------
                case 'N': format_filled << SETW(4) << (u16)loc_sys_time.year << "/" << SETW(2) << (u16)loc_sys_time.month <<"/" << SETW(2) << (u16)loc_sys_time.day; break;             // data yy/mm/dd
                case 'Y': format_filled << SETW(4) << (u16)loc_sys_time.year; break;                                                                                                    // year
                case 'O': format_filled << SETW(2) << (u16)loc_sys_time.month; break;                                                                                                    // month
                case 'D': format_filled << SETW(2) << (u16)loc_sys_time.day; break;                                                                                                     // day

                default: break;
                }

                x++;
            }
            
            else
                format_filled << format_current[x];
        }

        std::string log_str = format_filled.str();
        if (!((static_cast<u8>(message.msg_sev) >= static_cast<u8>(severity_level_buffering_threshhold)) || (buffered_messages.capacity() - buffered_messages.size()) <= log_str.size())) {

            buffered_messages.append(log_str);
            return;
        }
        
        OPEN_FILE
        main_file << buffered_messages << log_str;
        CLOSE_FILE

        if (write_log_to_console) 
            std::cout << buffered_messages << log_str;

        buffered_messages.clear();
    }

}
