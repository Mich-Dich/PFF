#pragma once

#include "editor_window.h"
#include "content_browser.h"

namespace PFF {

	class world_viewport_window : public editor_window {
	public:

		world_viewport_window();
		~world_viewport_window();


		void window() override;

	private:
		
		void serialize(serializer::option option);

		void window_general_debugger();								// TODO: convert into editor window
		void window_world_settings();								// TODO: convert into editor window
		void window_main_viewport();
		void window_outliner();
		void window_details();
		void window_renderer_backgrond_effect();

		bool m_show_renderer_backgrond_effect = false;
		bool m_show_world_settings = false;
		bool m_show_general_debugger = true;
		bool m_show_details = false;
		bool m_show_outliner = true;
		content_browser m_content_browser;

	};

}