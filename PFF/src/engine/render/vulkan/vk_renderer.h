#pragma once

#include "engine/render/renderer.h"
#include "engine/render/vulkan/vk_types.h"


#include "engine/platform/pff_window.h"
#include "engine/geometry/mesh.h"


#include "vk_types.h"

class PFF::layer_stack;

namespace PFF::render::vulkan {

	// !! CAUTION !! - Doing callbacks like this is inneficient at scale, because we are storing whole std::functions for every object to be deleted. This is suboptimal. 
	// For deleting thousands of objects, a better implementation would be to store arrays of vulkan handles of various types such as VkImage, VkBuffer, and so on. And then delete those from a loop.
	struct deletion_queue {

		std::deque<std::function<void()>> deletors;

		void push_func(std::function<void()>&& function) { deletors.push_back(function); }

		void flush() {

			for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
				(*it)();

			deletors.clear();
		}
	};

	struct compute_push_constants {
	
		glm::vec4				data1;
		glm::vec4				data2;
		glm::vec4				data3;
		glm::vec4				data4;
	};

	struct compute_effect {

		const char*				name{};
		VkPipeline				pipeline{};
		VkPipelineLayout		layout{};
		compute_push_constants	data{};
	};

	// framedata	// TODO: extract into own file
	struct FrameData {

		deletion_queue			deletion_queue{};
		VkSemaphore				swapchain_semaphore{}, render_semaphore{};
		VkFence					render_fence{};

		VkCommandPool			command_pool{};
		VkCommandBuffer			main_command_buffer{};
	};

	constexpr u32 FRAME_COUNT = 2;

	class vk_renderer : public PFF::render::renderer {
	public:

		vk_renderer(ref<pff_window> window, ref<PFF::layer_stack> layer_stack);
		~vk_renderer();

		FORCEINLINE f32 get_aspect_ratio() { return 1.f; };			// UNFINISHED

		// --------------- general ----------------
		void draw_frame(f32 delta_time) override;
		void refresh(f32 delta_time) override;
		void set_size(u32 width, u32 height) override;
		void wait_idle() override;

		// --------------- ImGui ----------------
		void imgui_init() override;
		void imgui_create_fonts() override;
		void imgui_shutdown() override;

		// --------------- util ----------------
		void immediate_submit(std::function<void()>&& function) override;
		void enable_vsync(bool enable) override {}							// TODO: implement (recreate swapchain with new VK_PRESENT_MODE_XXX )
		void* get_rendered_image() override { return (void*)m_imugi_image_dset; }

	private:

		FORCEINLINE FrameData& get_current_frame() { return m_frames[m_frame_number % FRAME_COUNT]; };
		
		void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function); // Improvement => run this on a different queue than the graphics_queue, so it can overlap the execution with the main render loop.

		void init_default_data();
		void init_swapchain();
		void init_commands();
		void init_sync_structures();
		void init_descriptors();
		void destroy_swapchain();

		void init_pipelines();
		void init_pipelines_background();
		void init_pipeline_triangle();
		void init_pipeline_mesh();

		void draw_internal(VkCommandBuffer cmd);
		void draw_geometry(VkCommandBuffer cmd);
		void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
		void create_swapchain(u32 width, u32 height);

		allocated_buffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void destroy_buffer(const allocated_buffer& buffer);

		// TIP: Note that this pattern is not very efficient, as CPU is waiting for the GPU command to fully execute before continuing with our CPU side logic
		//		This is should be put on a background thread, whose sole job is to execute uploads like this one, and deleting/reusing the staging buffers.
		vk_GPU_mesh_buffers upload_mesh(std::vector<u32> indices, std::vector<PFF::geometry::vertex> vertices);

		bool m_is_initialized = false;
		int m_frame_number = 0;
		ref<pff_window> m_window{};
		ref<PFF::layer_stack> m_layer_stack{};

		// ---------------------------- instance ---------------------------- 
		VkInstance					m_instance{};			// Vulkan library handle
		VkDebugUtilsMessengerEXT	m_debug_messenger{};	// Vulkan debug output handle
		VkPhysicalDevice			m_chosenGPU{};			// GPU chosen as the default device
		VkDevice					m_device{};				// Vulkan device for commands
		VkSurfaceKHR				m_surface{};			// Vulkan window surface
		
		// ---------------------------- queues ---------------------------- 
		FrameData					m_frames[FRAME_COUNT];
		VkQueue						m_graphics_queue{};
		u32							m_graphics_queue_family{};
		
		// ---------------------------- swapchain ---------------------------- 
		VkSwapchainKHR				m_swapchain{};
		VkFormat					m_swapchain_image_format{};
		std::vector<VkImage>		m_swapchain_images{};
		std::vector<VkImageView>	m_swapchain_image_views{};
		VkExtent2D					m_swapchain_extent{};

		deletion_queue				m_deletion_queue{};
		VmaAllocator				m_allocator{};

		VkExtent2D					m_draw_extent{};
		vk_image					m_draw_image{};
		vk_image					m_depth_image{};

		// display rendered image in imgui
		VkSampler			m_texture_sampler{};
		VkDescriptorSet		m_imugi_image_dset{};

		// ---------------------------- descriptors ---------------------------- 
		descriptor_allocator		global_descriptor_allocator{};
		VkDescriptorSet				m_draw_image_descriptors{};
		VkDescriptorSetLayout		m_draw_image_descriptor_layout{};
		
		// ---------------------------- pipelines ---------------------------- 
		VkPipeline					m_gradient_pipeline{};
		VkPipelineLayout			m_gradient_pipeline_layout{};

		// ---------------------------- immediate-submit ---------------------------- 
		VkFence						m_immFence{};
		VkCommandBuffer				m_immCommandBuffer{};
		VkCommandPool				m_immCommandPool{};

		VkDescriptorPool			m_imgui_desc_pool{};

		std::vector<compute_effect> m_background_effects{};
		int							m_current_background_effect = 0;

		// ---------------------------- triangle pipeline ---------------------------- 
		VkPipelineLayout			m_triangle_pipeline_layout{};
		VkPipeline					m_triangle_pipeline{};

		// ---------------------------- mesh pipeline ---------------------------- 
		VkPipelineLayout			m_mesh_pipeline_layout{};
		VkPipeline					m_mesh_pipeline{};

		vk_GPU_mesh_buffers						T_rectangle;
		std::vector<ref<PFF::geometry::mesh>>		T_test_meshes;


	};
}
