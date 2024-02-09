#pragma once

#include "engine/render/vk_device.h"
#include <vulkan/vulkan.h>

namespace PFF {

    // ==================================================================== Descriptor Set Layout ====================================================================

    class descriptor_set_layout {
    public:

        class builder {
        public:

            builder(vk_device& device) : m_device{ device } {}

            builder& addBinding(u32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, u32 count = 1);
            std::unique_ptr<descriptor_set_layout> build() const;

        private:
            vk_device& m_device;
            std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings{};
        };

        descriptor_set_layout(vk_device& device, std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings);
        ~descriptor_set_layout();

        DELETE_COPY(descriptor_set_layout);

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        vk_device& m_device;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings;

        friend class descriptor_writer;
    };

    // ==================================================================== Descriptor Pool ====================================================================

    class descriptor_pool {
    public:

        class builder {
        public:

            builder(vk_device& device) : m_device{ device } {}

            builder& addPoolSize(VkDescriptorType descriptorType, u32 count);
            builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            builder& setMaxSets(u32 count);
            std::unique_ptr<descriptor_pool> build() const;

        private:
            vk_device& m_device;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            u32 maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        descriptor_pool(vk_device& device, u32 maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~descriptor_pool();

        DELETE_COPY(descriptor_pool);

        bool allocate_descriptor_set(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;
        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
        void resetPool();

    private:
        vk_device& m_device;
        VkDescriptorPool descriptorPool;

        friend class descriptor_writer;
    };

    // ==================================================================== Descriptor Writer ====================================================================

    class descriptor_writer {
    public:

        descriptor_writer(descriptor_set_layout& setLayout, descriptor_pool& pool);

        descriptor_writer& writeBuffer(u32 binding, VkDescriptorBufferInfo* bufferInfo);
        descriptor_writer& writeImage(u32 binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        descriptor_set_layout& setLayout;
        descriptor_pool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace lve