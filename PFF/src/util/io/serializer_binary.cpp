
#include "util/pffpch.h"

#include "serializer_binary.h"

namespace PFF::serializer {

	binary::binary(const std::filesystem::path filename, const std::string& section_name, option option) 
	: m_filename(filename), m_name(section_name), m_option(option) {

		// ASSERT(std::filesystem::is_regular_file(filename), "", "Provided filepath is not a file [" << filename.generic_string() << "]");
		if (m_option == option::save_to_file) {

			m_ostream = std::ofstream(m_filename, std::ios::out | std::ios::binary | std::ios::trunc);
			VALIDATE(m_ostream, return, "", "Failed to save to file: [" << m_filename << "]");

		} else {

			m_istream = std::ifstream(m_filename, std::ios::in | std::ios::binary);
			VALIDATE(m_istream, return, "", "Failed to load file: [" << m_filename << "]");

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
