#pragma once

#include "ui/editor_window.h"

namespace PFF {

	class mesh_import_window : public editor_window {
	public:

		mesh_import_window(const std::vector<std::filesystem::path>&& source_paths, const std::filesystem::path destination_path);
		~mesh_import_window();
		
		void window() override;

		std::vector<std::filesystem::path>		source_paths;
		std::filesystem::path 					destination_path;
		
	private:
		
		bool									m_asset_alredy_exists = false;
		std::vector<std::string> 				m_assets_that_already_exist{};
		size_t 									m_number_of_sources = 0;

		struct importing_thread {
			f32 			progress = .0f;          	// progress [0,1] per asset
			std::thread 	thread{};
		};
		std::atomic<bool> 						m_importing{false};
		std::vector<importing_thread>			m_import_threads{};
		std::unique_ptr<std::thread> 			m_import_manager;
		void start_import();
		void check_if_assets_exist();

	};

}
