#pragma once

#include "util/util.h"

namespace PFF::config {

	enum class config_file_types {
		default_editor,
		default_engine,
		default_game,
		default_input,
	};

	std::string configtype_to_string(config_file_types type);

	void init();

}