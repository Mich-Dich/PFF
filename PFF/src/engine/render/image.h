#pragma once

#include "util/core_config.h"

#if defined PFF_RENDER_API_VULKAN
    #include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF {

    class PFF_API image {
    public:

#if defined PFF_RENDER_API_VULKAN

        image() = default;
        image(void* data, VkExtent3D size, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM, VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT, bool mipmapped = false);
        ~image();

        void allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);

        PFF_DEFAULT_GETTER_SETTER_ALL(VkImage,          image);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkImageView,      image_view);
        PFF_DEFAULT_GETTER_SETTER_ALL(VmaAllocation,    allocation);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkExtent3D,       image_extent);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkFormat,         image_format);

    private:
      
        VkImage             m_image{};
        VkImageView         m_image_view{};
        VmaAllocation       m_allocation{};
        VkExtent3D          m_image_extent{};
        VkFormat            m_image_format{};
#endif

    };

}
