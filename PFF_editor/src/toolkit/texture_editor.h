#pragma once

#include "util/pch_editor.h"

namespace PFF::toolkit {

	class texture_editor : public PFF::editor_window {
	public:

		texture_editor();
		~texture_editor();
		void window() override;

	private:
		ref<image> m_image;

	};

}
