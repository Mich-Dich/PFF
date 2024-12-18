#pragma once

#include "util/data_structures/string_manipulation.h"

#define TEST_NAME_CONVERTION(variable)		extract_vaiable_name(#variable)
#define SERIALIZE_KEY_VALUE(variable)		serialize_key_value(extract_vaiable_name(#variable), variable);

// @brief used with [PFF::serializer] to shorten the PFF::serializer::yaml::entry call
#define KEY_VALUE(var)						PFF::util::extract_vaiable_name(#var), var

namespace PFF::serializer {

	enum class option {

		save_to_file,
		load_from_file,
	};
	
}
