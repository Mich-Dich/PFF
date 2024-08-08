
#include "util/pch_editor.h"

#include "editor_window.h"

namespace PFF {

	void editor_window::make_window_name(const char* base_name) {

		std::stringstream ss;
		ss << base_name << "##" << std::hex << std::setfill('0') << std::setw(16) << reinterpret_cast<std::uintptr_t>(this);
		window_name = ss.str();
	}

}
