#pragma once

#include "editor_window.h"

namespace PFF {

	class mesh_import_window : public editor_window {
	public:

		mesh_import_window(const std::filesystem::path source_path, const std::filesystem::path destination_path)
			: source_path(source_path), destination_path(destination_path) { }

		void window() override;

		std::filesystem::path source_path;
		std::filesystem::path destination_path;
	};

}
