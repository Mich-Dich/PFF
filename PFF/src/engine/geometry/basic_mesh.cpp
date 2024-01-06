
#include "util/pffpch.h"

#include <cstring>
#include <vulkan/vulkan.h>

#include "engine/render/vk_device.h"

#include "basic_mesh.h"



namespace PFF {



	basic_mesh::basic_mesh(std::shared_ptr<vk_device> device, const std::vector<vertex> vertices)
		:m_device{ device } {

		create_vetex_buffers(vertices);
	}

	basic_mesh::~basic_mesh() {
	
		vkDestroyBuffer(m_device->get_device(), m_vertex_buffer, nullptr);
		vkFreeMemory(m_device->get_device(), m_vertex_buffer_memory, nullptr);
	}

	void basic_mesh::bind(VkCommandBuffer_T* commandBuffer) {

		VkBuffer buffers[] = { m_vertex_buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void basic_mesh::draw(VkCommandBuffer_T* commandBuffer) {

		vkCmdDraw(commandBuffer, m_vertex_count, 1, 0, 0);
	}

	void basic_mesh::create_vetex_buffers(const std::vector<vertex>& vertices) {

		m_vertex_count = static_cast<u32>(vertices.size());

		CORE_ASSERT(m_vertex_count >= 3, "", "Vertex count must be at least 3");
		VkDeviceSize buffer_size = sizeof(vertices[0]) * m_vertex_count;
		m_device->create_buffer(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vertex_buffer, m_vertex_buffer_memory);

		void* data;
		vkMapMemory(m_device->get_device(), m_vertex_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(buffer_size));
		vkUnmapMemory(m_device->get_device(), m_vertex_buffer_memory);

	}

	std::vector<VkVertexInputBindingDescription> basic_mesh::vertex::get_binding_descriptions() {

		std::vector<VkVertexInputBindingDescription> binding_desc(1);
		binding_desc[0].binding = 0;
		binding_desc[0].stride = sizeof(vertex);
		binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return binding_desc;
	}

	std::vector<VkVertexInputAttributeDescription> basic_mesh::vertex::get_attribute_descriptions() {
		
		std::vector<VkVertexInputAttributeDescription> attribut_desc(2);
		attribut_desc[0].location = 0;
		attribut_desc[0].binding = 0;
		attribut_desc[0].format = VK_FORMAT_R32G32_SFLOAT;
		attribut_desc[0].offset = offsetof(vertex, position);

		attribut_desc[1].location = 1;
		attribut_desc[1].binding = 0;
		attribut_desc[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribut_desc[1].offset = offsetof(vertex, color);
		return attribut_desc;
	}

}
