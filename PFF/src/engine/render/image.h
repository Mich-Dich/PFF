#pragma once

#include "util/core_config.h"

#if defined PFF_RENDER_API_VULKAN
    #include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF {
    
    enum class image_format {
        None = 0,
        RGBA,
        RGBA32F
    };

    class image {
    public:

#if defined PFF_RENDER_API_VULKAN

        //PFF_DEFAULT_CONSTRUCTORS(image);
        //PFF_DEFAULT_COPY_CONSTRUCTOR(image);

        image() = default;
        image(void* data, VkExtent3D size, image_format format, bool mipmapped = false);
        image(void* data, u32 width, u32 height, image_format format, bool mipmapped = false);
        image(std::filesystem::path image_path, image_format format, bool mipmapped = false);
        ~image();

        PFF_DEFAULT_GETTER_SETTER_ALL(VkImage,          image);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkImageView,      image_view);
        PFF_DEFAULT_GETTER_SETTER_ALL(VmaAllocation,    allocation);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkExtent3D,       image_extent);
        PFF_DEFAULT_GETTER_SETTER_ALL(VkFormat,         image_format);
        FORCEINLINE u32 get_width()                     const { return m_image_extent.width; }
        FORCEINLINE u32 get_height()                    const { return m_image_extent.height; }
        VkDescriptorSet get_descriptor_set();
        VkDescriptorSet generate_descriptor_set(VkSampler sampler, VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        static void* decode(const void* data, u64 length, u32& outWidth, u32& outHeight);
        static void* load(std::filesystem::path path, u64 length, u32& outWidth, u32& outHeight);

    private:

        void allocate_memory(void* data, VkExtent3D size, image_format format, bool mipmapped = false, VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT);
        void allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false);
        void release();

        bool                m_is_initalized = false;
        VkImage             m_image = VK_NULL_HANDLE;
        VkImageView         m_image_view = VK_NULL_HANDLE;
        VmaAllocation       m_allocation = nullptr;
        VkExtent3D          m_image_extent{};
        VkFormat            m_image_format{};
        VkDescriptorSet     m_descriptor_set = VK_NULL_HANDLE;
        //VkSampler           m_sampler = nullptr;

#endif
    };

}
