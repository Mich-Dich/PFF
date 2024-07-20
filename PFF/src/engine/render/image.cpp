
#include "util/pffpch.h"

#if defined PFF_RENDER_API_VULKAN
	#include <vulkan/vulkan.h>
	#include "vulkan/vk_image.h"
	#include "vulkan/vk_initializers.h"

	#include "imgui_impl_vulkan.h"
#endif

#include "application.h"

#include "image.h"

#define GET_RENDERER					application::get().get_renderer()

namespace PFF {

#if defined PFF_RENDER_API_VULKAN

	void image::allocate_image(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {

		set_image_format(format);
		set_image_extent(size);

		VkImageCreateInfo img_info = render::vulkan::init::image_create_info(format, usage, size);
		if (mipmapped)
			img_info.mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(size.width, size.height)))) + 1;

		// always allocate images on dedicated GPU memory
		VmaAllocationCreateInfo allocinfo = {};
		allocinfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocinfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		VK_CHECK(vmaCreateImage(GET_RENDERER->get_allocator(), &img_info, &allocinfo, get_image_pointer(), get_allocation_pointer(), nullptr));

		// if the format is a depth format, we will need to have it use the correct aspect flag
		VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
		if (format == VK_FORMAT_D32_SFLOAT)
			aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;

		// build a image-view for the image
		VkImageViewCreateInfo view_info = render::vulkan::init::imageview_create_info(format, get_image(), aspectFlag);
		view_info.subresourceRange.levelCount = img_info.mipLevels;
		VK_CHECK(vkCreateImageView(GET_RENDERER->get_device(), &view_info, nullptr, get_image_view_pointer()));
	}

	VkDescriptorSet image::get_descriptor_set() {

		if (m_descriptor_set == nullptr)
			m_descriptor_set = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(GET_RENDERER->get_default_sampler_nearest(), m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		return m_descriptor_set;
	};

	image::image(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped) {

		size_t data_size = size.depth * size.width * size.height * 4;
		vk_buffer uploadbuffer = GET_RENDERER->create_buffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		memcpy(uploadbuffer.info.pMappedData, data, data_size);
		allocate_image(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, mipmapped);

		GET_RENDERER->immediate_submit([&](VkCommandBuffer cmd) {

			render::vulkan::util::transition_image(cmd, get_image(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.bufferImageHeight = 0;
			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.mipLevel = 0;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageExtent = size;

			vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, get_image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);
			render::vulkan::util::transition_image(cmd, get_image(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			//m_descriptor_set = (VkDescriptorSet)ImGui_ImplVulkan_AddTexture(GET_RENDERER->get_texture_sampler_nearest(), m_image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		});

		GET_RENDERER->destroy_buffer(uploadbuffer);
	}

	image::~image() {

		//vkDestroyImageView(GET_RENDERER->get_device(), get_image_view(), nullptr);
		//vmaDestroyImage(GET_RENDERER->get_allocator(), get_image(), get_allocation());
	}

#endif


}