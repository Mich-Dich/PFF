#pragma once

#include "ui/editor_window.h"

namespace PFF {

	class texture_import_window : public editor_window {
	public:

    texture_import_window(const std::filesystem::path source_path, const std::filesystem::path destination_path);

		void window() override;

		std::filesystem::path 		source_path;
		std::filesystem::path 		destination_path;
		
	private:
		bool						m_asset_alredy_exists = false;
		std::vector<std::string> 	m_assets_that_already_exist{};
		std::string 				m_source_string{};
	};

}
