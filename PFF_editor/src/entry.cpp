
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

	private:
		editor_layer* m_editor_layer;
	};

	application* create_application() {

		return new PFF_editor();
	}

	// ============================================================================= implementstion =============================================================================

	bool PFF_editor::init() {

		// PFF_PROFILE_FUNCTION();

		m_editor_layer = new editor_layer(get_imgui_layer()->get_context());
		push_overlay(m_editor_layer);

		LOG(Trace, "register player controller");
		register_player_controller(std::make_shared<editor_controller>());
		

#if 0

		std::shared_ptr<basic_mesh> model = basic_mesh::create_mesh_from_file("assets/thruster.obj");
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

		std::shared_ptr<basic_mesh> model = basic_mesh::create_mesh_from_file("assets/thruster.obj");
		auto test_obj = get_current_map()->create_empty_game_object();
		test_obj->mesh = model;
		test_obj->transform.translation = { .0f, 0.f, 0.f };
		test_obj->transform.scale = glm::vec3(1.f);
		test_obj->rotation_speed = glm::vec3(0.f, .5f, 0.f);

		std::shared_ptr<basic_mesh> floor = basic_mesh::create_mesh_from_file("assets/floor.obj");
		auto floor_obj = get_current_map()->create_empty_game_object();
		floor_obj->mesh = floor;
		floor_obj->transform.translation = { .0f, 2.f, 0.f };
		floor_obj->transform.scale = glm::vec3(1.f);
#endif // 1

		// TODO: load editor camera loc && rot

		glm::vec2 camera_pos{ 10.f, 0.f };
		glm::vec2 camera_direction{ 0.f, 0.f };

		// config::save(config::file::editor, "editor_camera", "position", camera_pos);
		// config::save(config::file::editor, "editor_camera", "direction", camera_direction);

		//get_world_layer()->get_editor_camera()->set_view_XYZ(camera_pos, camera_direction);

		return true;
	}

}
