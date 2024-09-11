#pragma once

#include "engine/world/entity.h"

namespace PFF {

    class world_layer;

    namespace script_system {

        PFF_API void init();
        PFF_API void shutdown();
        PFF_API bool free_script_library(bool delete_script_components = false);

        PFF_API void editor_update(float delta_time);
        PFF_API void reinit_scripts();
        PFF_API void reload(bool delete_script_components = false);
        PFF_API void add_component_from_string(std::string class_name, PFF::entity entity);
        PFF_API void display_properties(std::string class_name, entity_script* script);

        PFF_API void serialize_script(std::string class_name, entity_script* script, serializer::yaml& serializer);

        FORCEINLINE PFF_API bool is_ready();
        FORCEINLINE PFF_API const char** get_all_procedural_mesh_scripts(u32* count);
        FORCEINLINE PFF_API const char** get_all_scripts(u32* count);
    };

}
