#pragma once

//#include <entt/entt.hpp>
#include "engine/world/entity.h"

namespace PFF {

    class world_layer;

    typedef void (*editor_update_script_Fn)(entt::registry&, float);
    typedef void (*init_scripts_Fn)(entt::registry*);
    typedef void (*delete_scripts_Fn)();
    typedef void (*add_component_Fn)(std::string, PFF::entity);
    typedef void (*display_properties_Fn)(std::string, entity_script*);
    typedef const char** (*get_scripts_Fn)(u32* count);

    namespace script_system {

        PFF_API void init();
        PFF_API void shutdown();
        PFF_API bool free_script_library(bool delete_script_components = false);

        PFF_API void editor_update(float delta_time);
        PFF_API void reinit_scripts();
        PFF_API void reload(bool delete_script_components = false);
        PFF_API void add_component_from_string(std::string class_name, PFF::entity entity);
        PFF_API void display_properties(std::string class_name, entity_script* script);

        FORCEINLINE PFF_API const char** get_all_procedural_mesh_scripts(u32* count);
        FORCEINLINE PFF_API const char** get_all_scripts(u32* count);
    };

}
