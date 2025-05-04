#pragma once

#include "util/io/serializer_yaml.h"

#include "engine/render/renderer.h"
#include "engine/render/vulkan/vk_types.h"
#include "engine/platform/pff_window.h"
#include "engine/geometry/mesh.h"
#include "engine/render/material.h"

#include <typeindex>

class PFF::layer_stack;

namespace PFF::render::vulkan {

	class deletion_queue;
	constexpr u32 FRAME_COUNT = 2;
	//struct vk_buffer;

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

		PFF_DELETE_COPY_CONSTRUCTOR(vk_renderer);
		//vk_renderer(const vk_renderer&) = delete;

		static vk_renderer& get() { return s_instance; }

		FORCEINLINE f32 get_aspect_ratio()										{ return 1.f; };			// TODO: finish
		PFF_DEFAULT_GETTERS(VkPhysicalDevice,									chosenGPU);
		PFF_DEFAULT_GETTER(VkSampler,											texture_sampler);
		PFF_DEFAULT_GETTER(VkSampler,											default_sampler_linear);
		PFF_DEFAULT_GETTER(VkSampler,											default_sampler_nearest);
		PFF_DEFAULT_GETTER(VkDevice,											device);
		PFF_DEFAULT_GETTER(VmaAllocator,										allocator);
		PFF_DEFAULT_GETTER_REF(material,										metal_rough_material);
		PFF_DEFAULT_GETTER(VkDescriptorSetLayout,								gpu_scene_data_descriptor_layout);
		PFF_DEFAULT_GETTER(descriptor_allocator_growable,						global_descriptor_allocator);
		PFF_DEFAULT_GETTER_REF(render::GPU_scene_data,							scene_data);
		// PFF_DEFAULT_GETTER_REF(render::compute_push_constants_dynamic_skybox,	skybox_data);
		 		
		FORCEINLINE void set_imugi_viewport_size(glm::u32vec2 imugi_viewport_size) { m_imugi_viewport_size = imugi_viewport_size; }
		
		VkBuffer get_material_constant_buffer() { return m_material_constant.buffer; }
		// PFF_DEFAULT_GETTER_POINTER(vk_buffer, 				material_constant)
		PFF_DEFAULT_GETTER_POINTER(image,					draw_image)
		PFF_DEFAULT_GETTER_POINTER(image,					depth_image)

		void setup(ref<pff_window> window, ref<PFF::layer_stack> layer_stack);
		void shutdown();
		void resource_free();

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

		// TIP: Note that this pattern is not very efficient, as CPU is waiting for the GPU command to fully execute before continuing with our CPU side logic
		//		This is should be put on a background thread, whose sole job is to execute uploads like this one, and deleting/reusing the staging buffers.
		render::GPU_mesh_buffers upload_mesh(std::vector<u32> indices, std::vector<PFF::geometry::vertex> vertices);
		void update_mesh(render::GPU_mesh_buffers& mesh, const std::vector<u32>& indices, const std::vector<PFF::geometry::vertex>& vertices);
		void update_mesh(PFF::geometry::procedural_mesh_asset& mesh, const std::vector<u32>& indices, const std::vector<PFF::geometry::vertex>& vertices);
		void cleanup_procedural_mesh(PFF::geometry::procedural_mesh_asset& mesh);
		void release_mesh(render::GPU_mesh_buffers& mesh);

#ifdef PFF_RENDERER_DEBUG_CAPABILITY
		void add_debug_line(PFF::geometry::vertex start, PFF::geometry::vertex end);
		void clear_debug_line();
#else
		void add_debug_line(PFF::geometry::vertex start, PFF::geometry::vertex end)		{ LOG(Warn, "function [add_debug_line()] only available in debug mode"); }
		void clear_debug_line()															{ LOG(Warn, "function [clear_debug_line()] only available in debug mode"); }
#endif // PFF_RENDERER_DEBUG_CAPABILITY

	private:

		vk_renderer() {}
		static vk_renderer s_instance;

		friend class deletion_queue;

		struct FrameData {

			VkSemaphore								swapchain_semaphore{}, render_semaphore{};
			VkFence									render_fence{};

			VkCommandPool							command_pool{};
			VkCommandBuffer							main_command_buffer{};

			deletion_queue							del_queue{};
			descriptor_allocator_growable			frame_descriptors;

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
		void init_grid_pipeline();

		void draw_internal(VkCommandBuffer cmd);
		void draw_geometry(VkCommandBuffer cmd);
		void draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView);
		void create_swapchain(u32 width, u32 height);

		void calc_frustum_planes(const glm::mat4& pro_view);
		bool is_bounds_in_frustum(const PFF::geometry::bounds& bounds, const glm::mat4& transform);
		void serialize(const PFF::serializer::option option);

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
		//descriptor_allocator						global_descriptor_allocator{};
		descriptor_allocator_growable				m_global_descriptor_allocator;
		VkDescriptorSet								m_global_descriptor;
		VkDescriptorSet								m_draw_image_descriptors{};
		VkDescriptorSetLayout						m_draw_image_descriptor_layout{};
		
		// ---------------------------- pipelines ---------------------------- 
		VkPipeline									m_gradient_pipeline{};
		VkPipelineLayout							m_gradient_pipeline_layout{};
		VkPipelineLayout							m_skybox_pipeline_layout{};
		VkPipeline									m_skybox_pipeline{};

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

		VkPipelineLayout							m_grid_pipeline_layout{};
		VkPipeline									m_grid_pipeline{};

		// ---------------------------- GPU side global scene data ---------------------------- 
		render::GPU_scene_data						m_scene_data;
		VkDescriptorSetLayout						m_gpu_scene_data_descriptor_layout;

		// ---------------------------- default textures ---------------------------- 
		ref<image>									m_white_image;
		ref<image>									m_black_image;
		ref<image>									m_grey_image;
		ref<image>									m_error_checkerboard_image;
		VkSampler									m_default_sampler_linear;
		VkSampler									m_default_sampler_nearest;
		VkDescriptorSetLayout						m_single_image_descriptor_layout;
		material									m_metal_rough_material;
		ref<material_instance>						m_default_material;
		vk_buffer 									m_material_constant;


		// render::compute_push_constants_dynamic_skybox 	m_skybox_data{};

		// ---------------------------- data for debug ---------------------------- 
#ifdef PFF_RENDERER_DEBUG_CAPABILITY
		PFF::geometry::mesh_asset					m_debug_lines{};

		//std::vector<PFF::geometry::mesh_asset>		m_debug_line_assets{};			// potentionaly save all debug assets like this

		material_instance							m_debug_lines_material_inst;
		material									m_debug_lines_material;
#endif // PFF_RENDERER_DEBUG_CAPABILITY

		std::array<glm::vec4, 6>					m_view_frustum{};
	};
}
