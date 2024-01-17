
#include <PFF.h>

#include "controller/editor_controller.h"
#include "ui/editor_layer.h"

namespace PFF {

	class PFF_editor : public application {

	public:

		PFF_editor() {

			LOG(Debug, "PFF_editor Constructor");
			init();
		}
		~PFF_editor() {}

		bool init() override;
		std::unique_ptr<basic_mesh> create_cube_model(glm::vec3 offset);

	private:
		editor_layer* m_editor_layer;
	};

	application* create_application() {

		return new PFF_editor();
	}

	// ============================================================================= implementstion =============================================================================

	bool PFF_editor::init() {

		m_editor_layer = new editor_layer(get_imgui_layer()->get_context());
		push_overlay(m_editor_layer);

		LOG(Trace, "register player controller");
		register_player_controller(std::make_shared<editor_controller>());

		// TODO: move to map init() function
		std::shared_ptr<basic_mesh> model = create_cube_model({ 0.0f, 0.0f, 0.0f });
#if 1
		u32 counter = 0;
		for (int16 x = -13; x < 14; x++) {
			for (int16 y = -6; y < 7; y++) {

				auto cube = get_current_map()->create_empty_game_object();
				cube->mesh = model;
				cube->transform.translation = { x * 2, y * 2, 30.f };
				cube->rotation_speed = glm::linearRand(glm::vec3(0.0f), glm::vec3(glm::two_pi<f32>()));
				counter++;
			}
		}
		LOG(Info, "Num of objects: " << counter);
#else
		auto cube = get_current_map()->create_empty_game_object();
		cube->mesh = model;
		cube->transform.translation = { .0f, .0f, 10.f };
		cube->rotation_speed = glm::linearRand(glm::vec3(0.0f), glm::vec3(glm::two_pi<f32>()));
#endif // 1

		return true;
	}


	std::unique_ptr<basic_mesh> PFF_editor::create_cube_model(glm::vec3 offset) {

		std::vector<basic_mesh::vertex> vertices{

			// left face (gray)
			{{-.5f, -.5f, -.5f}, {0.2f, 0.2f, 0.2f}},
			{{-.5f, .5f, .5f}, {0.2f, 0.2f, 0.2f}},
			{{-.5f, -.5f, .5f}, {0.2f, 0.2f, 0.2f}},
			{{-.5f, -.5f, -.5f}, {0.2f, 0.2f, 0.2f}},
			{{-.5f, .5f, -.5f}, {0.2f, 0.2f, 0.2f}},
			{{-.5f, .5f, .5f}, {0.2f, 0.2f, 0.2f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {0.6f, 0.6f, 0.0f}},
			{{.5f, .5f, .5f}, {0.6f, 0.6f, 0.0f}},
			{{.5f, -.5f, .5f}, {0.6f, 0.6f, 0.0f}},
			{{.5f, -.5f, -.5f}, {0.6f, 0.6f, 0.0f}},
			{{.5f, .5f, -.5f}, {0.6f, 0.6f, 0.0f}},
			{{.5f, .5f, .5f}, {0.6f, 0.6f, 0.0f}},

			// top face (orange)
			{{-.5f, -.5f, -.5f}, {0.0f, 0.6f, 0.6f}},
			{{.5f, -.5f, .5f}, {0.0f, 0.6f, 0.6f}},
			{{-.5f, -.5f, .5f}, {0.0f, 0.6f, 0.6f}},
			{{-.5f, -.5f, -.5f}, {0.0f, 0.6f, 0.6f}},
			{{.5f, -.5f, -.5f}, {0.0f, 0.6f, 0.6f}},
			{{.5f, -.5f, .5f}, {0.0f, 0.6f, 0.6f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {0.6f, 0.0f, 0.0f}},
			{{.5f, .5f, .5f}, {0.6f, 0.0f, 0.0f}},
			{{-.5f, .5f, .5f}, {0.6f, 0.0f, 0.0f}},
			{{-.5f, .5f, -.5f}, {0.6f, 0.0f, 0.0f}},
			{{.5f, .5f, -.5f}, {0.6f, 0.0f, 0.0f}},
			{{.5f, .5f, .5f}, {0.6f, 0.0f, 0.0f}},

			// front face (blue)
			{{-.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.6f}},
			{{.5f, .5f, 0.5f}, {0.0f, 0.0f, 0.6f}},
			{{-.5f, .5f, 0.5f}, {0.0f, 0.0f, 0.6f}},
			{{-.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.6f}},
			{{.5f, -.5f, 0.5f}, {0.0f, 0.0f, 0.6f}},
			{{.5f, .5f, 0.5f}, {0.0f, 0.0f, 0.6f}},

			// back face (green)
			{{-.5f, -.5f, -0.5f}, {0.0f, 0.6f, 0.0f}},
			{{.5f, .5f, -0.5f}, {0.0f, 0.6f, 0.0f}},
			{{-.5f, .5f, -0.5f}, {0.0f, 0.6f, 0.0f}},
			{{-.5f, -.5f, -0.5f}, {0.0f, 0.6f, 0.0f}},
			{{.5f, -.5f, -0.5f}, {0.0f, 0.6f, 0.0f}},
			{{.5f, .5f, -0.5f}, {0.0f, 0.6f, 0.0f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<PFF::basic_mesh>(vertices);
	}

}