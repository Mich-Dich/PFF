#pragma once

#include "util/core_config.h"

#if defined PFF_RENDER_API_VULKAN
    #include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF {

    enum class PFF_API image_format {
        None = 0,
        RGBA,
        RGBA32F
    };
    

    class PFF_API image {
    public:

#if defined PFF_RENDER_API_VULKAN

        //PFF_DEFAULT_CONSTRUCTORS(image);
        //PFF_DEFAULT_COPY_CONSTRUCTOR(image);

        image() = default;
        image(void* data, VkExtent3D size, image_format format, bool mipmapped = false);
        image(void* data, u32 width, u32 height, image_format format, bool mipmapped = false);
        ~image();

        PFF_DEFAULT_GETTER_SETTER_ALL(VkImage,          image);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkImageView,      image_view);
        PFF_DEFAULT_GETTER_SETTER_ALL(VmaAllocation,    allocation);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkExtent3D,       image_extent);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkFormat,         image_format);
        FORCEINLINE u32 get_width()                     const { return m_image_extent.width; }
        FORCEINLINE u32 get_height()                    const { return m_image_extent.height; }
        VkDescriptorSet get_descriptor_set();

        //friend class render::vulkan::vk_renderer;

    private:

        void allocate_memory(void* data, VkExtent3D size, image_format format, bool mipmapped = false, VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT);
        void allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);

        bool                m_is_initalized = false;
        VkImage             m_image = nullptr;
        VkImageView         m_image_view = nullptr;
        VmaAllocation       m_allocation = nullptr;
        VkExtent3D          m_image_extent{};
        VkFormat            m_image_format{};
        VkDescriptorSet     m_descriptor_set = nullptr;
        //VkSampler           m_sampler = nullptr;

#endif
    };

    //class render::vulkan::vk_renderer;
}
