#pragma once

#include <vulkan/vulkan.h>

namespace PFF::renderer::util {


	void create_shader_module(const std::string& file_path, VkDevice decive,  VkShaderModule* shader_module) {

		PFF_PROFILE_FUNCTION();

		std::vector<char> shader_code;
		CORE_ASSERT_S(io_handler::read_file(file_path, shader_code));

		VkShaderModuleCreateInfo SM_CI{};
		SM_CI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		SM_CI.codeSize = shader_code.size();
		SM_CI.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

		CORE_ASSERT_S(vkCreateShaderModule(decive, &SM_CI, nullptr, shader_module) == VK_SUCCESS);

	}

}