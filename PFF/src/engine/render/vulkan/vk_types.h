#pragma once

#include "util/pffpch.h"

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

#include "vendor/vk_mem_alloc.h"
#include "vk_descriptor.h"

struct vk_image {

    VkImage             image;
    VkImageView         image_view;
    VmaAllocation       allocation;
    VkExtent3D          image_extent;
    VkFormat            image_format;
};

struct allocated_buffer {

    VkBuffer            buffer;
    VmaAllocation       allocation;
    VmaAllocationInfo   info;
};

// wierd layout because alignement limitations on GPU
struct vertex {

    glm::vec3           position;
    float               uv_x;
    glm::vec3           normal;
    float               uv_y;
    glm::vec4           color;
};

// holds the resources needed for a mesh
struct GPU_mesh_buffers {

    allocated_buffer    index_buffer;
    allocated_buffer    vertex_buffer;
    VkDeviceAddress     vertex_buffer_address;
};

// push constants for our mesh object draws
struct GPU_draw_push_constants {

    glm::mat4           world_matrix;
    VkDeviceAddress     vertex_buffer;
};


#define VK_CHECK(expr)		CORE_ASSERT_S(expr == VK_SUCCESS)
