#pragma once

#include "engine/layer/layer.h"

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

struct VkImageView_T;
typedef VkImageView_T* VkImageView;

typedef int ImGuiWindowFlags;


namespace PFF {

	class renderer;

	class imgui_layer : public layer {
	public:
		imgui_layer(std::shared_ptr<renderer> renderer);
		~imgui_layer();

		void on_attach() override;
		void on_detach() override;
		void on_update(f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void set_fps_values(u32 target_fps, u32 current_fps, f32 work_time, f32 sleep_time)
			{ m_target_fps = target_fps; m_current_fps = current_fps; m_work_time = work_time; m_sleep_time = sleep_time;  }

		void begin_frame();
		void end_frame(VkCommandBuffer commandbuffer);
		void capture_current_image(VkImageView frame_buffer);
	
	private:

		void set_next_window_pos(int16 location);
		void progressbar_with_text(f32 percent, const char* text, f32 min_size_x = 1.0f, f32 min_size_y = 1.0f);

		f32 m_work_time, m_sleep_time;
		u32 m_target_fps, m_current_fps;
		std::shared_ptr<renderer> m_renderer;
		f32 m_font_size = 14.5f;
	};

}
