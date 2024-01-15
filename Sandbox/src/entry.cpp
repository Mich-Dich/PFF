
#include <PFF.h>

#include "player/pc_default.h"

class Sandbox : public PFF::application {

public:

	Sandbox() {
		
		LOG(Debug, "Sandbox Constructor");
		init();
	}
	~Sandbox() {}

	bool init() override;
	std::unique_ptr<PFF::basic_mesh> create_cube_model(glm::vec3 offset);

};

PFF::application* PFF::create_application() {

	return new Sandbox();
}

bool Sandbox::init() {

	LOG(Trace, "register player controller");
	Sandbox::register_player_controller(std::make_shared<pc_default>());

	// TODO: move to map init() function
#if 0
	u32 counter = 0;
	std::shared_ptr<PFF::basic_mesh> model = create_cube_model({ 0.0f, 0.0f, 0.0f });
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
	std::shared_ptr<PFF::basic_mesh> model = create_cube_model({ 0.0f, 0.0f, 0.0f });
	auto cube = get_current_map()->create_empty_game_object();
	cube->mesh = model;
	cube->transform.translation = { .0f, .0f, 10.f };
	cube->rotation_speed = glm::linearRand(glm::vec3(0.0f), glm::vec3(glm::two_pi<f32>()));
#endif // 1


	return true;
}


std::unique_ptr<PFF::basic_mesh> Sandbox::create_cube_model(glm::vec3 offset) {

	std::vector<PFF::basic_mesh::vertex> vertices{

		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

	};
	for (auto& v : vertices) {
		v.position += offset;
	}
	return std::make_unique<PFF::basic_mesh>(vertices);
}
