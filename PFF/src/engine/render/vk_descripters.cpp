
#include "util/pffpch.h"

#include "vk_descripters.h"

namespace PFF {

    // ==================================================================== Descriptor Set Layout Builder ====================================================================

    vk_descriptor_set_layout::builder& vk_descriptor_set_layout::builder::add_binding(u32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, u32 count) {

        PFF_PROFILE_RENDER_FUNCTION();

        CORE_ASSERT(m_bindings.count(binding) == 0, "", "Binding already in use");

        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        // layoutBinding.pImmutableSamplers = nullptr;
        m_bindings[binding] = layoutBinding;
        return *this;
    }

    scope_ref<vk_descriptor_set_layout> vk_descriptor_set_layout::builder::build() const {
        return std::make_unique<vk_descriptor_set_layout>(m_device, m_bindings);
    }

    // ==================================================================== Descriptor Set Layout ====================================================================

    vk_descriptor_set_layout::vk_descriptor_set_layout(ref<vk_device> device, std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings)
        : m_device{ device }, m_bindings{ bindings } {

        PFF_PROFILE_RENDER_FUNCTION();

        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo DSL_CI{};
        DSL_CI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        DSL_CI.bindingCount = static_cast<u32>(setLayoutBindings.size());
        DSL_CI.pBindings = setLayoutBindings.data();

        CORE_ASSERT_S(vkCreateDescriptorSetLayout(m_device->get_device(), &DSL_CI, nullptr, &m_descriptor_set_layout) == VK_SUCCESS);
    }

    vk_descriptor_set_layout::~vk_descriptor_set_layout() {
        vkDestroyDescriptorSetLayout(m_device->get_device(), m_descriptor_set_layout, nullptr);
    }

    // ==================================================================== Descriptor Pool Builder ====================================================================

    vk_descriptor_pool::builder& vk_descriptor_pool::builder::addPoolSize(VkDescriptorType descriptor_type, u32 count) {

        m_pool_sizes.push_back({ descriptor_type, count });
        return *this;
    }

    vk_descriptor_pool::builder& vk_descriptor_pool::builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {

        m_pool_flags = flags;
        return *this;
    }
    vk_descriptor_pool::builder& vk_descriptor_pool::builder::setMaxSets(u32 count) {

        m_maxSets = count;
        return *this;
    }

    scope_ref<vk_descriptor_pool> vk_descriptor_pool::builder::build() const {

        return std::make_unique<vk_descriptor_pool>(m_device, m_maxSets, m_pool_flags, m_pool_sizes);
    }

    // ==================================================================== Descriptor Pool ====================================================================

    vk_descriptor_pool::vk_descriptor_pool(ref<vk_device> device, u32 max_sets, VkDescriptorPoolCreateFlags pool_flags, const std::vector<VkDescriptorPoolSize>& pool_sizes)
        : m_device{ device } {

        PFF_PROFILE_RENDER_FUNCTION();

        VkDescriptorPoolCreateInfo DP_CI{};
        DP_CI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        DP_CI.poolSizeCount = static_cast<u32>(pool_sizes.size());
        DP_CI.pPoolSizes = pool_sizes.data();
        DP_CI.maxSets = max_sets * static_cast<u32>(pool_sizes.size());
        DP_CI.flags = pool_flags;

        CORE_ASSERT_S(vkCreateDescriptorPool(m_device->get_device(), &DP_CI, nullptr, &m_descriptor_pool) == VK_SUCCESS);
    }

    vk_descriptor_pool::~vk_descriptor_pool() {

        PFF_PROFILE_RENDER_FUNCTION();

        vkDestroyDescriptorPool(m_device->get_device(), m_descriptor_pool, nullptr);
    }

    bool vk_descriptor_pool::allocate_descriptor_set( const VkDescriptorSetLayout descriptor_set_layout, VkDescriptorSet& descriptor) const {

        PFF_PROFILE_RENDER_FUNCTION();

        VkDescriptorSetAllocateInfo CS_AI{};
        CS_AI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        CS_AI.descriptorPool = m_descriptor_pool;
        CS_AI.pSetLayouts = &descriptor_set_layout;
        CS_AI.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds a new pool whenever an old pool fills up. But this is beyond current scope
        CORE_VALIDATE_S(vkAllocateDescriptorSets(m_device->get_device(), &CS_AI, &descriptor) == VK_SUCCESS, return false);
        return true;
    }

    void vk_descriptor_pool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {

        PFF_PROFILE_RENDER_FUNCTION();

        vkFreeDescriptorSets(
            m_device->get_device(),
            m_descriptor_pool,
            static_cast<u32>(descriptors.size()),
            descriptors.data());
    }

    void vk_descriptor_pool::resetPool() {

        PFF_PROFILE_RENDER_FUNCTION();

        vkResetDescriptorPool(m_device->get_device(), m_descriptor_pool, 0);
    }

    // ==================================================================== Descriptor Writer ====================================================================

    vk_descriptor_writer::vk_descriptor_writer(vk_descriptor_set_layout& setLayout, vk_descriptor_pool& pool)
        : m_set_layout{ setLayout }, m_pool{ pool } {}

    vk_descriptor_writer& vk_descriptor_writer::write_buffer(u32 binding, VkDescriptorBufferInfo* buffer_info) {

        PFF_PROFILE_RENDER_FUNCTION();

        CORE_ASSERT(m_set_layout.m_bindings.count(binding) == 1, "", "Layout does not contain specified binding");

        auto& bindingDescription = m_set_layout.m_bindings[binding];
        CORE_ASSERT(bindingDescription.descriptorCount == 1, "", "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = buffer_info;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    vk_descriptor_writer& vk_descriptor_writer::write_image(u32 binding, VkDescriptorImageInfo* image_info) {

        PFF_PROFILE_RENDER_FUNCTION();

        CORE_ASSERT(m_set_layout.m_bindings.count(binding) == 1, "", "Layout does not contain specified binding");

        auto& bindingDescription = m_set_layout.m_bindings[binding];
        CORE_ASSERT(bindingDescription.descriptorCount == 1, "", "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = image_info;
        write.descriptorCount = 1;

        m_writes.push_back(write);
        return *this;
    }

    bool vk_descriptor_writer::build(VkDescriptorSet& set) {

        PFF_PROFILE_RENDER_FUNCTION();

        bool success = m_pool.allocate_descriptor_set(m_set_layout.get_descriptor_set_layout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void vk_descriptor_writer::overwrite(VkDescriptorSet& set) {

        PFF_PROFILE_RENDER_FUNCTION();

        for (auto& write : m_writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(m_pool.m_device->get_device(), static_cast<u32>(m_writes.size()), m_writes.data(), 0, nullptr);
    }

}
