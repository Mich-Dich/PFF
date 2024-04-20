#pragma once

#include "engine/geometry/geometry.h"

namespace PFF::IO {

    namespace mesh_loader {

        std::optional<std::vector<std::shared_ptr<PFF::geometry::mesh_asset>>> load_gltf_meshes(std::filesystem::path filePath);

    }

}
