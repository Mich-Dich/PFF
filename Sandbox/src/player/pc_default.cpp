
#include <util/pffpch.h>
#include <engine/Events/Event.h>

#include "pc_default.h"
#include "input_mapping.h"



pc_default::pc_default() {

	// register Input mapping
	LOG(Debug, "register Input mapping");
	m_input_mapping = default_IAM{};
}

