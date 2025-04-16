#pragma once

#include "util/pffpch.h"
#include <filesystem>
#include <string>

namespace PFF {

    // -------------------------------------------------------------------------------------------------------------------------------------
    // GUIDELINE
    //      saved file layout:
    //          asset_file_header           <- general data for any asset (same for all files)
    //          general_header              <- the same for all files
    //          specific_header
    //          binary_blob
    // -------------------------------------------------------------------------------------------------------------------------------------

    enum class texture_type : u64 {
        texture2D,
        texture3D,
        cube_map,
        // Add other texture types when developed
    };

    // ---------------------- GENERAL ------------------------------------------------------------------------------------------------------
    struct general_texture_file_header {
        texture_type            type;
        version                 file_version{};
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------- SPECIFIC -----------------------------------------------------------------------------------------------------
    struct specific_texture_file_header {
        std::string             name{};
        std::filesystem::path   source_file{};        // need to also save path_size if required
        u32                     width{};              // width of the texture in pixels
        u32                     height{};             // height of the texture in pixels
        u32                     mip_levels{};         // number of mipmap levels
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

}
