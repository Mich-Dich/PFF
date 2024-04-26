#pragma once

#include "vendor/VkBootstrap.h"

namespace PFF::render::vulkan {

	VkDevice create_device(VkSurfaceKHR surface, VkPhysicalDevice& physical_device) {
		/*
		// vulkan 1.3 features
		VkPhysicalDeviceVulkan13Features features{};
		features.dynamicRendering = true;
		features.synchronization2 = true;

		// vulkan 1.2 features
		VkPhysicalDeviceVulkan12Features features12{};
		features12.bufferDeviceAddress = true;
		features12.descriptorIndexing = true;

		// select a gpu that can write to GLFW-surface and supports vulkan 1.3 with the correct features
		vkb::PhysicalDeviceSelector selector{ vkb_inst };
		vkb::PhysicalDevice physicalDevice = selector
			.set_minimum_version(1, 3)
			.set_required_features_13(features)
			.set_required_features_12(features12)
			.set_surface(surface)
			.select()
			.value();

		// create the final vulkan device
		vkb::DeviceBuilder deviceBuilder{ physicalDevice };
		vkb::Device vkbDevice = deviceBuilder.build().value();

		physical_device = physicalDevice.physical_device;
		return vkbDevice.device;
		*/
	}

}