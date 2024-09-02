#pragma once

//#include <entt/entt.hpp>
#include "engine/world/entity.h"

namespace PFF {

    class world_layer;

    typedef void (*editor_update_script_Fn)(entt::registry&, float);
    typedef void (*init_scripts_Fn)(entt::registry*);
    typedef void (*delete_scripts_Fn)();
    typedef void (*add_component_Fn)(std::string, PFF::entity);
    //typedef void (*ImGui_Fn)(entity);

    namespace script_system {

        PFF_API void init();
        PFF_API void shutdown();
        PFF_API void editor_update(float delta_time);
        PFF_API void reinit_scripts();
        PFF_API void reload(bool delete_script_components = false);
        PFF_API bool free_script_library(bool delete_script_components = false);

        PFF_API void add_component_from_string(std::string class_name, PFF::entity entity);
        //PFF_API void imGui(entity entity);
    };

}
