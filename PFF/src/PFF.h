#pragma once

#include "util/pffpch.h"

#include <glm/glm.hpp>

#include "util/io/io_handler.h"
#include "util/io/serializer_data.h"
#include "util/io/serializer_yaml.h"
#include "util/io/serializer_binary.h"

#include "util/math/constance.h"
#include "util/math/math.h"
#include "util/math/noise.h"
#include "util/math/random.h"

#include "application.h"
#include "entry_point.h"
#include "util/pffpch.h"

#include "engine/Events/Event.h"
#include "engine/game_objects/player_controller.h"
#include "engine/layer/layer_stack.h"


#include "engine/world/map.h"
#include "engine/world/entity.h"
#include "engine/world/components.h"
#include "engine/world/entity_script.h"


// ================================ IN DEV ================================
#include "engine/resource_management/static_mesh_asset_manager.h"
// ================================ IN DEV ================================

#include "util/ui/imgui_markdown.h"
#include "util/UUID.h"


#include "project/project_macros.h"
