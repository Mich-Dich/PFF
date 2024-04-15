#pragma once

#include "util/pffpch.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "vendor/vk_mem_alloc.h"
#include "vk_descriptor.h"

struct vk_image {

    VkImage         image;
    VkImageView     image_view;
    VmaAllocation   allocation;
    VkExtent3D      image_extent;
    VkFormat        image_format;
};

#define VK_CHECK(expr)		CORE_ASSERT_S(expr == VK_SUCCESS)
