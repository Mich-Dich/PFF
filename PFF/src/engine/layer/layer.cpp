
#include "util/pffpch.h"

#include "layer.h"

namespace PFF {



	layer::layer(const std::string& name) 
		: m_debugname(name), m_enabled(false) {}

	layer::~layer() {

		CORE_LOG(Info, "Shutdown");
	}

	void layer::on_attach() {
	}

	void layer::on_detach() {
	}

	void layer::on_update(f32 delta_time) {
	}

	void layer::on_event(event& event) {
	}

	void layer::on_imgui_render() {
	}


}