
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
