
#include "util/pch_editor.h"

#include <imgui.h>

#include "util/ui/pannel_collection.h"

#include "texture_editor.h"


namespace PFF::toolkit {

	texture_editor::texture_editor() {
	
		make_window_name("Image Editor");

		//checkerboard image
		u32 grey = glm::packUnorm4x8(glm::vec4(0.4f, 0.44f, 0.4f, 1));
		u32 color = glm::packUnorm4x8(glm::vec4(0.2f, 0.2f, 0.2f, 1));
		const int EDGE_LENGTH = 212;
		std::array<u32, EDGE_LENGTH* EDGE_LENGTH > pixels; //for checkerboard texture
		for (int x = 0; x < EDGE_LENGTH; x++)
			for (int y = 0; y < EDGE_LENGTH; y++)
				pixels[y * EDGE_LENGTH + x] = ((x % EDGE_LENGTH) ^ (y % EDGE_LENGTH)) ? grey : color;
		m_image = create_ref<image>(pixels.data(), EDGE_LENGTH, EDGE_LENGTH, image_format::RGBA);
	}
	
	texture_editor::~texture_editor() {
	
		m_image.reset();
		LOG_SHUTDOWN();
	}

	void texture_editor::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin(window_name.c_str(), &show_window, window_flags);


		f32 width = std::max(ImGui::GetWindowSize().x * 0.4f, 350.0f);
		UI::custom_frame(width, 
			
			
			[&] {
			

				ImVec2 image_size = { width, width };
				ImGui::Image(m_image->get_descriptor_set(), image_size);
			},

			[] {});

		ImGui::End();
	}

}
