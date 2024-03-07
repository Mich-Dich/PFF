
#include "util/pffpch.h"

#include "serializer.h"

namespace PFF::serializer {

	// ================================================== yaml ==================================================

	yaml::yaml(const std::string& filename, const std::string& section_name, option option)
		: m_filename(filename), m_name(section_name), m_option(option) {

		LOG(Info, "called yaml() constructor");
		extract_part_befor_delimiter(m_path, filename, "/");

		CORE_ASSERT(io_handler::create_directory(m_path), "", "Could not create file-path");

		if (m_option == option::load_from_file) {

			deserialize();
		} else {

			m_content_buffer << section_name << ":\n";
			m_level_of_indention = 1;
		}

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

	yaml::~yaml() {

		LOG(Info, "called yaml() destructor");

		if (m_option == option::save_to_file)
			serialize();
	}

	/*
	yaml& yaml::section_name(const std::string& name) {

		m_name = name;

		LOG(Debug, "called yaml::section_name()");

		if (m_option == serializer::serializer_option::load_from_file) {

			m_is_correct_struct = (m_name == name);
		}

		return *this;
	}
	*/

	void yaml::serialize() {

		LOG(Info, "called yaml::serialize()");

		m_ostream = std::ofstream(m_filename);
		CORE_ASSERT(m_ostream.is_open(), "", "file-stream is not open");

		// make new stream to buffer updated file
		std::ostringstream updated_file;

		// todo::write to corrent place (copy beginning && end)
		m_ostream << m_content_buffer.str();

		/*
		for (auto it = m_key_value_pares.begin(); it != m_key_value_pares.end(); it++) {

			m_ostream << "  " << it->first << ": " << it->second << "\n";
			LOG(Info, "  " << it->first << ": " << it->second);
		}
		*/
	}

	yaml& yaml::deserialize() {

		CORE_ASSERT(!m_name.empty(), "called yaml::deserialize()", "name of section to find is empty");

		m_istream = std::ifstream(m_filename);
		CORE_ASSERT(m_istream.is_open(), "", "file-stream is not open");

		const u32 SECTION_INDENTATION = 0;
		bool found_section = false;
		std::string line;
		while (std::getline(m_istream, line)) {

			// skip empty lines or comments
			if (line.empty() || line.front() == '#')
				continue;
			
			// if line contains desired section enter inner-loop
			if (line.find(m_name + ":") != std::string::npos && measure_indentation(line) == 0) {

				found_section = true;

				//     not end of file                   line has more leading spaces
				while (std::getline(m_istream, line) && (measure_indentation(line) > SECTION_INDENTATION)) {

					line = line.substr(NUM_OF_INDENTING_SPACES);

					//  more indented                                          beginning of sub-section
					if ((measure_indentation(line) != SECTION_INDENTATION) || (line.back() == ':')) {

						m_file_content << line << "\n";
						continue;
					}

					line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
					std::istringstream iss(line);
					std::string key, value;
					std::getline(iss, key, ':');
					std::getline(iss, value);
					m_key_value_pares[key] = value;
				}

				LOG(Debug, "END OF SECTION");
			}

			// exit outer loop if inner-loop already done
			if (found_section)
				break;

		}

		LOG(Trace, "Filished parting file")
		return *this;
	}

	std::string yaml::add_spaces(const u32 multiple_of_indenting_spaces) {
		
		if (multiple_of_indenting_spaces == 0)
			return "";

		return std::string(multiple_of_indenting_spaces * NUM_OF_INDENTING_SPACES, ' ');
	}

	u32 yaml::measure_indentation(const std::string& str) {

		u32 count = 0;
		for (char ch : str) {
			if (ch == ' ')
				count++;
			else
				break; // Stop counting when a non-space character is encountered			
		}

		return count / NUM_OF_INDENTING_SPACES;
	}

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