
#include "util/pffpch.h"

#if defined PFF_RENDER_API_VULKAN
	#include <vulkan/vulkan.h>
	#include "vulkan/vk_image.h"
	#include "vulkan/vk_initializers.h"

	#include "imgui_impl_vulkan.h"
	#include "engine/render/vulkan/vk_types.h"
#endif

#include "application.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image.h"


namespace PFF {

	namespace util {

		static u32 bytes_per_pixel(image_format format) {

			switch (format) {
				case image_format::RGBA:    return 4;
				case image_format::RGBA32F: return 16;
				default: return 0;
			}
		}

	}

#if defined PFF_RENDER_API_VULKAN

	namespace util {

		static uint32_t get_vulkan_memory_type(VkMemoryPropertyFlags properties, u32 type_bits) {
			
			VkPhysicalDeviceMemoryProperties prop;
			vkGetPhysicalDeviceMemoryProperties(GET_RENDERER.get_chosenGPU(), &prop);
			for (uint32_t i = 0; i < prop.memoryTypeCount; i++) {
				if ((prop.memoryTypes[i].propertyFlags & properties) == properties && type_bits & (1 << i))
					return i;
			}

			return 0xffffffff;
		}

		static VkFormat image_format_to_vulkan_vormat(image_format format) {
		
			switch (format) {
				case image_format::RGBA:    return VK_FORMAT_R8G8B8A8_UNORM;
				case image_format::RGBA32F: return VK_FORMAT_R32G32B32A32_SFLOAT;
				default: return (VkFormat)0;
			}
		}

	}

	image::image(void* data, VkExtent3D size, image_format format, bool mipmapped) {

		allocate_memory(data, size, format, mipmapped);
	}

	image::image(void* data, u32 width, u32 height, image_format format, bool mipmapped) {

		allocate_memory(data, VkExtent3D{width, height, 1}, format, mipmapped);
	}

	image::image(std::filesystem::path image_path, image_format format, bool mipmapped) {

		int channels;
		int width = 0, height = 0;
		void* data = stbi_load(image_path.string().c_str(), &width, &height, &channels, 4);
		allocate_memory(data, VkExtent3D{ (u32)width, (u32)height, 1 }, format, mipmapped);
		stbi_image_free(data);
	}

	image::~image() {
		
		release();
		CORE_LOG_SHUTDOWN();
	}

	void image::allocate_memory(void* data, VkExtent3D size, image_format format, bool mipmapped, VkImageUsageFlags usage) {

		size_t data_size = size.depth * size.width * size.height * util::bytes_per_pixel(format);
		vk_buffer uploadbuffer = GET_RENDERER.create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		memcpy(uploadbuffer.info.pMappedData, data, data_size);
		allocate_image(size, util::image_format_to_vulkan_vormat(format), usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

		GET_RENDERER.immediate_submit([&](VkCommandBuffer cmd) {

			render::vulkan::util::transition_image(cmd, m_image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;
			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = size;

			vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
			render::vulkan::util::transition_image(cmd, m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

		GET_RENDERER.destroy_buffer(uploadbuffer);
		m_is_initalized = true;
	}

	void image::allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {

		set_image_format(format);
		set_image_extent(size);

		VkImageCreateInfo img_info = render::vulkan::init::image_create_info(format, usage, size);
		if (mipmapped)
			img_info.mipLevels = static_cast<u32>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

		// always allocate images on dedicated GPU memory
		VmaAllocationCreateInfo allocinfo = {};
		allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK_S(vmaCreateImage(GET_RENDERER.get_allocator(), &img_info, &allocinfo, &m_image, &m_allocation, nullptr));

		// if the format is a depth format, we will need to have it use the correct aspect flag
		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		if (format == VK_FORMAT_D32_SFLOAT)
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

		// build a image-view for the image
		VkImageViewCreateInfo view_info = render::vulkan::init::imageview_create_info(format, m_image, aspectFlag);
		view_info.subresourceRange.levelCount = img_info.mipLevels;
		VK_CHECK_S(vkCreateImageView(GET_RENDERER.get_device(), &view_info, nullptr, &m_image_view));

		m_is_initalized = true;
	}

	void image::release() {

		if (m_is_initalized) {

			GET_RENDERER.submit_resource_free([image = m_image, image_view = m_image_view, allocation = m_allocation, descriptor_set = m_descriptor_set] {

				vkDestroyImageView(GET_RENDERER.get_device(), image_view, nullptr);
				vmaDestroyImage(GET_RENDERER.get_allocator(), image, allocation);

				if (descriptor_set != nullptr)
					ImGui_ImplVulkan_RemoveTexture(descriptor_set);
			});

			m_image = VK_NULL_HANDLE;
			m_image_view = VK_NULL_HANDLE;
			m_allocation = nullptr;
			m_descriptor_set = VK_NULL_HANDLE;
			m_is_initalized = false;
		}
	}

	VkDescriptorSet image::get_descriptor_set() {

		if (m_descriptor_set == nullptr)
			m_descriptor_set = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(GET_RENDERER.get_default_sampler_linear(), m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		return m_descriptor_set;
	}

	VkDescriptorSet image::generate_descriptor_set(VkSampler sampler, VkImageLayout layout) {

		return m_descriptor_set = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(sampler, m_image_view, layout);
	}

	void* image::decode(const void* data, u64 length, u32& outWidth, u32& outHeight) {

		int width, height, channels;
		u8* buffer = nullptr;
		u16 size = 0;

		buffer = stbi_load_from_memory((const stbi_uc*)data, (int)length, &width, &height, &channels, 4);
		size = width * height * 4;

		outWidth = width;
		outHeight = height;

		return buffer;
	}

	void* image::load(std::filesystem::path path, u64 length, u32& outWidth, u32& outHeight) {

		//stbi_load();

		return nullptr;
	}

#endif

}
