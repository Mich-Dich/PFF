#pragma once

namespace PFF::render::vulkan {

	namespace util {
	
		// @brief Consider abstracting the path to work from a folder set in a config file, and only select the shader (e.g: combute.comp => internally looking in correct folder) (platform agnostic)
		bool load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);
	}
}