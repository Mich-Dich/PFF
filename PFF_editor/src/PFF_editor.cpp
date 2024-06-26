
#include "util/pch_editor.h"

#include "PFF_editor.h"

namespace PFF {

	void PFF_editor::serialize(serializer::option option) {

		// list all serialize functions
		m_editor_controller->serialize(option);
	}

	PFF_editor::PFF_editor() {

		LOG(Trace, "register player controller");

		m_editor_layer = new editor_layer(application::get().get_imgui_layer()->get_context());
		push_overlay(m_editor_layer);

		m_editor_controller = std::make_shared<editor_controller>();
		register_player_controller(m_editor_controller);
				
		serialize(serializer::option::load_from_file);	
	}

	PFF_editor::~PFF_editor() {

		serialize(serializer::option::save_to_file);

		pop_overlay(m_editor_layer);
		delete m_editor_layer;
	}

	bool PFF_editor::init() { return true; }

	bool PFF_editor::shutdown() { return true; }

	bool PFF_editor::render(const f32 delta_time) { return true; }

	bool PFF_editor::update(const f32 delta_time) { return true; }
}





/*

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
	
		return true;
	}




	bool PFF_editor::init() {

		LOG(Trace, "init editor logic");

		auto floor_obj = get_current_map()->create_empty_game_object();
		floor_obj->mesh = basic_mesh::create_mesh_from_file("assets/floor.obj");
		floor_obj->transform.translation = { 0.f, 0.f, 0.f };
		floor_obj->transform.scale = glm::vec3(2.5f);

#if 1		// populate scene with grid of meshes

		u32 counter = 0;
		for (int16 x = -10; x < 10; x++) {
			for (int16 y = -10; y < 10; y++) {

				auto test_obj = get_current_map()->create_empty_game_object();
				test_obj->mesh = basic_mesh::create_mesh_from_file("assets/smooth_vase.obj");
				test_obj->transform.translation = { (x * 2) + 1, (y * 2) + 1, 0.5f };
				test_obj->transform.scale = glm::vec3(2.f);
				test_obj->rotation_speed = glm::linearRand(glm::vec3(0.0f), glm::vec3(glm::two_pi<f32>()));
				counter++;
			}
		}
		LOG(Info, "Num of objects: " << counter);

#else
		// untitled / thruster
		m_test_game_object = get_current_map()->create_empty_game_object();
		m_test_game_object->mesh = basic_mesh::create_mesh_from_file("assets/untitled.obj");
		m_test_game_object->set_translation({ .0f, 0.f, 2.f });
		m_test_game_object->transform.scale = glm::vec3(1.f);
		m_test_game_object->rotation_speed = glm::vec3(0.f, 3.5f, 0.f);

#endif // 1
		return true;
	}



*/