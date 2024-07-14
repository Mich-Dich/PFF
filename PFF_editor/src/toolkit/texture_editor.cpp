
#include "util/pch_editor.h"

#include <imgui.h>

#include "texture_editor.h"


namespace PFF::toolkit {

	texture_editor::texture_editor() {

		////checkerboard image
		//u32 grey = glm::packUnorm4x8(glm::vec4(0.4f, 0.44f, 0.4f, 1));
		//u32 color = glm::packUnorm4x8(glm::vec4(0.2f, 0.2f, 0.2f, 1));
		//const int EDGE_LENGTH = 2;
		//std::array<u32, EDGE_LENGTH* EDGE_LENGTH > pixels; //for checkerboard texture
		//for (int x = 0; x < EDGE_LENGTH; x++)
		//	for (int y = 0; y < EDGE_LENGTH; y++)
		//		pixels[y * EDGE_LENGTH + x] = ((x % EDGE_LENGTH) ^ (y % EDGE_LENGTH)) ? grey : color;
		//m_image = image(pixels.data(), VkExtent3D{ EDGE_LENGTH, EDGE_LENGTH, 1 });
	}
	
	texture_editor::~texture_editor() {}

	void texture_editor::window() {

		LOG(Debug, "bool value: " << util::bool_to_str(m_show_window));
		if (!m_show_window)
			return;


		LOG(Trace, "Texture Editor window");

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin("image_editor", &m_show_window, window_flags);

		//ImVec2 image_size = {60, 60};
		//ImGui::Image(m_image.get_descriptor_set_nearest(), image_size);

		ImGui::End();
	}

}
