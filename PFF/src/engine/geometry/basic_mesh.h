#pragma once

struct VkBuffer_T;
struct VkDeviceMemory_T;
struct VkCommandBuffer_T;
struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

#include "engine/render/vk_buffer.h"

namespace PFF {

	class vk_device;



	class PFF_API basic_mesh {

	public:

		struct vertex {
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();

			bool operator==(const vertex& other) const {
				return (position == other.position && color == other.color && normal == other.normal && uv == other.uv);
			}
		};
	
		struct builder {
			std::vector<vertex> vertices{};
			std::vector<u32> indices{};

			void load_mesh(const std::string& file_path);
		};

		basic_mesh(const basic_mesh::builder& builder);
		~basic_mesh();

		static std::shared_ptr<basic_mesh> create_mesh_from_file(const std::string& file_path);

		void bind(VkCommandBuffer_T* commandBuffer);
		void draw(VkCommandBuffer_T* commandBuffer);

		DELETE_COPY(basic_mesh);

	protected:

		std::shared_ptr<vk_device> m_device{};

		std::unique_ptr<vk_buffer> m_vertex_buffer{};
		u32 m_vertex_count{};

		bool m_has_index_buffer = false;
		std::unique_ptr<vk_buffer> m_index_buffer{};
		u32 m_index_count{};
		
	private:

		virtual void create_vetex_buffers(const std::vector<vertex>& vertices);
		virtual void create_index_buffers(const std::vector<u32>& indices);


	};

}
