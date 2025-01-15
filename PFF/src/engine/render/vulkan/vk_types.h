#pragma once

#include "util/pffpch.h"

#include "vendor/vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "vk_descriptor.h"
//#include "engine/geometry/mesh.h"

//class image {
//public:
//
//    //PFF_DEFAULT_CONSTRUCTORS(image);
//
//    //friend class PFF::render::vulkan::vk_renderer;
////private:
//
//    VkImage             m_image{};
//    VkImageView         m_image_view{};
//    VmaAllocation       m_allocation{};
//    VkExtent3D          m_image_extent{};
//    VkFormat            m_image_format{};
//};

namespace PFF::render::vulkan {

    struct vk_buffer {

        VkBuffer            buffer;
        VmaAllocation       allocation;
        VmaAllocationInfo   info;
    };

    // push constants for our mesh object draws
    struct GPU_draw_push_constants {

        glm::mat4           world_matrix;
        VkDeviceAddress     vertex_buffer;
    };

}

#define VK_CHECK_S(expr)		                        ASSERT_S(expr == VK_SUCCESS)
#define VK_CHECK(expr, successMsg, failureMsg)		    ASSERT(expr == VK_SUCCESS)
