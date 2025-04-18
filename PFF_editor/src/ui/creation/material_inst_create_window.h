#pragma once

#include "ui/editor_window.h"

namespace PFF {

	class material_inst_create_window : public editor_window {
	public:

    material_inst_create_window(const std::filesystem::path destination_path);

		void window() override;

		std::filesystem::path 				source_path;
		std::filesystem::path 				destination_path;
		
	private:
		bool								m_asset_alredy_exists = false;
		std::vector<std::string> 			m_assets_that_already_exist{};
		std::string 						m_source_string{};
		bool 								m_metadata_available = false;
		char								m_possible_asset_name[256]{};
	};

}
