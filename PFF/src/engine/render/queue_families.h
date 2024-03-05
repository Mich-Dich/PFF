#pragma once
#include "util/pffpch.h"

namespace PFF::vk_util {

    struct QueueFamilyIndices {
        u32 graphicsFamily = -1;
        u32 presentFamily = -1;
        bool graphics_family_has_value = false;
        bool present_family_has_value = false;
        FORCEINLINE bool isComplete() const { return graphics_family_has_value && present_family_has_value; }
    };

}