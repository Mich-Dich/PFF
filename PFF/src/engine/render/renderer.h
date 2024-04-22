#pragma once

#include "render_util.h"

#include "engine/geometry/geometry.h"
#include "engine/io_handler/file_loader.h"
#include "engine/platform/pff_window.h"
#include "engine/layer/layer_stack.h"

namespace PFF::render {

	enum class render_api {

		none	= 0,
		Vulkan	= 1,
		OpenGl	= 2,
		D3D12	= 3,
		Metal	= 4,
	};

	class GPU_mesh_buffers {
	public:

		virtual ~GPU_mesh_buffers() {}
	};


	class renderer {
	public:
		
		virtual ~renderer() {};

		FORCEINLINE static render_api get_api() { return s_render_api; }
		FORCEINLINE static void set_api(render_api api) { s_render_api = api; }

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
		virtual void imgui_shutdown() = 0;

		// --------------- util ----------------
		virtual void immediate_submit(std::function<void()>&& function) = 0;
		virtual void enable_vsync(bool enable) = 0;

		static render_api s_render_api;
		system_state m_state = system_state::inactive;

		bool m_imgui_initalized = false;
		bool m_render_swapchain = false;	// false => will display rendered image in a imgui window TRUE: will display rendered image directly into GLFW_window

	private:

	};

}
