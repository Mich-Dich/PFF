#pragma once

#include <entt.hpp>
#include "engine/world/entity.h"

namespace PFF {

    class world_layer;

    typedef void (*add_component_Fn)(std::string, entt::entity);
    typedef void (*update_script_Fn)(entt::registry&, float);
    typedef void (*editor_update_script_Fn)(entt::registry&, float);
    typedef void (*serialize_scripts_Fn)(const serializer::option, const std::filesystem::path&, PFF::entity);
    typedef void (*init_scripts_Fn)(world_layer*);
    typedef void (*init_ImGui_Fn)(void*);
    typedef void (*ImGui_Fn)(entity);
    typedef void (*delete_scripts_Fn)();
    typedef void (*notify_begin_contact_Fn)(entity, entity);
    typedef void (*notify_end_contact_Fn)(entity, entity);

    namespace script_system {

        PFF_API void init();
        PFF_API void shutdown();

        PFF_API void update(float delta_time);
        PFF_API void editor_update(float delta_time);

        PFF_API void imGui(entity entity);
        PFF_API void init_ImGui(void* context);

        PFF_API void notify_begin_contact(entity entityA, entity entityB);
        PFF_API void notify_end_contact(entity entityA, entity entityB);

        PFF_API void reload(bool delete_script_components = false);
        PFF_API void serialize(serializer::option option, entity entity);

        PFF_API bool free_script_library(bool delete_script_components = false);
        PFF_API void addComponentFromString(std::string className, entt::entity entity, entt::registry& registry);
    };

}
