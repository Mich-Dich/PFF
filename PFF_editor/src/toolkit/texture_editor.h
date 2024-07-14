#pragma once

#include "util/pch_editor.h"

namespace PFF::toolkit {

	class editor_window {
	public:

		virtual ~editor_window() = 0;
		virtual void window() = 0;

	protected:
		bool m_show_window = false;

	private:

	};

	class texture_editor : editor_window {
	public:

		texture_editor();
		~texture_editor();
		void window() override;

	private:
		image m_image;
	};

}
