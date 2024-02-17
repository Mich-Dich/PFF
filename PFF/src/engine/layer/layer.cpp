
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

	void layer::on_update(const f32 delta_time) {

		PFF_PROFILE_FUNCTION();

	}

	void layer::on_event(event& event) {

		PFF_PROFILE_FUNCTION();

	}

	void layer::on_imgui_render() {

		PFF_PROFILE_FUNCTION();

	}


}