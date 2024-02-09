#pragma once

#include "basic_mesh.h"

struct VkBuffer_T;
struct VkDeviceMemory_T;
struct VkCommandBuffer_T;

namespace PFF {

	class vk_device;

	class PFF_API static_mesh : public basic_mesh {

	public:

		static_mesh(const basic_mesh::builder& builder) : basic_mesh(builder) {};
		~static_mesh() {}

		DELETE_COPY(static_mesh);

	private:

		void create_vetex_buffers(const std::vector<vertex>& vertices);
		void create_index_buffers(const std::vector<u32>& indices);

	};

}
