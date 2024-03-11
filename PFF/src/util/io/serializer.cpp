
#include "util/pffpch.h"

#include "serializer.h"

namespace PFF::serializer {

	// ================================================== yaml ==================================================

	yaml::yaml(const std::string& filename, const std::string& section_name, option option)
		: m_filename(filename), m_name(section_name), m_option(option) {

		std::string path{};
		extract_part_befor_delimiter(path, filename, "/");
		CORE_ASSERT(io_handler::create_directory(path), "", "Could not create file-path");

		// make shure the file exists
		if (!std::filesystem::exists(m_filename)) {

			auto file = std::ofstream(m_filename);
			file.close();
		}

		if (m_option == option::load_from_file)
			deserialize();
		
		else {

			m_file_content << section_name << ":\n";
			m_level_of_indention = 1;
		}

	}

	yaml::~yaml() {

		if (m_option == option::save_to_file)
			serialize();
	}

	void yaml::serialize() {

		m_ostream = std::ofstream(m_filename);
		CORE_ASSERT(m_ostream.is_open(), "", "file-stream is not open");

		// make new stream to buffer updated file
		std::ostringstream updated_file;

		// todo::write to corrent place (copy beginning && end)
		m_ostream << m_file_content.str();

	}

	yaml& yaml::deserialize() {

		CORE_ASSERT(!m_name.empty(), "", "name of section to find is empty");

		m_istream = std::ifstream(m_filename);
		CORE_VALIDATE(m_istream.is_open(), return *this,"", "file-stream is not open");

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

					//  more indented                                         is sub-section        is array-element
					if ((measure_indentation(line) != SECTION_INDENTATION) || line.back() == ':' || line.front() == '-') {

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

				//LOG(Debug, "END OF SECTION");
			}

			// exit outer loop if inner-loop already done
			if (found_section)
				break;

		}

		//LOG(Trace, "Filished parting file")
		return *this;
	}

	yaml& yaml::sub_section(const std::string& section_name, std::function<void(PFF::serializer::yaml&)> sub_section_function) {

		m_level_of_indention++;

		if (m_option == PFF::serializer::option::save_to_file) {

			m_file_content << add_spaces(m_level_of_indention - 1) << section_name << ":\n";
			sub_section_function(*this);

		} else {	// load from file

			// buffer [m_key_value_pares] for duration of function
			std::unordered_map<std::string, std::string> key_value_pares_buffer = m_key_value_pares;
			m_key_value_pares = {};

			// buffer [m_file_content] for duration of function
			std::stringstream file_content_buffer;
			file_content_buffer << m_file_content.str();
			m_file_content = {};

			// deserialize content of subsections				
			const u32 section_indentation = 0;
			bool found_section = false;
			std::string line;
			//m_level_of_indention++;
			while (std::getline(file_content_buffer, line)) {

				// skip empty lines or comments
				if (line.empty() || line.front() == '#')
					continue;

				// if line contains desired section enter inner-loop
				//   has correct indentaion              has correct section_name                          ends with double-point
				if ((measure_indentation(line) == 0) && (line.find(section_name) != std::string::npos) && (line.back() == ':')) {

					found_section = true;
					//m_level_of_indention++;
					//LOG(Debug, "sub_section() found section => line: [" << line << "]");

					//     not end of content
					while (std::getline(file_content_buffer, line)) {

						line = line.substr(NUM_OF_INDENTING_SPACES);

						//  more indented                                        beginning of new sub-section
						if (measure_indentation(line) != 0 || line.back() == ':') {

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

					//LOG(Debug, "END OF SUB-SECTION");
				}

				// skip rest of content if section found
				if (found_section)
					break;
			}
			//m_level_of_indention--;

			sub_section_function(*this);

			// restore [m_key_value_pares]
			//LOG(Fatal, "Reseting values");
			m_key_value_pares = key_value_pares_buffer;
			m_file_content << file_content_buffer.str();
		}

		m_level_of_indention--;
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
				break; // Stop counting on non-space characters
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