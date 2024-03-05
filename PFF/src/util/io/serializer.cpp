
#include "util/pffpch.h"

#include "serializer.h"

namespace PFF::serializer {

	// ================================================== yaml ==================================================

	yaml::yaml(const std::string& path, const std::string& filename, serializer_option option)
		: m_path(path), m_filename(filename), m_option(option) {

		CORE_ASSERT(io_handler::create_directory(path), "", "Could not create file-path");
		m_filepath = path + "/" + filename;

		switch (option) {
		case serializer_option::save_to_file:

			m_ostream = std::ofstream(m_filepath);
			break;

		case serializer_option::load_from_file:

			CORE_VALIDATE(std::filesystem::exists(m_filepath), , "", "file: [" << m_filepath << "] does not exist");
			m_istream = std::ifstream(m_filepath);
			break;

		default:
			DEBUG_BREAK();
			break;
		}
	}

	yaml::~yaml() {}

	void yaml::serialize_struct_begining(const std::string& name) {

		CORE_ASSERT(m_ostream.is_open(), "", "output-file-stream is not open");
		m_ostream << name << ":\n";
	}


	// ================================================== yaml ==================================================

}
