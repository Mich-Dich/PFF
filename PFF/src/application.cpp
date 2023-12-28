
#include "application.h"
#include "util/util.h"
#include "engine/platform/window.h"

namespace PFF {

	application::application() {

		CORE_LOG(Debug, "application Constructor");
		CORE_ASSERT(!s_instance, "", "Application already exists");
		s_instance = this;

		// TODO: load WindowAttributes from config file and pass into constructor
		m_window = std::unique_ptr<window>(new window());
	}

	application::~application() {}

	void application::run() {}

	void application::on_event() {}

	bool application::initalize() {
		return true;
	}

	bool application::update(f64 delta_time) {
		return true;
	}

	bool application::render(f64 delta_time) {
		return true;
	}

}

