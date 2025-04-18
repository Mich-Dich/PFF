
#include "util/pffpch.h"

#include "application.h"
#include "engine/resource_management/headers.h"

#include "material_asset_manager.h"

namespace PFF {

	material_asset_manager material_asset_manager::s_instance;

	material_asset_manager::material_asset_manager() {}

	
	ref<material> get_material_from_path(const std::filesystem::path path) {
		
		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");
		return nullptr;
	}

	ref<material_instance> get_material_instance_from_path(const std::filesystem::path path) {
		
		VALIDATE(path.extension() == PFF_ASSET_EXTENTION, return nullptr, "", "Provided path is not a PFF-asset");
		return nullptr;
	}


}
