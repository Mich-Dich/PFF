#pragma once

namespace PFF::render::vulkan {

    struct descriptor_layout_builder {

        std::vector<VkDescriptorSetLayoutBinding> bindings;

        void add_binding(u32 binding, VkDescriptorType type);
        void clear();
        VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);
    };

    struct descriptor_allocator {

        struct pool_size_ratio {
            VkDescriptorType type;
            float ratio;
        };

        VkDescriptorPool m_pool;

        void init_pool(VkDevice device, u32 maxSets, std::vector<pool_size_ratio> poolRatios);
        void clear_descriptors(VkDevice device);
        void destroy_pool(VkDevice device);
        VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);
    };

    struct descriptor_allocator_growable {
    public:
      
        struct PoolSizeRatio {
            VkDescriptorType type;
            float ratio;
        };

        void init(VkDevice device, u32 initialSets, std::vector<PoolSizeRatio> poolRatios);
        void clear_pools(VkDevice device);
        void destroy_pools(VkDevice device);
        VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);

    private:

        VkDescriptorPool get_pool(VkDevice device);
        VkDescriptorPool create_pool(VkDevice device, u32 set_count, std::vector<PoolSizeRatio> pool_ratios);

        std::vector<PoolSizeRatio> ratios;
        std::vector<VkDescriptorPool> full_pools;
        std::vector<VkDescriptorPool> ready_pools;
        u32 sets_per_pool = 1;

    };

    struct descriptor_writer {

        std::deque<VkDescriptorImageInfo> imageInfos;
        std::deque<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkWriteDescriptorSet> writes;

        void write_image(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);

        // Allowed types:
        //      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
        //      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        //      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
        //      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC
        void write_buffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

        void clear();
        void update_set(VkDevice device, VkDescriptorSet set);
    };

}