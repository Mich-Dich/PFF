#pragma once 

#include <vulkan/vulkan.h>

namespace PFF::render::vulkan::util {

	void transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout);

	//@brief Vulkan has 2 main ways of copying one image to another. you can use VkCmdCopyImage or VkCmdBlitImage. 
	// CopyImage is faster, but its much more restricted, for example the resolution on both images must match. 
	// Meanwhile, blit image lets you copy images of different formats and different sizes into one another. 
	// You have a source rectangle and a target rectangle, and the system copies it into its position. 
	// Those two functions are useful when setting up the engine, but later its best to ignore them and write your own version that can do extra logic on a fullscreen fragment shader.
	void copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);

}
