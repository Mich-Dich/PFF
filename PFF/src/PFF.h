#pragma once

// This file contains everything that should be exposed to the user
// MAYBE: divide into category based sub files when [compile-time/file-size] to high

// -------------------------- external --------------------------
#include <glm/glm.hpp>

#include "application.h"

// -------------------------- util --------------------------
#include "util/pffpch.h"
#include "util/util.h"													// this includes all util sub files

// -------------------------- engine --------------------------
#include "engine/events/event.h"
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
