#pragma once

#include "render_util.h"

#include "engine/render/vulkan/vk_types.h"
#include "engine/layer/layer_stack.h"
#include "engine/game_objects/camera.h"

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

	enum class material_pass : u8 {

		MainColor,
		Transparent,
		Other
	};

	struct GPU_mesh_buffers {

#if defined PFF_RENDER_API_VULKAN
		vk_buffer			index_buffer{};
		vk_buffer			vertex_buffer{};
		VkDeviceAddress		vertex_buffer_address{};
#endif
	};

	struct material_pipeline {

		VkPipeline			pipeline;
		VkPipelineLayout	layout;
	};

	struct material_instance {

		material_pipeline*	pipeline;
		VkDescriptorSet		materialSet;
		material_pass		passType;
	};

	struct GPU_scene_data {

		glm::mat4			view;
		glm::mat4			proj;
		glm::mat4			viewproj;
		glm::vec4			ambientColor;
		glm::vec4			sunlightDirection; // w for sun power
		glm::vec4			sunlightColor;
	};

	// ------------------------------------------------------------------------------------------------------------------------
	// ECS - COMPONENT
	// ------------------------------------------------------------------------------------------------------------------------

	struct mesh_component {

		PFF_DEFAULT_CONSTRUCTORS(mesh_component);

		u32					indexCount;
		u32					firstIndex;
		glm::mat4			transform;

#if defined PFF_RENDER_API_VULKAN
		VkBuffer			indexBuffer;
		VkDeviceAddress		vertexBufferAddress;
		material_instance*	material;
#endif
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

		static render_api s_render_api;
		system_state m_state = system_state::inactive;

		bool m_imgui_initalized = false;
		bool m_render_swapchain = false;	// false => will display rendered image in a imgui window TRUE: will display rendered image directly into GLFW_window
		ref<camera> m_active_camera{};
	private:

	};

}
