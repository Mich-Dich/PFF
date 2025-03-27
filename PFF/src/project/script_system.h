#pragma once

#include "engine/world/entity.h"

namespace PFF {

    class world_layer;

    namespace script_system {

        void init();
        void shutdown();
        bool free_script_library(bool delete_script_components = false);

        void editor_update(float delta_time);
        void reinit_scripts();
        void reload(bool delete_script_components = false);
        void add_component_from_string(std::string class_name, PFF::entity entity);
        void display_properties(std::string class_name, entity_script* script);

        void serialize_script(std::string class_name, entity_script* script, serializer::yaml& serializer);

        bool is_ready();
        const char** get_all_procedural_mesh_scripts(u32* count);
        const char** get_all_scripts(u32* count);
    };

}
