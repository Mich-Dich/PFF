
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

};

PFF::application* PFF::create_application() {

	return new Sandbox();
}

bool Sandbox::init() {

	LOG(Trace, "register player controller");
	Sandbox::register_player_controller(std::make_shared<pc_default>());
	return true;
}
