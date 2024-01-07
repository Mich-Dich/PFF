#pragma once

#include "engine/layer/layer.h"

namespace PFF {

	class imgui_layer : public layer {
	public:
		imgui_layer();
		~imgui_layer();

		void on_attach();
		void on_detach();
		void on_update();
		void on_event(event& event);

	private:

	};

}
