#pragma once

namespace PFF::render::vulkan {

    struct descriptor_layout_builder {

        std::vector<VkDescriptorSetLayoutBinding> bindings;

        void add_binding(uint32_t binding, VkDescriptorType type);
        void clear();
        VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);
    };

    struct descriptor_allocator {

        struct pool_size_ratio {

            VkDescriptorType type;
            float ratio;
        };

        VkDescriptorPool m_pool;

        void init_pool(VkDevice device, uint32_t maxSets, std::vector<pool_size_ratio> poolRatios);
        void clear_descriptors(VkDevice device);
        void destroy_pool(VkDevice device);
        VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
    };

}