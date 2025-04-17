#pragma once

#include "util/pffpch.h"
#include "engine/render/material.h"
#include <filesystem>
#include <string>

namespace PFF {

    // ============================================================== MATERIAL =============================================================
    // GUIDELINE
    //      saved file layout:
    //          asset_file_header           <- general data for any asset (same for all files)
    //          general_header              <- the same for all files
    //          specific_header             <- contains the data needed to create a material instance
    //          binary_blob                 <- the material should expose some vars the instance can change (e.g: float, texture, ...) AND a function that serializes this data
    // -------------------------------------------------------------------------------------------------------------------------------------

    enum class material_type : u64 {
        opaque, 
    };

    // ---------------------- GENERAL ------------------------------------------------------------------------------------------------------
    struct general_material_file_header {
        version                             file_version{};         // header version
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------- SPECIFIC -----------------------------------------------------------------------------------------------------
    struct specific_texture_file_header {
        std::string                         name{};
        std::filesystem::path               source_file{};          // need to also save path_size if required
        material_type                       material_type{};

        material                            parent_material{};      // need to call create_instance() from this
        material::material_resources        resources{};
    };
    // -------------------------------------------------------------------------------------------------------------------------------------







    // ========================================================= MATERIAL INSTANCE =========================================================
    // GUIDELINE
    //      saved file layout:
    //          asset_file_header           <- general data for any asset (same for all files)
    //          general_header              <- the same for all files
    //          specific_header             <- contains the data needed to create a material instance
    //          binary_blob                 <- the material should expose some vars the instance can change (e.g: float, texture, ...) AND a function that serializes this data
    // -------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------- GENERAL ------------------------------------------------------------------------------------------------------
    struct general_material_file_header {
        version                             file_version{};         // header version
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------- SPECIFIC -----------------------------------------------------------------------------------------------------
    struct specific_texture_file_header {
        std::filesystem::path               source_file{};          // need to also save path_size if required

        material                            parent_material{};      // need to call create_instance() from this
        material::material_resources        resources{};
    };
    // -------------------------------------------------------------------------------------------------------------------------------------

}
