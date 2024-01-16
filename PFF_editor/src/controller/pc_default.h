#pragma once

#include "engine/game_objects/player_controller.h"

namespace PFF {

	class pc_default : public player_controller {
	public:
		pc_default();
		~pc_default() {};

	private:

	};

	class IAM_default : public input_mapping {

	};

}
