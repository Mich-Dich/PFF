#pragma once

#include "util/pffpch.h"

namespace PFF {

    // -------------------------------------------------------------------------------------------------------------------------------------
    // GUIDLINE
    //      saved file layout:
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
    struct general_file_header {

        mesh_type       type;
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------- SPECIFIC -----------------------------------------------------------------------------------------------------
	struct static_mesh_header {

		u16                     version;
        std::filesystem::path   source_file;        // need to also save path_size
        size_t                  mesh_index;         // index of this mesh from the original gltf-file
	};

    // ...

    // -------------------------------------------------------------------------------------------------------------------------------------

}