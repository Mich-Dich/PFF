
#include "util/pffpch.h"

#include "vk_types.h"
#include "vk_initializers.h"

#include "vk_pipeline.h"

namespace PFF::render::vulkan {


    bool util::load_shader_module(const char* filePath, VkDevice device, VkShaderModule* outShaderModule) {

        // open the file. With cursor at the end
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            return false;

        // find what the size of the file is by looking up the location of the cursor
        // because the cursor is at the end, it gives the size directly in bytes
        size_t fileSize = (size_t)file.tellg();

        // spirv expects the buffer to be on uint32, so make sure to reserve a int
        // vector big enough for the entire file
        std::vector<u32> buffer(fileSize / sizeof(u32));

        file.seekg(0);
        file.read((char*)buffer.data(), fileSize);
        file.close();

        // create a new shader module, using the buffer we loaded
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = nullptr;

        // codeSize has to be in bytes, so multply the ints in the buffer by size of
        // int to know the real size of the buffer
        createInfo.codeSize = buffer.size() * sizeof(u32);
        createInfo.pCode = buffer.data();

        // check that the creation goes well.
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            return false;

        *outShaderModule = shaderModule;
        return true;
    }
}
