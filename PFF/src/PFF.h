#pragma once

// This file contains everything that should be exposed to the user
// MAYBE: divide into category based sub files when [compile-time/file-size] to high

// -------------------------- external --------------------------
#include <glm/glm.hpp>

// -------------------------- util --------------------------
#include "util/pffpch.h"
#include "util/io/io_handler.h"
#include "util/io/serializer_data.h"
#include "util/io/serializer_yaml.h"
#include "util/io/serializer_binary.h"
#include "util/math/constance.h"
#include "util/math/math.h"
#include "util/math/noise.h"
#include "util/math/random.h"
#include "util/ui/imgui_markdown.h"
#include "util/ui/pannel_collection.h"
#include "util/UUID.h"
#include "util/profiling/instrumentor.h"

#include "application.h"

#include "engine/Events/Event.h"
#include "engine/game_objects/player_controller.h"
#include "engine/layer/layer_stack.h"
#include "engine/world/map.h"
#include "engine/world/entity.h"
#include "engine/world/components.h"
#include "engine/world/entity_script.h"
#include "engine/resource_management/static_mesh_asset_manager.h"

// -------------------------- project --------------------------
#include "project/project_macros.h"

// -------------------------- procedural --------------------------
#include "procedural/procedural_mesh_script.h"
