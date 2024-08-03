#pragma once

#include "render_util.h"

#include "engine/layer/layer_stack.h"
#include "engine/game_objects/camera.h"

#include "engine/render/image.h"
#include "engine/world/components.h"
#include "engine/render/render_public_data.h"


#if defined PFF_RENDER_API_VULKAN
	#include "engine/render/vulkan/vk_descriptor.h"
	#include "engine/render/vulkan/vk_types.h"
#endif

namespace PFF::render {

	// ------------------------------------------------------------------------------------------------------------------------
	// DATA STRUCTURES
	// ------------------------------------------------------------------------------------------------------------------------

	struct renderer_metrik {
		u32 mesh_draw = 0;
		u32 draw_calls = 0;
		u64 triangles = 0;
		f32 sleep_time = 0.f,	work_time = 0.f;

		void reset() {
			draw_calls = mesh_draw = 0;
			triangles = 0;
			sleep_time = work_time = 0.f;
		}
	};

	// ------------------------------------------------------------------------------------------------------------------------
	// RENDERER FUNCTIONALLITY
	// ------------------------------------------------------------------------------------------------------------------------

	class renderer {
	public:
		
		virtual ~renderer() {};

		PFF_GETTER(int&, current_background_effect_index,				m_current_background_effect);
		PFF_DEFAULT_GETTERS(renderer_metrik,							renderer_metrik)

		FORCEINLINE static render_api get_api()							{ return s_render_api; }
		FORCEINLINE static void set_api(render_api api)					{ s_render_api = api; }
		FORCEINLINE void set_active_camera(ref<camera> camera)			{ m_active_camera = camera; }
		FORCEINLINE compute_effect& get_current_background_effect()		{ return m_background_effects[m_current_background_effect]; }
		FORCEINLINE int get_number_of_background_effects()				{ return static_cast<int>(m_background_effects.size()); }

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

		renderer_metrik								m_renderer_metrik{};

	protected:
		std::vector<compute_effect> 				m_background_effects{};
		int											m_current_background_effect = 2;

	private:

	};

}
