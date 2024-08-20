#pragma once

#include "util/pffpch.h"

namespace PFF::UI {

    PFF_API void markdown(const std::string_view& markdown_text);

    PFF_API void markdown(const char* markdown_text, size_t markdownLength);

}
