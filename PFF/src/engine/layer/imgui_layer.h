#pragma once

#include "engine/layer/layer.h"
#include "engine/render/renderer.h"

//class VkCommandBuffer;

namespace PFF {

	class imgui_layer : public layer {
	public:
		imgui_layer(std::shared_ptr<vk_device> device);
		~imgui_layer();

		void on_attach() override;
		void on_detach() override;
		void on_update() override;
		void on_imgui_render() override;

		void begin_frame();
		void end_frame(VkCommandBuffer command_buffer);

	private:

		std::shared_ptr<vk_device> m_device;
	};

}
