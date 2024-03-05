
#include "util/pffpch.h"

#include "serializer.h"

namespace PFF::serializer {

	// ================================================== yaml ==================================================

	yaml::yaml(const std::string& filename, serializer_option option)
		: m_filename(filename), m_option(option) {

		LOG(Info, "called yaml() constructor");
		extract_part_befor_delimiter(m_path, filename, "/");

		CORE_ASSERT(io_handler::create_directory(m_path), "", "Could not create file-path");
		/*
		switch (option) {
		case serializer_option::save_to_file:

			m_ostream = std::ofstream(m_filename);
			break;

		case serializer_option::load_from_file:

			CORE_VALIDATE(std::filesystem::exists(m_filename), , "", "file: [" << m_filename << "] does not exist");
			m_istream = std::ifstream(m_filename);
			break;

		default:
			DEBUG_BREAK();
			break;
		}*/
	}

	yaml::~yaml() {}

	yaml& yaml::struct_name(const std::string& name) {

		switch (m_option) {
		case PFF::serializer::serializer_option::save_to_file:

			m_name = name;
			break;

		case PFF::serializer::serializer_option::load_from_file:

			m_is_correct_struct = (m_name == name);
			break;

		default:
			break;
		}

		return *this;
	}

	void yaml::serialize() {

		LOG(Info, "called yaml::serialize()");

		m_ostream = std::ofstream(m_filename);
		CORE_ASSERT(m_ostream.is_open(), "", "file-stream is not open");

		m_ostream << m_name << ":\n";

		for (auto it = m_key_value_pares.begin(); it != m_key_value_pares.end(); it++) {

			m_ostream << "  " << it->first << ": " << it->second << "\n";
			LOG(Info, "  " << it->first << ": " << it->second);
		}

	}

	yaml& yaml::deserialize() {

		LOG(Info, "called yaml::deserialize()");

		m_istream = std::ifstream(m_filename);
		CORE_ASSERT(m_istream.is_open(), "", "file-stream is not open");

		std::string line;
		while (std::getline(m_istream, line)) {

			if (line.empty())
				continue;

			if (line.back() == ':') {
				m_name = line.substr(0, line.size() - 1);

			} else {

				line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
				std::istringstream iss(line);
				std::string key, value;
				std::getline(iss, key, ':');
				std::getline(iss, value);
				m_key_value_pares[key] = value;
			}

		}
		return *this;
	}



	// ================================================== yaml ==================================================


}

namespace PFF {

	void extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter) {

		size_t found = input.find_last_of(delimiter);
		if (found != std::string::npos) {

			dest = input.substr(found + 1);
			return;
		}

		return; // If delimiter is not found
	}

	void extract_part_befor_delimiter(std::string& dest, const std::string& input, const char* delimiter) {

		size_t found = input.find_last_of(delimiter);
		if (found != std::string::npos) {

			dest = input.substr(0, found);
			return;
		}

		return; // If delimiter is not found
	}

	std::string extract_vaiable_name(const std::string& input) {

		std::string result = input;
		extract_part_after_delimiter(result, input, "->");
		extract_part_after_delimiter(result, result, ".");

		return result;
	}

}