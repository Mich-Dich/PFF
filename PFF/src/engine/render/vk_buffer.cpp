
#include "util/pffpch.h"

#include "vk_buffer.h"

namespace PFF {

    // ==================================================================== setup ====================================================================

    //
    vk_buffer::vk_buffer(ref<vk_device> device, VkDeviceSize m_instance_size, u32 m_instance_count, VkBufferUsageFlags m_usage_flags, VkMemoryPropertyFlags m_memory_property_flags, VkDeviceSize minOffsetAlignment)
        : m_device{ device }, m_instance_size{ m_instance_size }, m_instance_count{ m_instance_count }, m_usage_flags{ m_usage_flags }, m_memory_property_flags{ m_memory_property_flags } {

        PFF_PROFILE_FUNCTION();

        m_alignment_size = get_alignment(m_instance_size, minOffsetAlignment);
        m_buffer_size = m_alignment_size * m_instance_count;
        m_device->create_buffer(m_buffer_size, m_usage_flags, m_memory_property_flags, m_buffer, m_memory);
    }

    //
    vk_buffer::~vk_buffer() {

        PFF_PROFILE_FUNCTION();

        unmap();
        vkDestroyBuffer(m_device->get_device(), m_buffer, nullptr);
        vkFreeMemory(m_device->get_device(), m_memory, nullptr);
    }

    // ==================================================================== public ====================================================================

    //
    VkResult vk_buffer::map(VkDeviceSize size, VkDeviceSize offset) {

        PFF_PROFILE_FUNCTION();

        assert(m_buffer && m_memory && "Called map on buffer before create");
        return vkMapMemory(m_device->get_device(), m_memory, offset, size, 0, &m_mapped);
    }

    //
    void vk_buffer::unmap() {

        PFF_PROFILE_FUNCTION();

        if (m_mapped) {
            vkUnmapMemory(m_device->get_device(), m_memory);
            m_mapped = nullptr;
        }
    }

    //
    void vk_buffer::write_to_buffer(void* data, VkDeviceSize size, VkDeviceSize offset) {

        PFF_PROFILE_FUNCTION();

        assert(m_mapped && "Cannot copy to unmapped buffer");

        if (size == VK_WHOLE_SIZE) {
            memcpy(m_mapped, data, m_buffer_size);
        } else {
            char* memOffset = (char*)m_mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    //
    VkResult vk_buffer::flush(VkDeviceSize size, VkDeviceSize offset) {

        PFF_PROFILE_FUNCTION();

        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_device->get_device(), 1, &mappedRange);
    }

    //
    VkResult vk_buffer::invalidate(VkDeviceSize size, VkDeviceSize offset) {

        PFF_PROFILE_FUNCTION();

        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_device->get_device(), 1, &mappedRange);
    }

    //
    VkDescriptorBufferInfo vk_buffer::descriptor_info(VkDeviceSize size, VkDeviceSize offset) {

        return VkDescriptorBufferInfo{ m_buffer, offset, size };
    }

    //
    void vk_buffer::write_to_index(void* data, int index) {

        PFF_PROFILE_FUNCTION();

        write_to_buffer(data, m_instance_size, index * m_alignment_size);
    }

    //
    VkResult vk_buffer::flush_index(int index) { return flush(m_alignment_size, index * m_alignment_size); }

    //
    VkDescriptorBufferInfo vk_buffer::descriptor_info_for_index(int index) {

        PFF_PROFILE_FUNCTION();

        return descriptor_info(m_alignment_size, index * m_alignment_size);
    }

    //
    VkResult vk_buffer::invalidate_index(int index) {

        PFF_PROFILE_FUNCTION();

        return invalidate(m_alignment_size, index * m_alignment_size);
    }

    // ==================================================================== private ====================================================================

    VkDeviceSize vk_buffer::get_alignment(VkDeviceSize m_instance_size, VkDeviceSize minOffsetAlignment) {

        PFF_PROFILE_FUNCTION();

        if (minOffsetAlignment > 0)
            return (m_instance_size + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);

        return m_instance_size;
    }


}
