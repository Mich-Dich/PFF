#pragma once

#include "basic_mesh.h"

struct VkBuffer_T;
struct VkDeviceMemory_T;
struct VkCommandBuffer_T;

namespace PFF {

	class vk_device;

	class PFF_API dynamic_mesh : public basic_mesh {

	public:

		dynamic_mesh(const dynamic_mesh::builder& builder) : basic_mesh(builder) {};
		~dynamic_mesh() {};

		DELETE_COPY(dynamic_mesh);

	private:

		// TODO: override basic_mesh implementation
		// void create_vetex_buffers(const std::vector<vertex>& vertices);
		// void create_index_buffers(const std::vector<u32>& indices);

	};

}
