
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
		std::unique_ptr<static_mesh> create_cube_model(glm::vec3 offset);

	private:
		editor_layer* m_editor_layer;
	};

	application* create_application() {

		return new PFF_editor();
	}

	// ============================================================================= implementstion =============================================================================

	bool PFF_editor::init() {

		PFF_PROFILE_FUNCTION();

		m_editor_layer = new editor_layer(get_imgui_layer()->get_context());
		push_overlay(m_editor_layer);
		{
			PFF_PROFILE_SCOPE("test1");

			LOG(Trace, "register player controller");
			register_player_controller(std::make_shared<editor_controller>());
		}

		std::shared_ptr<basic_mesh> model = basic_mesh::create_mesh_from_file("assets/untitled.obj");
#if 0
		u32 counter = 0;
		for (int16 x = -13; x < 14; x++) {
			for (int16 y = -6; y < 7; y++) {

				auto test_obj = get_current_map()->create_empty_game_object();
				test_obj->mesh = model;
				test_obj->transform.translation = { x * 2, y * 2, 30.f };
				test_obj->transform.scale = glm::vec3(3.f);
				test_obj->rotation_speed = glm::linearRand(glm::vec3(0.0f), glm::vec3(glm::two_pi<f32>()));
				counter++;
			}
		}
		LOG(Info, "Num of objects: " << counter);
#else
		auto test_obj = get_current_map()->create_empty_game_object();
		test_obj->mesh = model;
		test_obj->transform.translation = { .0f, 0.f, 10.f };
		test_obj->transform.scale = glm::vec3(1.f);
		//test_obj->rotation_speed = glm::vec3(0.f, .5f, 0.f);
#endif // 1

		return true;
	}


	std::unique_ptr<static_mesh> PFF_editor::create_cube_model(glm::vec3 offset) {

		static_mesh::builder modle_builder{};
		modle_builder.vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : modle_builder.vertices) {
			v.position += offset;
		}

		modle_builder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<PFF::static_mesh>(modle_builder);
	}

}