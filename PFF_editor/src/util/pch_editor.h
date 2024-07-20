#pragma once

// PCH of engine
#include "util/pffpch.h"

// main files
#include "application.h"

// UI related files
//#include "ui/panels/pannel_collection.h"
//#include "engine/color_theme.h"


namespace PFF {

	class editor_window {
	public:

		editor_window() {};
		virtual ~editor_window() {};
		virtual void window() {};

	protected:
		void make_window_name(const char* base_name);
		std::string window_name{};
		bool show_window = true;

	private:

	};

}
