#pragma once

#include "util/util.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

struct VkBuffer_T;
struct VkDeviceMemory_T;
struct VkCommandBuffer_T;
struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;


namespace PFF {

	class vk_device;



	class basic_mesh {

	public:

		struct vertex {
			glm::vec2 position;

			static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
		};
	
		basic_mesh(std::shared_ptr<vk_device> device, const std::vector<vertex> vertices);
		~basic_mesh();

		void bind(VkCommandBuffer_T* commandBuffer);
		void draw(VkCommandBuffer_T* commandBuffer);

		DELETE_COPY(basic_mesh)

	private:
		
		void create_vetex_buffers(const std::vector<vertex>& vertices);

		std::shared_ptr<vk_device> m_device;
		VkBuffer_T* m_vertex_buffer;
		VkDeviceMemory_T* m_vertex_buffer_memory;
		u32 m_vertex_count;
	
	};

}
