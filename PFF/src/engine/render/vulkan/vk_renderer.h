#pragma once

#include "engine/render/renderer.h"
#include "engine/render/vulkan/vk_types.h"
#include "engine/platform/pff_window.h"
#include "engine/geometry/mesh.h"

#include "engine/render/material.h"

#include <typeindex>
//#include "vk_types.h"
//#include <vulkan/vulkan.h>

class PFF::layer_stack;

namespace PFF::render::vulkan {

	class deletion_queue;
	constexpr u32 FRAME_COUNT = 2;


	class deletion_queue {
	public:

		PFF_DEFAULT_CONSTRUCTORS(deletion_queue);

		void setup(VkDevice device, VmaAllocator allocator);
		void cleanup();

		template<typename T>
		void push_pointer(T* pointer) { m_pointers.push_back(std::pair<std::type_index, void*>{std::type_index(typeid(T*)), pointer}); }

		void push_func(std::function<void()>&& function);
		void flush();

		// --------------- data ---------------
	private:
		std::deque<std::function<void()>>				m_deletors{};
		std::vector<std::pair<std::type_index, void*>>	m_pointers{};
		VkDevice										m_dq_device{};
		VmaAllocator									m_dq_allocator{};
	};


	class vk_renderer : public PFF::render::renderer {
	public:

		vk_renderer(ref<pff_window> window, ref<PFF::layer_stack> layer_stack);
		~vk_renderer();

		FORCEINLINE f32 get_aspect_ratio()			{ return 1.f; };			// TODO: finish
		PFF_DEFAULT_GETTERS(VkPhysicalDevice,		chosenGPU);
		PFF_DEFAULT_GETTER(VkSampler,				texture_sampler);
		PFF_DEFAULT_GETTER(VkSampler,				default_sampler_linear);
		PFF_DEFAULT_GETTER(VkSampler,				default_sampler_nearest);
		PFF_DEFAULT_GETTER(VkDevice,				device);
		PFF_DEFAULT_GETTER(VmaAllocator,			allocator);
		
		PFF_DEFAULT_SETTER(glm::u32vec2,			imugi_viewport_size);
				
		
		PFF_DEFAULT_GETTER(VkDescriptorSetLayout,	gpu_scene_data_descriptor_layout);


		PFF_DEFAULT_GETTER_POINTER(image,			draw_image)
		PFF_DEFAULT_GETTER_POINTER(image,			depth_image)


		// !!!!!!!!!!!!!!!! DEV !!!!!!!!!!!!!!!!!!!!!!
		void setup();

		// --------------- general ----------------
		void draw_frame(f32 delta_time) override;
		void refresh(f32 delta_time) override;
		void set_size(u32 width, u32 height) override;
		void wait_idle() override;

		// --------------- ImGui ----------------
		void imgui_init() override;
		void imgui_create_fonts() override;
		void imgui_destroy_fonts() override;
		void imgui_shutdown() override;

		// --------------- util ----------------
		void immediate_submit(std::function<void()>&& function) override;
		void immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function); // Improvement => run this on a different queue than the graphics_queue, so it can overlap the execution with the main render loop.

		void submit_resource_free(std::function<void()>&& func);

		void enable_vsync(bool enable) override {}							// TODO: implement (recreate swapchain with new VK_PRESENT_MODE_XXX )
		void* get_rendered_image() override { return (void*)m_imugi_image_dset; }

		vk_buffer create_buffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
		void destroy_buffer(const vk_buffer& buffer);


	private:

		friend class deletion_queue;

		struct FrameData {

			VkSemaphore						swapchain_semaphore{}, render_semaphore{};
			VkFence							render_fence{};

			VkCommandPool					command_pool{};
			VkCommandBuffer					main_command_buffer{};

			deletion_queue					deletion_queue{};
			descriptor_allocator_growable	frame_descriptors;

		};

		FORCEINLINE FrameData& get_current_frame() { return m_frames[m_frame_number % FRAME_COUNT]; };



		void init_default_data();
		void init_commands();
		void init_sync_structures();
		void init_descriptors();

		void init_swapchain();
		void destroy_swapchain();
		void resize_swapchain();

