
#include "util/pffpch.h"

#include <cstring>
#include <vulkan/vulkan.h>

#include "application.h"
#include "engine/render/renderer.h"
#include "engine/render/vk_device.h"

// TODO: REMOVE THIS (make seperate load/sive file)
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#define TINYOBJLOADER_USE_MAPBOX_EARCUT			// gives robust trinagulation. Requires C++11
#include <tiny_obj_loader.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include "basic_mesh.h"

namespace std {

	template<>
	struct hash<PFF::basic_mesh::vertex> {
		size_t operator()(PFF::basic_mesh::vertex const& vertex) const {

			size_t seed = 0;
			PFF::util::hash_combine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace PFF {

	basic_mesh::basic_mesh(const basic_mesh::builder& builder) {

		PFF_PROFILE_FUNCTION();

		m_device = application::get().get_renderer()->get_device();
		create_vetex_buffers(builder.vertices);
		create_index_buffers(builder.indices);
	}

	basic_mesh::~basic_mesh() {

		PFF_PROFILE_FUNCTION();

		m_device.reset();
		CORE_LOG(Trace, "Shutdown");
	}

	ref<basic_mesh> basic_mesh::create_mesh_from_file(const std::string& file_path) {

		PFF_PROFILE_FUNCTION();

		builder loc_builder{};
		loc_builder.load_mesh(file_path);

		return std::make_shared<basic_mesh>(loc_builder);
	}

	void basic_mesh::bind(VkCommandBuffer_T* commandBuffer) {

		PFF_PROFILE_FUNCTION();

		VkBuffer buffers[] = { m_vertex_buffer->get_buffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (m_has_index_buffer)
			vkCmdBindIndexBuffer(commandBuffer, m_index_buffer->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void basic_mesh::draw(VkCommandBuffer_T* commandBuffer) {

		PFF_PROFILE_FUNCTION();

		if (m_has_index_buffer)
			vkCmdDrawIndexed(commandBuffer, m_index_count, 1, 0, 0, 0);
		else
			vkCmdDraw(commandBuffer, m_vertex_count, 1, 0, 0);
	}

	// best for use static data
	void basic_mesh::create_vetex_buffers(const std::vector<vertex>& vertices) {

		PFF_PROFILE_FUNCTION();

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
	void basic_mesh::create_index_buffers(const std::vector<u32>& indices) {

		PFF_PROFILE_FUNCTION();

		m_index_count = static_cast<u32>(indices.size());
		m_has_index_buffer = (m_index_count > 0);
		if (!m_has_index_buffer)
			return;

		VkDeviceSize buffer_size = sizeof(indices[0]) * m_index_count;
		u32 index_size = sizeof(indices[0]);

		vk_buffer staging_buffer{ m_device, index_size, m_index_count, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT };
		staging_buffer.map();
		staging_buffer.write_to_buffer((void*)indices.data());

		m_index_buffer = std::make_unique<vk_buffer>(m_device, index_size, m_index_count, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		m_device->copy_buffer(staging_buffer.get_buffer(), m_index_buffer->get_buffer(), buffer_size);
	}

	std::vector<VkVertexInputBindingDescription> basic_mesh::vertex::get_binding_descriptions() {

		PFF_PROFILE_FUNCTION();

		std::vector<VkVertexInputBindingDescription> binding_desc(1);
		binding_desc[0].binding = 0;
		binding_desc[0].stride = sizeof(vertex);
		binding_desc[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return binding_desc;
	}

	std::vector<VkVertexInputAttributeDescription> basic_mesh::vertex::get_attribute_descriptions() {

		PFF_PROFILE_FUNCTION();

		std::vector<VkVertexInputAttributeDescription> attribut_desc{};

		attribut_desc.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(vertex, position) });
		attribut_desc.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(vertex, color) });
		attribut_desc.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT , offsetof(vertex, normal) });
		attribut_desc.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT , offsetof(vertex, uv) });

		return attribut_desc;
	}

	void basic_mesh::builder::load_mesh(const std::string& file_path) {

		PFF_PROFILE_FUNCTION();

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> material;
		std::string warn, err;

		CORE_LOG(Trace, "Loading mesh [" << file_path << "]");
		CORE_ASSERT(tinyobj::LoadObj(&attrib, &shapes, &material, &warn, &err, file_path.c_str()), "loaded", warn + err);

		vertices.clear();
		indices.clear();

		std::unordered_map<vertex, u32> unique_verts{};
		for (const auto& loc_shape : shapes) {
			for (const auto& index : loc_shape.mesh.indices) {

				vertex vertex{};
				if (index.vertex_index >= 0) {

					vertex.position = {
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color = {
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0) {
					vertex.normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0) {
					vertex.uv = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if (unique_verts.count(vertex) == 0) {
					unique_verts[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}
				indices.push_back(unique_verts[vertex]);
				//vertices.push_back(vertex);
			}
		}

		CORE_LOG(Info, "finished calculating index_buffer, vertex count [" << vertices.size() << "] indexes count [" << indices.size() << "]");
	}


}
