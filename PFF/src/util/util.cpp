
#include <util/pffpch.h>

#ifdef PFF_PLATFORM_WINDOWS
    #include <Windows.h>
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
    #include <sys/time.h>
    #include <ctime>
#endif


#include "util.h"

namespace PFF::util {

    void extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter) {

        size_t found = input.find_last_of(delimiter);
        if (found != std::string::npos) {

            dest = input.substr(found + 1);
            return;
        }

        return; // If delimiter is not found
    }

    void extract_part_befor_delimiter(std::string& dest, const std::string& input, const char* delimiter) {

        size_t found = input.find_last_of(delimiter);
        if (found != std::string::npos) {

            dest = input.substr(0, found);
            return;
        }

        return; // If delimiter is not found
    }

    std::string extract_vaiable_name(const std::string& input) {

        std::string result = input;
        extract_part_after_delimiter(result, input, "->");
        extract_part_after_delimiter(result, result, ".");

        return result;
    }
    

    void high_precision_sleep(f32 duration_in_milliseconds) {

        static const f32 estimated_deviation = 10.0f;
        auto loc_duration_in_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::duration<f32>(duration_in_milliseconds) ).count();
        auto target_time = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(static_cast<int>(loc_duration_in_milliseconds));
        
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(loc_duration_in_milliseconds - estimated_deviation)));

        // Busy wait for the remaining time
        while (std::chrono::high_resolution_clock::now() < target_time)
            ;

        //auto actual_sleep_time = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::high_resolution_clock::now() - target_time + std::chrono::milliseconds(static_cast<int>(duration_in_milliseconds)) ).count();
        //CORE_LOG(Debug, "left over time: " << actual_sleep_time << " ms");
    }

    system_time get_system_time() {

        system_time loc_system_time{};

#ifdef PFF_PLATFORM_WINDOWS

        SYSTEMTIME win_time;
        GetLocalTime(&win_time);
        loc_system_time.year = static_cast<unsigned short>(win_time.wYear);
        loc_system_time.month = static_cast<unsigned short>(win_time.wMonth);
        loc_system_time.day = static_cast<unsigned short>(win_time.wDay);
        loc_system_time.day_of_week = static_cast<unsigned short>(win_time.wDayOfWeek);
        loc_system_time.hour = static_cast<unsigned short>(win_time.wHour);
        loc_system_time.minute = static_cast<unsigned short>(win_time.wMinute);
        loc_system_time.secund = static_cast<unsigned short>(win_time.wSecond);
        loc_system_time.millisecends = static_cast<unsigned short>(win_time.wMilliseconds);

#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC

        struct timeval tv;
        gettimeofday(&tv, NULL);
        struct tm* ptm = localtime(&tv.tv_sec);
        loc_system_time.year = static_cast<unsigned short>(ptm->tm_year + 1900);
        loc_system_time.month = static_cast<unsigned short>(ptm->tm_mon + 1);
        loc_system_time.day = static_cast<unsigned short>(ptm->tm_mday);
        loc_system_time.day_of_week = static_cast<unsigned short>(ptm->tm_wday);
        loc_system_time.hour = static_cast<unsigned short>(ptm->tm_hour);
        loc_system_time.minute = static_cast<unsigned short>(ptm->tm_min);
        loc_system_time.secund = static_cast<unsigned short>(ptm->tm_sec);
        loc_system_time.millisecends = static_cast<unsigned short>(tv.tv_usec / 1000);

#endif
        return loc_system_time;
    }

    void util::cancel_timer(timer& timer) {

        timer.shared_state->first = true;
        timer.shared_state->second.notify_one();
        if (timer.future.valid()) {
            timer.future.wait();
        }
    }

    std::string add_spaces(const u32 multiple_of_indenting_spaces, u32 num_of_indenting_spaces) {

        if (multiple_of_indenting_spaces == 0)
            return "";

        return std::string(multiple_of_indenting_spaces * num_of_indenting_spaces, ' ');
    }


    u32 measure_indentation(const std::string& str, u32 num_of_indenting_spaces) {

        u32 count = 0;
        for (char ch : str) {
            if (ch == ' ')
                count++;
            else
                break; // Stop counting on non-space characters
        }

        return count / num_of_indenting_spaces;
    }

    int count_lines(const char* text) {

        if (text[0] == '\0')
            return 1;

        int line_count = 0;
        for (int i = 0; i < 256; ++i) {
            if (text[i] == '\0')
                break;

            if (text[i] == '\n')
                ++line_count;
        }

        // If the last character is not a newline and the string is not empty, count the last line
        if (text[0] != '\0' && text[255] != '\n')
            ++line_count;

        return line_count;
    }

}