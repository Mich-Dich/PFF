
#include <PFF.h>

#include "controller/editor_controller.h"
#include "ui/editor_layer.h"
#include "util/io/config.h"

namespace PFF {

	class PFF_editor : public application {

	public:

		PFF_editor() {

			LOG(Debug, "PFF_editor Constructor");
			init();
		}

		~PFF_editor() {

			shutdown();
		}

		bool init() override;
		bool shutdown() override;
		bool update(const f32 delta_time) override;

	private:
		std::shared_ptr<editor_controller> m_editor_controller{};
		editor_layer* m_editor_layer;
		PFF::game_object* m_test_game_object;
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
		m_editor_controller = std::make_shared<editor_controller>();
		register_player_controller(m_editor_controller);
		

#if 0		// populate scene with grid of meshes

		std::shared_ptr<basic_mesh> model = basic_mesh::create_mesh_from_file("assets/smooth_vase.obj");
		u32 counter = 0;
		for (int16 x = -10; x < 10; x++) {
			for (int16 y = -10; y < 10; y++) {

				auto test_obj = get_current_map()->create_empty_game_object();
				test_obj->mesh = model;
				test_obj->transform.translation = { (x * 2) + 1, (y * 2) + 1, 0.5f };
				test_obj->transform.scale = glm::vec3(2.f);
				test_obj->rotation_speed = glm::linearRand(glm::vec3(0.0f), glm::vec3(glm::two_pi<f32>()));
				counter++;
			}
		}
		LOG(Info, "Num of objects: " << counter);

#else


#endif // 1

		std::shared_ptr<basic_mesh> floor = basic_mesh::create_mesh_from_file("assets/floor.obj");
		auto floor_obj = get_current_map()->create_empty_game_object();
		floor_obj->mesh = floor;
		floor_obj->transform.translation = { 0.f, 0.f, 0.f };
		floor_obj->transform.scale = glm::vec3(2.5f);

		std::shared_ptr<basic_mesh> model = basic_mesh::create_mesh_from_file("assets/untitled.obj");
		auto test_1 = get_current_map()->create_empty_game_object();
		test_1->mesh = model;
		test_1->set_translation({ 0.f, -10.f, 0.f });
		test_1->transform.scale = glm::vec3(1.f);

		auto test_2 = get_current_map()->create_empty_game_object();
		test_2 ->mesh = model;
		test_2 ->set_translation({ 0.f, 10.f, 0.f });
		test_2 ->transform.scale = glm::vec3(1.f);


		// std::shared_ptr<basic_mesh> model = basic_mesh::create_mesh_from_file("assets/untitled.obj");
		m_test_game_object = get_current_map()->create_empty_game_object();
		m_test_game_object->mesh = model;
		m_test_game_object->set_translation({ .0f, -10.f, 0.f });
		m_test_game_object->transform.scale = glm::vec3(1.f);
		// m_test_game_object->rotation_speed = glm::vec3(0.f, .5f, 0.f);

		LOG(Fatal, "position: " << m_test_game_object->transform.translation.y);


		// TODO: load editor camera loc && rot
		glm::vec3 camera_pos{ 0.0f, 10.0f, 10.0f };
		glm::vec3 camera_direction{};

		config::load(config::file::editor, "editor_camera", "pos", camera_pos);
		config::load(config::file::editor, "editor_camera", "direction", camera_direction);

		m_editor_controller->set_editor_camera_pos(camera_pos);
		m_editor_controller->set_editor_camera_direction(camera_direction);

		LOG(Fatal, "position: " << m_test_game_object->transform.translation.y);
		return true;
	}

	bool PFF_editor::shutdown() {

		// save camera position
		glm::vec3 camera_pos = m_editor_controller->get_editor_camera_pos();
		glm::vec3 camera_direction = m_editor_controller->get_editor_camera_direction();

		config::save(config::file::editor, "editor_camera", "pos", camera_pos);
		config::save(config::file::editor, "editor_camera", "direction", camera_direction);

		return true;
	}

	bool PFF_editor::update(const f32 delta_time) {

		static bool move_positive{};

		if (m_test_game_object->transform.translation.y >= 10.f)
			move_positive = false;

		if (m_test_game_object->transform.translation.y <= -10.f)
			move_positive = true;


		if (move_positive)
			m_test_game_object->transform.translation.y += 10.f * delta_time;
		else
			m_test_game_object->transform.translation.y -= 10.f * delta_time;
		
		return false;
	}

}
