
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
        info.bindingCount = (u32)bindings.size();
        info.flags = 0;

        VkDescriptorSetLayout set;
        VK_CHECK_S(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));
        return set;
    }

    void descriptor_allocator::init_pool(VkDevice device, u32 maxSets, std::vector<pool_size_ratio> poolRatios) {

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (pool_size_ratio ratio : poolRatios) {

            VkDescriptorPoolSize buffer{};
            buffer.type = ratio.type;
            buffer.descriptorCount = static_cast<u32>(ratio.ratio * maxSets);
            poolSizes.push_back(buffer);
        }

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = maxSets;
        pool_info.poolSizeCount = static_cast<u32>(poolSizes.size());
        pool_info.pPoolSizes = poolSizes.data();

        vkCreateDescriptorPool(device, &pool_info, nullptr, &m_pool);
    }

    void descriptor_allocator::clear_descriptors(VkDevice device) { vkResetDescriptorPool(device, m_pool, 0); }

    void descriptor_allocator::destroy_pool(VkDevice device) { vkDestroyDescriptorPool(device, m_pool, nullptr); }

    VkDescriptorSet descriptor_allocator::allocate(VkDevice device, VkDescriptorSetLayout layout) {

        VkDescriptorSetAllocateInfo alloc_I = {};
        alloc_I.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_I.pNext = nullptr;
        alloc_I.descriptorPool = m_pool;
        alloc_I.descriptorSetCount = 1;
        alloc_I.pSetLayouts = &layout;

        VkDescriptorSet ds;
        VK_CHECK_S(vkAllocateDescriptorSets(device, &alloc_I, &ds));
        return ds;
    }

    // ================================================= growable =================================================

    VkDescriptorPool descriptor_allocator_growable::get_pool(VkDevice device) {
        
        VkDescriptorPool newPool;
        if (ready_pools.size() != 0) {
            newPool = ready_pools.back();
            ready_pools.pop_back();
        
        } else {
            //need to create a new pool
            newPool = create_pool(device, sets_per_pool, ratios);

            sets_per_pool = static_cast<u32>(sets_per_pool * 1.5);
            if (sets_per_pool > 4092) 
                sets_per_pool = 4092;
            
        }

        return newPool;
    }

    VkDescriptorPool descriptor_allocator_growable::create_pool(VkDevice device, u32 set_count, std::vector<pool_size_ratio> pool_ratios) {

        std::vector<VkDescriptorPoolSize> poolSizes;
        for (pool_size_ratio ratio : pool_ratios) {

            VkDescriptorPoolSize loc_desc_size{};
            loc_desc_size.type = ratio.type;
            loc_desc_size.descriptorCount = static_cast<u32>(ratio.ratio * set_count);
            poolSizes.push_back(loc_desc_size);
        }

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = 0;
        pool_info.maxSets = set_count;
        pool_info.poolSizeCount = static_cast<u32>(poolSizes.size());
        pool_info.pPoolSizes = poolSizes.data();

        VkDescriptorPool newPool;
        vkCreateDescriptorPool(device, &pool_info, nullptr, &newPool);
        return newPool;
    }
    
    void descriptor_allocator_growable::init(VkDevice device, u32 maxSets, std::vector<pool_size_ratio> poolRatios) {

        ratios.clear();

        for (auto r : poolRatios)
            ratios.push_back(r);

        VkDescriptorPool newPool = create_pool(device, maxSets, poolRatios);
        sets_per_pool = static_cast<u32>(maxSets * 1.5); //grow it next allocation
        ready_pools.push_back(newPool);
    }

    void descriptor_allocator_growable::clear_pools(VkDevice device) {

        for (auto p : ready_pools) 
            vkResetDescriptorPool(device, p, 0);
        
        for (auto p : full_pools) {
            vkResetDescriptorPool(device, p, 0);
            ready_pools.push_back(p);
        }
        full_pools.clear();
    }

    void descriptor_allocator_growable::destroy_pools(VkDevice device) {

        for (auto p : ready_pools) 
            vkDestroyDescriptorPool(device, p, nullptr);
        ready_pools.clear();
        
        for (auto p : full_pools)
            vkDestroyDescriptorPool(device, p, nullptr);
        full_pools.clear();
    }

    VkDescriptorSet descriptor_allocator_growable::allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext) {

        //get or create a pool to allocate from
        VkDescriptorPool poolToUse = get_pool(device);

        VkDescriptorSetAllocateInfo descriptor_set_AI = {};
        descriptor_set_AI.pNext = pNext;
        descriptor_set_AI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptor_set_AI.descriptorPool = poolToUse;
        descriptor_set_AI.descriptorSetCount = 1;
        descriptor_set_AI.pSetLayouts = &layout;

        VkDescriptorSet ds;
        VkResult result = vkAllocateDescriptorSets(device, &descriptor_set_AI, &ds);

        //allocation failed. Try again
        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL) {

            full_pools.push_back(poolToUse);
            poolToUse = get_pool(device);
            descriptor_set_AI.descriptorPool = poolToUse;
            VK_CHECK_S(vkAllocateDescriptorSets(device, &descriptor_set_AI, &ds));    // If the second time fails too stuff is completely broken
        }

        ready_pools.push_back(poolToUse);
        return ds;
    }


    void descriptor_writer::write_image(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type) {

        VkDescriptorImageInfo descriptor_buffer_I = {};
        descriptor_buffer_I.sampler = sampler;
        descriptor_buffer_I.imageView = image;
        descriptor_buffer_I.imageLayout = layout;

        VkDescriptorImageInfo& info = imageInfos.emplace_back(descriptor_buffer_I);

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE; //left empty for now until we need to write it
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = &info;

        writes.push_back(write);
    }


    void descriptor_writer::write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type) {

        VkDescriptorBufferInfo descriptor_buffer_I = {};
        descriptor_buffer_I.buffer = buffer;
        descriptor_buffer_I.offset = offset;
        descriptor_buffer_I.range = size;

        VkDescriptorBufferInfo& info = bufferInfos.emplace_back(descriptor_buffer_I);

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE; //left empty for now until we need to write it
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = &info;

        writes.push_back(write);
    }

    void descriptor_writer::clear() {

        imageInfos.clear();
        writes.clear();
        bufferInfos.clear();
    }

    void descriptor_writer::update_set(VkDevice device, VkDescriptorSet set) {

#if 0
        for (VkWriteDescriptorSet& write : writes)
            write.dstSet = set;
#else
        VALIDATE(!writes.empty(), return, "", "Error: No writes available for updating descriptor set.");
        for (VkWriteDescriptorSet& write : writes) {

            VALIDATE(write.sType == VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, return, "", "Error: Invalid write descriptor set structure type.");    // Check if write is valid
            write.dstSet = set;
        }
#endif

        vkUpdateDescriptorSets(device, static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
    }

}
