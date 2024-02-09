
#include "util/pffpch.h"

#include "vk_descripters.h"

namespace PFF {

    // ==================================================================== Descriptor Set Layout Builder ====================================================================

    descriptor_set_layout::builder& descriptor_set_layout::builder::addBinding(u32 binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, u32 count) {

        PFF_PROFILE_FUNCTION();

        assert(bindings.count(binding) == 0 && "Binding already in use");
        VkDescriptorSetLayoutBinding layoutBinding{};
        layoutBinding.binding = binding;
        layoutBinding.descriptorType = descriptorType;
        layoutBinding.descriptorCount = count;
        layoutBinding.stageFlags = stageFlags;
        bindings[binding] = layoutBinding;
        return *this;
    }

    std::unique_ptr<descriptor_set_layout> descriptor_set_layout::builder::build() const {
        return std::make_unique<descriptor_set_layout>(m_device, bindings);
    }

    // ==================================================================== Descriptor Set Layout ====================================================================

    descriptor_set_layout::descriptor_set_layout(vk_device& device, std::unordered_map<u32, VkDescriptorSetLayoutBinding> bindings)
        : m_device{ device }, bindings{ bindings } {

        PFF_PROFILE_FUNCTION();

        std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
        for (auto kv : bindings) {
            setLayoutBindings.push_back(kv.second);
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.bindingCount = static_cast<u32>(setLayoutBindings.size());
        descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

        CORE_ASSERT_S(vkCreateDescriptorSetLayout(m_device.get_device(), &descriptorSetLayoutInfo, nullptr, &descriptorSetLayout) == VK_SUCCESS);
    }

    descriptor_set_layout::~descriptor_set_layout() {
        vkDestroyDescriptorSetLayout(m_device.get_device(), descriptorSetLayout, nullptr);
    }

    // ==================================================================== Descriptor Pool Builder ====================================================================

    descriptor_pool::builder& descriptor_pool::builder::addPoolSize(VkDescriptorType descriptorType, u32 count) {

        poolSizes.push_back({ descriptorType, count });
        return *this;
    }

    descriptor_pool::builder& descriptor_pool::builder::setPoolFlags(VkDescriptorPoolCreateFlags flags) {

        poolFlags = flags;
        return *this;
    }
    descriptor_pool::builder& descriptor_pool::builder::setMaxSets(u32 count) {

        maxSets = count;
        return *this;
    }

    std::unique_ptr<descriptor_pool> descriptor_pool::builder::build() const {

        return std::make_unique<descriptor_pool>(m_device, maxSets, poolFlags, poolSizes);
    }

    // ==================================================================== Descriptor Pool ====================================================================

    descriptor_pool::descriptor_pool(vk_device& device, u32 maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes)
        : m_device{ device } {

        PFF_PROFILE_FUNCTION();

        VkDescriptorPoolCreateInfo descriptorPoolInfo{};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.flags = poolFlags;

        CORE_ASSERT_S(vkCreateDescriptorPool(m_device.get_device(), &descriptorPoolInfo, nullptr, &descriptorPool) == VK_SUCCESS);
    }

    descriptor_pool::~descriptor_pool() {

        PFF_PROFILE_FUNCTION();

        vkDestroyDescriptorPool(m_device.get_device(), descriptorPool, nullptr);
    }

    bool descriptor_pool::allocate_descriptor_set( const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const {

        PFF_PROFILE_FUNCTION();

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.pSetLayouts = &descriptorSetLayout;
        allocInfo.descriptorSetCount = 1;

        // Might want to create a "DescriptorPoolManager" class that handles this case, and builds
        // a new pool whenever an old pool fills up. But this is beyond our current scope
        CORE_VALIDATE_S(vkAllocateDescriptorSets(m_device.get_device(), &allocInfo, &descriptor) == VK_SUCCESS, return false);
        return true;
    }

    void descriptor_pool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const {

        PFF_PROFILE_FUNCTION();

        vkFreeDescriptorSets(
            m_device.get_device(),
            descriptorPool,
            static_cast<u32>(descriptors.size()),
            descriptors.data());
    }

    void descriptor_pool::resetPool() {

        PFF_PROFILE_FUNCTION();

        vkResetDescriptorPool(m_device.get_device(), descriptorPool, 0);
    }

    // ==================================================================== Descriptor Writer ====================================================================

    descriptor_writer::descriptor_writer(descriptor_set_layout& setLayout, descriptor_pool& pool)
        : setLayout{ setLayout }, pool{ pool } {}

    descriptor_writer& descriptor_writer::writeBuffer(u32 binding, VkDescriptorBufferInfo* bufferInfo) {

        PFF_PROFILE_FUNCTION();

        CORE_ASSERT(setLayout.bindings.count(binding) == 1, "", "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];
        CORE_ASSERT(bindingDescription.descriptorCount == 1, "", "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pBufferInfo = bufferInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    descriptor_writer& descriptor_writer::writeImage(u32 binding, VkDescriptorImageInfo* imageInfo) {

        PFF_PROFILE_FUNCTION();

        CORE_ASSERT(setLayout.bindings.count(binding) == 1, "", "Layout does not contain specified binding");

        auto& bindingDescription = setLayout.bindings[binding];
        CORE_ASSERT(bindingDescription.descriptorCount == 1, "", "Binding single descriptor info, but binding expects multiple");

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = bindingDescription.descriptorType;
        write.dstBinding = binding;
        write.pImageInfo = imageInfo;
        write.descriptorCount = 1;

        writes.push_back(write);
        return *this;
    }

    bool descriptor_writer::build(VkDescriptorSet& set) {

        PFF_PROFILE_FUNCTION();

        bool success = pool.allocate_descriptor_set(setLayout.getDescriptorSetLayout(), set);
        if (!success) {
            return false;
        }
        overwrite(set);
        return true;
    }

    void descriptor_writer::overwrite(VkDescriptorSet& set) {

        PFF_PROFILE_FUNCTION();

        for (auto& write : writes) {
            write.dstSet = set;
        }
        vkUpdateDescriptorSets(pool.m_device.get_device(), static_cast<u32>(writes.size()), writes.data(), 0, nullptr);
    }

}  // namespace lve