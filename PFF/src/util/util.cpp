
#include <util/pffpch.h>
#include "util.h"

#ifdef PFF_PLATFORM_WINDOWS
    #include <Windows.h>
#elif defined PFF_PLATFORM_LINUX || defined PFF_PLATFORM_MAC
    #include <sys/time.h>
    #include <ctime>
#endif


namespace PFF::util {

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