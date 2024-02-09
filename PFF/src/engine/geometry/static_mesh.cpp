
#include "util/pffpch.h"

#include <cstring>
#include <vulkan/vulkan.h>

#include "application.h"
#include "engine/render/renderer.h"
#include "engine/render/vk_device.h"

#include "static_mesh.h"


namespace PFF {

	// best for use static data
	void static_mesh::create_vetex_buffers(const std::vector<vertex>& vertices) {

		m_vertex_count = static_cast<u32>(vertices.size());

		CORE_ASSERT(m_vertex_count >= 3, "", "Vertex count must be at least 3");
		VkDeviceSize buffer_size = sizeof(vertices[0]) * m_vertex_count;
		u32 vertex_size = sizeof(vertices[0]);

		vk_buffer staging_buffer{ m_device, vertex_size, m_vertex_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
		staging_buffer.map();
		staging_buffer.write_to_buffer((void*)vertices.data());

		m_vertex_buffer = std::make_unique<vk_buffer>(m_device, vertex_size, m_vertex_count, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_device->copy_buffer(staging_buffer.get_buffer(), m_vertex_buffer->get_buffer(), buffer_size);

	}

	// best for use static data
	void static_mesh::create_index_buffers(const std::vector<u32>& indices) {

		m_index_count = static_cast<u32>(indices.size());
		m_has_index_buffer = (m_index_count > 0);
		if (!m_has_index_buffer)
			return;

		VkDeviceSize buffer_size = sizeof(indices[0]) * m_index_count;
		u32 index_size = sizeof(indices[0]);

		vk_buffer staging_buffer{ m_device, index_size, m_index_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
		staging_buffer.map();
		staging_buffer.write_to_buffer((void*)indices.data());

		m_vertex_buffer = std::make_unique<vk_buffer>(m_device, index_size, m_index_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_device->copy_buffer(staging_buffer.get_buffer(), m_index_buffer->get_buffer(), buffer_size);
	}


}
