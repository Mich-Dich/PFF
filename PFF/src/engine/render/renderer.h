#pragma once

#include "render_util.h"

#include "engine/layer/layer_stack.h"
#include "engine/game_objects/camera.h"

#include "engine/render/image.h"

#if defined PFF_RENDER_API_VULKAN
#include "engine/render/vulkan/vk_descriptor.h"
#include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF::render {

	// ------------------------------------------------------------------------------------------------------------------------
	// DATA STRUCTURES
	// ------------------------------------------------------------------------------------------------------------------------

	enum class render_api {

		none = 0,
		Vulkan = 1,
		OpenGl = 2,
		D3D12 = 3,
		Metal = 4,
	};

	struct GPU_mesh_buffers {

#if defined PFF_RENDER_API_VULKAN
		vk_buffer			index_buffer{};
		vk_buffer			vertex_buffer{};
		VkDeviceAddress		vertex_buffer_address{};
#endif
	};

	enum class material_pass : u8 {

		MainColor,
		Transparent,
		Other
	};

	struct material_pipeline {

		VkPipeline			pipeline;
		VkPipelineLayout	layout;
	};


	struct compute_push_constants {

		glm::vec4				data1;
		glm::vec4				data2;
		glm::vec4				data3;
		glm::vec4				data4;
	};

	struct compute_effect {

		const char* name{};
#if defined PFF_RENDER_API_VULKAN
		VkPipeline				pipeline{};
		VkPipelineLayout		layout{};
#endif
		compute_push_constants	data{};
	};

	struct material_instance {

		material_pipeline*	pipeline;
#if defined PFF_RENDER_API_VULKAN
		VkDescriptorSet		material_set;
#endif
		material_pass		pass_type;
	};

	struct GPU_scene_data {

		glm::mat4			view;
		glm::mat4			proj;
		glm::mat4			view_proj;
		glm::vec4			ambient_color;
		glm::vec4			sunlight_direction;		// w for sun power
		glm::vec4			sunlight_color;
	};


	// ------------------------------------------------------------------------------------------------------------------------
	// RENDERER FUNCTIONALLITY
	// ------------------------------------------------------------------------------------------------------------------------


	class renderer {
	public:
		
		virtual ~renderer() {};

		FORCEINLINE static render_api get_api() { return s_render_api; }
		FORCEINLINE static void set_api(render_api api) { s_render_api = api; }
		FORCEINLINE void set_active_camera(ref<camera> camera) { m_active_camera = camera; }
		FORCEINLINE compute_effect& get_current_background_effect() { return m_background_effects[m_current_background_effect]; }

		FORCEINLINE int get_number_of_background_effects() { return static_cast<int>(m_background_effects.size()); }
		PFF_GETTER(int&, current_background_effect_index, m_current_background_effect);


		virtual void* get_rendered_image() = 0;

		system_state get_state() const { return m_state; }
		void set_state(system_state state) { m_state = state; }

		// --------------- general ----------------
		virtual void draw_frame(f32 delta_time) = 0;
		virtual void refresh(f32 delta_time) = 0;
		virtual void set_size(u32 width, u32 height) = 0;
		virtual f32 get_aspect_ratio() = 0;
		virtual void wait_idle() = 0;

		// --------------- ImGui ----------------
		virtual void imgui_init() = 0;
		virtual void imgui_create_fonts() = 0;
		virtual void imgui_destroy_fonts() = 0;
		virtual void imgui_shutdown() = 0;

		// --------------- util ----------------
		virtual void immediate_submit(std::function<void()>&& function) = 0;
		virtual void enable_vsync(bool enable) = 0;

		static render_api							s_render_api;
		system_state								m_state = system_state::inactive;

		bool										m_imgui_initalized = false;
		bool										m_render_swapchain = false;	// FALSE: will display rendered image in a imgui window    TRUE: will display rendered image directly into GLFW_window
		ref<camera>									m_active_camera{};

	protected:
		std::vector<compute_effect> 				m_background_effects{};
		int											m_current_background_effect = 2;

	private:

	};

}
