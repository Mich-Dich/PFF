#pragma once

//#include "vulkan/vk_renderer.h"
// #include "opengl/gl_renderer.h"
// #include "d3d12/dx_renderer.h"
// #include "matal/me_renderer.h"

namespace PFF {

	enum class render_api {
		none	= 0,
		Vulkan	= 1,
		OpenGl	= 2,
		D3D12	= 3,
		Metal	= 4,
	};

	class renderer {
	public:
		
		virtual ~renderer() {};

		FORCEINLINE static render_api get_api() { return s_render_api; }
		FORCEINLINE static void set_api(render_api api) { s_render_api = api; }

		system_state get_state() const { return m_state; }
		void set_state(system_state state) { m_state = state; }

		// --------------- ImGui ----------------
		virtual void imgui_init() = 0;
		virtual void imgui_shutdown() = 0;

		// --------------- renderer ----------------
		virtual void draw_frame(f32 delta_time) = 0;
		virtual void refresh(f32 delta_time) = 0;
		virtual void set_size(u32 width, u32 height) = 0;
		virtual f32 get_aspect_ratio() = 0;
		virtual void wait_idle() = 0;
		virtual void immediate_submit(std::function<void()>&& function) = 0;

		static render_api s_render_api;
		system_state m_state = system_state::inactive;


		bool m_imgui_initalized = false;
	private:

	};
}
