#pragma once

#include "engine/game_objects/player_controller.h"

class pc_default : public PFF::player_controller {
public:
	pc_default() {
		LOG(Trace, "player controller constructor");
		init();
	};
	~pc_default() {};

	void init() override;

private:

};

class IAM_default : public PFF::input_mapping {

};