#pragma once

#include "engine/geometry/mesh.h"

namespace PFF::IO {

    namespace mesh_loader {

        std::optional<std::vector<ref<PFF::geometry::mesh_asset>>> load_gltf_meshes(std::filesystem::path filePath);        // DEV-ONLY 

    }

}
