#pragma once

#include "engine/layer/layer.h"

namespace PFF {

	class imgui_layer : public layer {
	public:
		imgui_layer();
		~imgui_layer();

		void on_attach() override;
		void on_detach() override;
		void on_update() override;
		void on_event(event& event) override;
		void on_imgui_render() override;

		void begin_frame();
		void end_frame();
	
	private:

	};

}
