#pragma once

#include "util/pffpch.h"

namespace PFF {

	struct transform_component {

		PFF_DEFAULT_CONSTRUCTORS(transform_component);
		transform_component(const glm::mat4& transform)
			: transform(transform) {}

		glm::mat4 transform;

		PFF_IMPLISITE_CASTS(glm::mat4, transform);
	};

	struct ID_component {

		UUID ID;
		PFF_DEFAULT_CONSTRUCTORS(ID_component);
	};

	struct tag_component {
		std::string tag;

		PFF_DEFAULT_CONSTRUCTORS(tag_component);
		tag_component(const std::string& tag)
			: tag(tag) {}
	};

}
