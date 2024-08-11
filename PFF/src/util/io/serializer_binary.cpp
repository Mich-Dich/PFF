
#include "util/pffpch.h"

#include "serializer_binary.h"


namespace PFF::serializer {



	binary::binary(const std::filesystem::path filename, const std::string& section_name, option option) 
	: m_filename(filename), m_name(section_name), m_option(option) {


		if (m_option == option::save_to_file) {

			m_ostream = std::ofstream(m_filename, std::ios::binary);
			CORE_VALIDATE(m_ostream, return, "", "Failed to create file: [" << m_filename << "]");

		} else {

			m_istream = std::ifstream(m_filename, std::ios::binary);
			CORE_VALIDATE(m_istream, return, "", "Failed to create file: [" << m_filename << "]");

		}

	}

	binary::~binary() {

		if (m_option == option::save_to_file) {

			m_ostream.close();

		} else {

			m_istream.close();
		}
	}

}


//// TODO:   !!!!!!!  REWRITE  !!!!!!!  THIS IS UGLY AND UNSAVE  !!!!!!!
//file.write(reinterpret_cast<const char*>(&general_header), sizeof(general_file_header));
//file.write(reinterpret_cast<const char*>(&static_mesh_header), sizeof(PFF::static_mesh_header));
//
//file.write(reinterpret_cast<const char*>(&loc_mesh_assets.value()[x]->bounds), sizeof(geometry::bounds));
//file.write(reinterpret_cast<const char*>(loc_mesh_assets.value()[x]->surfaces.data()), sizeof(geometry::Geo_surface)* loc_mesh_assets.value()[x]->surfaces.size());
//file.write(reinterpret_cast<const char*>(loc_mesh_assets.value()[x]->vertices.data()), sizeof(geometry::vertex)* loc_mesh_assets.value()[x]->vertices.size());
//file.write(reinterpret_cast<const char*>(loc_mesh_assets.value()[x]->indices.data()), sizeof(u32)* loc_mesh_assets.value()[x]->indices.size());
//
//file.close();
