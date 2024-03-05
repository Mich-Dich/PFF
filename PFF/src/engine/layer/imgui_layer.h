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
		PFF_API_EDITOR ImFont* get_font(const std::string& name);

		void on_attach() override;
		void on_detach() override;
		void on_update(const f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void show_performance();

		void begin_frame();
		void end_frame(VkCommandBuffer commandbuffer);
		void capture_current_image(VkImageView frame_buffer);

	private:

		void set_next_window_pos(int16 location);
		void imgui_layer::progressbar_with_text(const char* lable, const char* progress_bar_text, f32 percent, f32 lable_size = 50.f, f32 progressbar_size_x = 50.f, f32 progressbar_size_y = 1.f);

		ImGuiContext* m_context;
		f32 m_work_time, m_sleep_time;
		u32 m_target_fps, m_current_fps;
		bool m_limit_fps;
		std::shared_ptr<renderer> m_renderer;
		f32 m_font_size = 15.f;
		f32 m_big_font_size = 18.f;
		std::unordered_map<std::string, ImFont*> m_fonts{};

		bool m_show_performance_window = true;
	};

}
