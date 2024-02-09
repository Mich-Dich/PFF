
#include "util/pffpch.h"

#include <cstring>
#include <vulkan/vulkan.h>

#include "application.h"
#include "engine/render/renderer.h"
#include "engine/render/vk_device.h"

#include "dynamic_mesh.h"


namespace PFF {
	/*
	// best for use static data
	void dynamic_mesh::create_vetex_buffers(const std::vector<vertex>& vertices) {

		m_vertex_count = static_cast<u32>(vertices.size());
		CORE_ASSERT(m_vertex_count >= 3, "", "Vertex count must be at least 3");

		VkDeviceSize buffer_size = sizeof(vertices[0]) * m_vertex_count;
		m_device->create_buffer(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vertex_buffer->getBuffer(), m_vertex_buffer_memory);

		void* data;
		vkMapMemory(m_device->get_device(), m_vertex_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
		vkUnmapMemory(m_device->get_device(), m_vertex_buffer_memory);
	}

	// best for use static data
	void dynamic_mesh::create_index_buffers(const std::vector<u32>& indices) {

		m_index_count = static_cast<u32>(indices.size());
		m_has_index_buffer = (m_index_count > 0);
		if (!m_has_index_buffer)
			return;

		VkDeviceSize buffer_size = sizeof(indices[0]) * m_index_count;
		m_device->create_buffer(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_index_buffer, m_index_buffer_memory);

		void* data;
		vkMapMemory(m_device->get_device(), m_index_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(buffer_size));
		vkUnmapMemory(m_device->get_device(), m_index_buffer_memory);
	}
*/
}
