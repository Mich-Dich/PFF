#pragma once
#include "util/pffpch.h"

namespace PFF::vk_util {

    struct QueueFamilyIndices {
        u32 graphicsFamily;
        u32 presentFamily;
        bool graphicsFamilyHasValue = false;
        bool presentFamilyHasValue = false;
        bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
    };

}