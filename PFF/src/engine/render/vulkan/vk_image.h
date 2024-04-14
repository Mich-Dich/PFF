#pragma once 

#include <vulkan/vulkan.h>

namespace PFF::render::vulkan::util {

	void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);

}
