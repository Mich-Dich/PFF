#pragma once

#include "engine/render/vk_device.h"
#include <vulkan/vulkan.h>

namespace PFF {

    // ==================================================================== Descriptor Set Layout ====================================================================

    class vk_descriptor_set_layout {
    public:

        class builder {
        public:

            builder(std::shared_ptr<vk_device> device) : m_device{ device } {}

            builder& add_binding(u32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, u32 count = 1);
            std::unique_ptr<vk_descriptor_set_layout> build() const;

        private:
            std::shared_ptr<vk_device> m_device;
            std::unordered_map<u32, VkDescriptorSetLayoutBinding> m_bindings{};
        };

        vk_descriptor_set_layout(std::shared_ptr<vk_device> device, std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings);
        ~vk_descriptor_set_layout();

        DELETE_COPY(vk_descriptor_set_layout);

        VkDescriptorSetLayout get_descriptor_set_layout() const { return m_descriptor_set_layout; }

    private:
        std::shared_ptr<vk_device> m_device;
        VkDescriptorSetLayout m_descriptor_set_layout;
        std::unordered_map<u32, VkDescriptorSetLayoutBinding> m_bindings;

        friend class vk_descriptor_writer;
    };

    // ==================================================================== Descriptor Pool ====================================================================

    class vk_descriptor_pool {
    public:

        class builder {
        public:

            builder(std::shared_ptr<vk_device> device) : m_device{ device } {}

            builder& addPoolSize(VkDescriptorType descriptor_type, u32 count);
            builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            builder& setMaxSets(u32 count);
            std::unique_ptr<vk_descriptor_pool> build() const;

        private:
            std::shared_ptr<vk_device> m_device;
            std::vector<VkDescriptorPoolSize> m_pool_sizes{};
            u32 m_maxSets = 1000;
            VkDescriptorPoolCreateFlags m_pool_flags = 0;
        };

        vk_descriptor_pool(std::shared_ptr<vk_device> device, u32 max_sets, VkDescriptorPoolCreateFlags pool_flags, const std::vector<VkDescriptorPoolSize>& pool_sizes);
        ~vk_descriptor_pool();

        DELETE_COPY(vk_descriptor_pool);

        FORCEINLINE VkDescriptorPool get_descriptorPool() const { return m_descriptor_pool; }

        bool allocate_descriptor_set(const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const;
        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;
        void resetPool();

    private:
        std::shared_ptr<vk_device> m_device;
        VkDescriptorPool m_descriptor_pool;

        friend class vk_descriptor_writer;
    };

    // ==================================================================== Descriptor Writer ====================================================================

    class vk_descriptor_writer {
    public:

        vk_descriptor_writer(vk_descriptor_set_layout& setLayout, vk_descriptor_pool& pool);

        vk_descriptor_writer& write_buffer(u32 binding, VkDescriptorBufferInfo* buffer_info);
        vk_descriptor_writer& write_image(u32 binding, VkDescriptorImageInfo* image_info);

        // Create a DescriptorSet and store handle into [set] var then cann [overwrite()] to update the resources the descripter should point to
        bool build(VkDescriptorSet& set);

        // update descriptors in the set by writing all the previously recorded write commands to the target set
        void overwrite(VkDescriptorSet& set);

    private:
        vk_descriptor_set_layout& m_set_layout;
        vk_descriptor_pool& m_pool;
        std::vector<VkWriteDescriptorSet> m_writes;
    };

}  // namespace lve