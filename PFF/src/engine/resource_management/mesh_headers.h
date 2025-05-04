#pragma once

#include "util/pffpch.h"

namespace PFF {

    // -------------------------------------------------------------------------------------------------------------------------------------
    // GUIDLINE
    //      saved file layout:
    //          asset_file_header           <- general data for any asset (same for all files)
    //          general_header              <- the same for all files
    //          spezific_header
    //          binary_blob
    // -------------------------------------------------------------------------------------------------------------------------------------

    enum class mesh_type : u64 {
        static_mesh,
        procedural_mesh,
        dynamic_mesh,
        skeletal_mesh,
        mesh_collection,                        // for destructable meshes
        // Add other mesh types when developed
    };

    // ---------------------- GENERAL ------------------------------------------------------------------------------------------------------
    struct general_mesh_file_header {

        mesh_type       type;
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------- SPECIFIC -----------------------------------------------------------------------------------------------------
	struct static_mesh_file_header {

        std::string             name{};
		u16                     version{};
        std::filesystem::path   source_file{};        // need to also save path_size
        size_t                  mesh_index{};         // index of this mesh from the original gltf-file
        PFF::geometry::bounds   mesh_bounds{};
	};
    // -------------------------------------------------------------------------------------------------------------------------------------

}
