
#include "util/pffpch.h"

#include "engine/resource_management/static_mesh_asset_manager.h"
#include "engine/resource_management/mesh_serializer.h"

#include "entity.h"

namespace PFF {

	void entity::add_mesh_component(mesh_component& mesh_comp) {

		CORE_ASSERT(!mesh_comp.asset_path.empty(), "", "Provided path is empty");

		asset_file_header asset_header;
		general_mesh_file_header general_header;
		auto serializer = serializer::binary(mesh_comp.asset_path, "PFF_asset_file", serializer::option::load_from_file);
		serialize_mesh_headers(serializer, asset_header, general_header);
		CORE_ASSERT(asset_header.type == file_type::mesh, "", "Tryed to add mesh_component but provided asset_path is not a mesh");

		switch (general_header.type) {
		case mesh_type::static_mesh:
		{

			static_mesh_file_header static_mesh_header;
			serialize_static_mesh_header(serializer, static_mesh_header);

			mesh_comp.mesh_asset = static_mesh_asset_manager::get_from_path(util::extract_path_from_project_content_folder(mesh_comp.asset_path));

			//TODO: check 
			//		if (user defined a material_instance) || ([static_mesh_header] has a material_instance)
			//		else use default_material
			CORE_LOG(Error, "Added mesh comp <= STILL NEED TO CHECK FOR DEFAULT MATERIAL");

		} break;

		case mesh_type::dynamic_mesh:
		case mesh_type::skeletal_mesh:
		case mesh_type::mesh_collection:
		case mesh_type::procedural_mesh:
			CORE_LOG(Trace, "Adding of mesh_type not supported yet"); break;

		default: CORE_LOG(Trace, "unidentified [mesh_type] used"); break;
		}


		// ---------------------------------- does not have mesh component  => add component to root ----------------------------------
		if (!has_component<mesh_component>()) {		

			add_component<mesh_component>(mesh_comp);
			return;
		}

		// ---------------------------------- already has mesh_component  => add new entity and make child of this entity ----------------------------------
		entity child = m_map->create_entity("SM_" + mesh_comp.asset_path.filename().replace_extension("").string());
		child.add_component<relationship_component>(get_component<ID_component>().ID);		// register child
		child.add_component<mesh_component>(mesh_comp);

		// ---------------------------------- current entity already has relationship ----------------------------------
		if (has_component<relationship_component>()) {
			
			get_component<relationship_component>().children_ID.push_back(child.get_component<ID_component>().ID);
			return;
		}

		std::vector<UUID> cildren{};
		cildren.push_back(child.get_component<ID_component>().ID);
		add_component<relationship_component>(NULL, cildren);		// register child
		return;

	}

}
