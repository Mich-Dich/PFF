
#include "util/pffpch.h"

#include "vk_types.h"

#include "vk_descriptor.h"

namespace PFF::render::vulkan {

    void descriptor_layout_builder::add_binding(uint32_t binding, VkDescriptorType type) {
        
        VkDescriptorSetLayoutBinding newbind{};
        newbind.binding = binding;
        newbind.descriptorCount = 1;
        newbind.descriptorType = type;
        bindings.push_back(newbind);
    }

    void descriptor_layout_builder::clear() { bindings.clear(); }

    VkDescriptorSetLayout descriptor_layout_builder::build(VkDevice device, VkShaderStageFlags shaderStages) {
     
        // TIPP: We are first looping through the bindings and adding the stage-flags. 
        //       For each of the descriptor bindings within a descriptor set, they can be different between the fragment shader and vertex shader. 
        //       Currently not supporting per-binding stage flags, and instead force it to be for the whole descriptor set.
        for (auto& b : bindings)
            b.stageFlags |= shaderStages;

        VkDescriptorSetLayoutCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        info.pNext = nullptr;
        info.pBindings = bindings.data();
        info.bindingCount = (uint32_t)bindings.size();
        info.flags = 0;

        VkDescriptorSetLayout set;
        VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));
        return set;
    }

    void descriptor_allocator::init_pool(VkDevice device, uint32_t maxSets, std::vector<pool_size_ratio> poolRatios) {

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (pool_size_ratio ratio : poolRatios) {

            VkDescriptorPoolSize buffer{};
            buffer.type = ratio.type;
            buffer.descriptorCount = uint32_t(ratio.ratio * maxSets);
            poolSizes.push_back(buffer);
        }

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = maxSets;
        pool_info.poolSizeCount = (uint32_t)poolSizes.size();
        pool_info.pPoolSizes = poolSizes.data();

        vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
    }

    void descriptor_allocator::clear_descriptors(VkDevice device) { vkResetDescriptorPool(device, pool, 0); }

    void descriptor_allocator::destroy_pool(VkDevice device) { vkDestroyDescriptorPool(device, pool, nullptr); }

    VkDescriptorSet descriptor_allocator::allocate(VkDevice device, VkDescriptorSetLayout layout) {

        VkDescriptorSetAllocateInfo alloc_I = {};
        alloc_I.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_I.pNext = nullptr;
        alloc_I.descriptorPool = pool;
        alloc_I.descriptorSetCount = 1;
        alloc_I.pSetLayouts = &layout;

        VkDescriptorSet ds;
        VK_CHECK(vkAllocateDescriptorSets(device, &alloc_I, &ds));
        return ds;
    }

}