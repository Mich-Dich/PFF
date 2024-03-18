#pragma once

#include "vk_device.h"

namespace PFF {

    class vk_buffer {
    public:
        vk_buffer(ref<vk_device> device, VkDeviceSize m_instance_size, u32 m_instance_count, VkBufferUsageFlags m_usage_flags,
            VkMemoryPropertyFlags m_memory_property_flags, VkDeviceSize minOffsetAlignment = 1);

        ~vk_buffer();

        DELETE_COPY(vk_buffer);

        //Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
        //@param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete buffer range.
        //@param offset (Optional) Byte offset from beginning
        //@return VkResult of the buffer mapping call
        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        //Unmap a mapped memory range
        //@note Does not return a result as vkUnmapMemory can't fail
        void unmap();

        //Copies the specified data to the mapped buffer. Default value writes whole buffer range
        //@param data Pointer to the data to copy
        //@param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer range.
        //@param offset (Optional) Byte offset from beginning of mapped region
        void write_to_buffer(void* data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        //Flush a memory range of the buffer to make it visible to the device
        //@note Only required for non-coherent memory
        //@param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the complete buffer range.
        //@param offset (Optional) Byte offset from beginning
        //@return VkResult of the flush call
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        //Create a buffer info descriptor
        //@param size (Optional) Size of the memory range of the descriptor
        //@param offset (Optional) Byte offset from beginning
        //@return VkDescriptorBufferInfo of specified offset and range
        VkDescriptorBufferInfo descriptor_info(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        //Invalidate a memory range of the buffer to make it visible to the host
        //@note Only required for non-coherent memory
        //@param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate the complete buffer range.
        //@param offset (Optional) Byte offset from beginning
        //@return VkResult of the invalidate call
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);


        //Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
        //@param data Pointer to the data to copy
        //@param index Used in offset calculation
        void write_to_index(void* data, int index);

        // Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
        //@param index Used in offset calculation
        VkResult flush_index(int index);

        //Create a buffer info descriptor
        //@param index Specifies the region given by index * alignmentSize
        //@return VkDescriptorBufferInfo for instance at index
        VkDescriptorBufferInfo descriptor_info_for_index(int index);

        //Invalidate a memory range of the buffer to make it visible to the host
        //@note Only required for non-coherent memory
        //@param index Specifies the region to invalidate: index * alignmentSize
        //@return VkResult of the invalidate call
        VkResult invalidate_index(int index);

        VkBuffer get_buffer() const { return m_buffer; }
        void* get_mapped_memory() const { return m_mapped; }
        uint32_t get_instance_count() const { return m_instance_count; }
        VkDeviceSize get_instance_size() const { return m_instance_size; }
        VkDeviceSize get_alignment_size() const { return m_instance_size; }
        VkBufferUsageFlags get_usage_flags() const { return m_usage_flags; }
        VkMemoryPropertyFlags get_memory_property_flags() const { return m_memory_property_flags; }
        VkDeviceSize get_buffer_size() const { return m_buffer_size; }

    private:

        // Returns the minimum instance size required to be compatible with devices minOffsetAlignment
        // @param instanceSize The size of an instance
        // @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg. minUniformBufferOffsetAlignment)
        // @return VkResult of the buffer mapping call
        static VkDeviceSize get_alignment(VkDeviceSize m_instance_size, VkDeviceSize minOffsetAlignment);

        ref<vk_device> m_device;
        void* m_mapped = nullptr;
        VkBuffer m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;

        VkDeviceSize m_buffer_size;
        uint32_t m_instance_count;
        VkDeviceSize m_instance_size;
        VkDeviceSize m_alignment_size;
        VkBufferUsageFlags m_usage_flags;
        VkMemoryPropertyFlags m_memory_property_flags;
    };

}
