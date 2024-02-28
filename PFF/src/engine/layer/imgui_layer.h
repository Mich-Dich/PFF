#pragma once

#include "engine/layer/layer.h"

#include <vulkan/vulkan.h>
#include <imgui.h>
#include "imgui_impl_vulkan.h"

namespace PFF {

	class renderer;
	static void Setup_vulkan_window(ImGui_ImplVulkanH_Window* wd, int width, int height);

	class imgui_layer : public layer {
	public:
		imgui_layer(std::shared_ptr<renderer> renderer);
		~imgui_layer();

		FORCEINLINE ImGuiContext* get_context() const { return m_context; }

		void on_attach() override;
		void on_detach() override;
		void on_update(const f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void show_performance();
		void set_fps_values(const bool limit_fps, const  u32 target_fps, const u32 current_fps, const f32 work_time, const f32 sleep_time) { m_limit_fps = limit_fps, m_target_fps = target_fps; m_current_fps = current_fps; m_work_time = work_time; m_sleep_time = sleep_time; }

		PFF_API_EDITOR ImFont* get_font(const std::string& name);
		void begin_frame();
		void end_frame(VkCommandBuffer commandbuffer);
		void capture_current_image(VkImageView frame_buffer);

	private:

		void set_next_window_pos(int16 location);
		void progressbar_with_text(f32 percent, const char* text, f32 min_size_x = 1.0f, f32 min_size_y = 1.0f);

		ImGuiContext* m_context;
		f32 m_work_time, m_sleep_time;
		u32 m_target_fps, m_current_fps;
		bool m_limit_fps;
		std::shared_ptr<renderer> m_renderer;
		f32 m_font_size = 15.f;
		f32 m_big_font_size = 17.f;
		std::unordered_map<std::string, ImFont*> m_fonts{};

	};

}