		void init_pipelines();
		void init_pipelines_background();
		void init_pipeline_mesh();

		void draw_internal(VkCommandBuffer cmd);
		void draw_geometry(VkCommandBuffer cmd);
		void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
		void create_swapchain(u32 width, u32 height);

		// TIP: Note that this pattern is not very efficient, as CPU is waiting for the GPU command to fully execute before continuing with our CPU side logic
		//		This is should be put on a background thread, whose sole job is to execute uploads like this one, and deleting/reusing the staging buffers.
		GPU_mesh_buffers upload_mesh(std::vector<u32> indices, std::vector<PFF::geometry::vertex> vertices);

		bool m_is_initialized = false;
		u64 m_frame_number = 0;
		ref<pff_window> m_window{};
		ref<PFF::layer_stack> m_layer_stack{};

		// ---------------------------- instance ---------------------------- 
		VkInstance									m_instance{};			// Vulkan library handle
		VkDebugUtilsMessengerEXT					m_debug_messenger{};	// Vulkan debug output handle
		VkPhysicalDevice							m_chosenGPU{};			// GPU chosen as the default device
		VkDevice									m_device{};				// Vulkan device for commands
		VkSurfaceKHR								m_surface{};			// Vulkan window surface
		
		// ---------------------------- queues ---------------------------- 
		FrameData									m_frames[FRAME_COUNT]{};
		VkQueue										m_graphics_queue{};
		u32											m_graphics_queue_family{};
		
		// ---------------------------- swapchain ---------------------------- 
		VkSwapchainKHR								m_swapchain{};
		VkFormat									m_swapchain_image_format{};
		std::vector<VkImage>						m_swapchain_images{};
		std::vector<VkImageView>					m_swapchain_image_views{};
		VkExtent2D									m_swapchain_extent{};
		bool										m_resize_nedded = false;
													
		deletion_queue								m_deletion_queue{};
		VmaAllocator								m_allocator{};
													
		VkExtent2D									m_draw_extent{};
		f32											m_render_scale = 1.f;
		image										m_draw_image;
		image										m_depth_image;

		// display rendered image in imgui
		VkSampler									m_texture_sampler{};
		VkDescriptorSet								m_imugi_image_dset{};
		glm::u32vec2								m_imugi_viewport_size{100};

		// ---------------------------- descriptors ---------------------------- 
		descriptor_allocator						global_descriptor_allocator{};
		VkDescriptorSet								m_draw_image_descriptors{};
		VkDescriptorSetLayout						m_draw_image_descriptor_layout{};
		
		// ---------------------------- pipelines ---------------------------- 
		VkPipeline									m_gradient_pipeline{};
		VkPipelineLayout							m_gradient_pipeline_layout{};

		// ---------------------------- immediate-submit ---------------------------- 
		VkFence										m_immFence{};
		VkCommandBuffer								m_immCommandBuffer{};
		VkCommandPool								m_immCommandPool{};
		VkDescriptorPool							m_imgui_desc_pool{};

		// ---------------------------- triangle pipeline ---------------------------- 
		//VkPipelineLayout							m_triangle_pipeline_layout{};
		//VkPipeline								m_triangle_pipeline{};

		// ---------------------------- mesh pipeline ---------------------------- 
		VkPipelineLayout							m_mesh_pipeline_layout{};
		VkPipeline									m_mesh_pipeline{};

		//vk_GPU_mesh_buffers		T_rectangle;
		std::vector<ref<PFF::geometry::mesh>>		T_test_meshes;

		// ---------------------------- GPU side global scene data ---------------------------- 
		GPU_scene_data								m_scene_data;
		VkDescriptorSetLayout						m_gpu_scene_data_descriptor_layout;

		// ---------------------------- default textures ---------------------------- 
		ref<image>									m_white_image;
		ref<image>									m_black_image;
		ref<image>									m_grey_image;
		ref<image>									m_error_checkerboard_image;
		VkSampler									m_default_sampler_linear;
		VkSampler									m_default_sampler_nearest;
		VkDescriptorSetLayout						m_single_image_descriptor_layout;
		material_instance							m_default_material;
		material									m_metal_rough_material;

	};
}
