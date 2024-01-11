#pragma once

#include "engine/layer/layer.h"

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

namespace PFF {

	class renderer;

	class imgui_layer : public layer {
	public:
		imgui_layer(std::shared_ptr<renderer> renderer);
		~imgui_layer();

		void on_attach() override;
		void on_detach() override;
		void on_update() override;
		void on_event(event& event) override;
		void on_imgui_render() override;
		void set_fps_values(f32 target_fps, f32 current_fps, f32 possible_fps, f32 work_time) { m_target_fps = target_fps; m_current_fps = current_fps; m_possible_fps = possible_fps; m_work_time = work_time; }

		void begin_frame();
		void end_frame(VkCommandBuffer commandbuffer);
	
	private:

		f32 m_target_fps, m_current_fps, m_possible_fps, m_work_time;
		std::shared_ptr<renderer> m_renderer;
	};

}
