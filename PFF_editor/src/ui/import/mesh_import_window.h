#pragma once

#include "ui/editor_window.h"

namespace PFF {

	class mesh_import_window : public editor_window {
	public:

		mesh_import_window(const std::filesystem::path source_path, const std::filesystem::path destination_path);

		void window() override;

		std::filesystem::path 		source_path;
		std::filesystem::path 		destination_path;
		
	private:
		bool						asset_alredy_exists = false;;
		std::vector<std::string> 	assets_that_already_exist;
	};

}
