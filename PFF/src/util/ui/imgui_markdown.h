#pragma once

#include "util/pffpch.h"

namespace PFF::UI {

    void markdown(const std::string_view& markdown_text);

    void markdown(const char* markdown_text, size_t markdownLength);

}
