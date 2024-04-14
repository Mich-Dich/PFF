#pragma once

#include "util/pffpch.h"

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define VK_CHECK(expr)		CORE_ASSERT_S(expr == VK_SUCCESS)