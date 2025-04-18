#pragma once

#include "ui/editor_window.h"

namespace PFF {

	class texture_import_window : public editor_window {
	public:


    texture_import_window(const std::vector<std::filesystem::path>&& source_paths, const std::filesystem::path destination_path);

		void window() override;

		std::vector<std::filesystem::path>		source_paths{};
		std::filesystem::path 					destination_path;
		
	private:
		bool									m_asset_alredy_exists = false;
		std::vector<std::string> 				m_assets_that_already_exist{};
		std::string 							m_source_string{};
		bool 									m_metadata_available = false;
		size_t 									m_number_of_sources = 0;
	};

}
