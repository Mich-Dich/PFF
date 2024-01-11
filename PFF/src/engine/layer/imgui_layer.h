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

		void begin_frame();
		void end_frame(VkCommandBuffer commandbuffer);
	
	private:

		std::shared_ptr<renderer> m_renderer;
	};

}
