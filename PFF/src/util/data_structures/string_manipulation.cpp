
#include "util/pffpch.h"

#include "string_manipulation.h"

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
