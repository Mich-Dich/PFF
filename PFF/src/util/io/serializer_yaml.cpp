
#include "util/pffpch.h"

#include "util/io/io.h"

#include "serializer_yaml.h"

namespace PFF::serializer {

	// ================================================== yaml ==================================================

	yaml::yaml(const std::filesystem::path filename, const std::string& section_name, option option)
		: m_filename(filename), m_name(section_name), m_option(option) {

		std::filesystem::path path = filename.parent_path();
		ASSERT(io::create_directory(path), "", "Could not create file-path");

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

		auto istream = std::ifstream(m_filename);
		ASSERT(istream.is_open(), "", "input-file-stream is not open");

		// make new stream to buffer updated file
		std::ostringstream updated_file;

		// copy content of file that is not the focus of this serialization
		bool found = false;				// ensure only one section can be skipped
		std::string line = "";
		while (std::getline(istream, line)) {

			// is correct section
			if (!found && (line.find(m_name + ":") != std::string::npos) && (util::measure_indentation(line, NUM_OF_INDENTING_SPACES) == 0)) {

				found = true;

				// override section with new content
				updated_file << m_file_content.str();

				// SKIP CONTENT
				while (std::getline(istream, line)) {

					if (line.back() == ':' && util::measure_indentation(line, NUM_OF_INDENTING_SPACES) == 0) {	// still in section ??

						updated_file << line + "\n";
						break;
					}
				}
			}

			else
				updated_file << line + "\n";
		}

		// apend if section not found
		if (!found)
			updated_file << m_file_content.str();

		istream.close();

		auto ostream = std::ofstream(m_filename);
		ASSERT(ostream.is_open(), "", "output-file-stream is not open");

		ostream << updated_file.str();
		ostream.close();
	}

	yaml& yaml::deserialize() {

		ASSERT(!m_name.empty(), "", "name of section to find is empty");

		m_istream = std::ifstream(m_filename);
		VALIDATE(m_istream.is_open(), return *this, "", "file-stream is not open");

		const u32 SECTION_INDENTATION = 0;
		bool found_section = false;
		std::string line;
		while (std::getline(m_istream, line)) {

			// skip empty lines or comments
			if (line.empty() || line.front() == '#')
				continue;

			// if line contains desired section enter inner-loop
			if (line.find(m_name + ":") != std::string::npos && util::measure_indentation(line, NUM_OF_INDENTING_SPACES) == 0) {

				found_section = true;

				//     not end of file                   line has more leading spaces
				while (std::getline(m_istream, line) && (util::measure_indentation(line, NUM_OF_INDENTING_SPACES) > SECTION_INDENTATION)) {

					line = line.substr(NUM_OF_INDENTING_SPACES);

					//  more indented                                         is sub-section        is array-element
					if ((util::measure_indentation(line, NUM_OF_INDENTING_SPACES) > SECTION_INDENTATION) || line.back() == ':' || line.front() == '-') {

						m_file_content << line << '\n';
						continue;
					}

					std::string key, value;
					extract_key_value(key, value, line);
					m_key_value_pares[key] = value;
				}
			}

			// exit outer loop if inner-loop already done
			if (found_section)
				break;

		}
		return *this;
	}

	void yaml::extract_key_value(std::string& key, std::string& value, std::string& line) {

		std::istringstream iss(line);
		std::getline(iss, key, ':');
		std::getline(iss, value);

		if (const u32 indentation = util::measure_indentation(key, 1); indentation > 0)
			key = key.substr(indentation);

		if (!value.empty() && value.front() == ' ')
			value.erase(0, 1);
	}

	yaml& yaml::sub_section(const std::string& section_name, std::function<void(PFF::serializer::yaml&)> sub_section_function) {

		m_level_of_indention++;

		if (m_option == PFF::serializer::option::save_to_file) {

			m_file_content << util::add_spaces(m_level_of_indention + static_cast<u32>(vector_func_index -1), NUM_OF_INDENTING_SPACES) << section_name << ":\n";
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
				//   has incorrect indentaion											ends NOT with double-point
				if ((util::measure_indentation(line, NUM_OF_INDENTING_SPACES) != 0) || (line.back() != ':'))
					continue;

				// remove leading and trailing whitespace
				auto trimmed = line;
				trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), [](unsigned char ch) {
					return !std::isspace(ch);
				}));
				trimmed.erase(std::find_if(trimmed.rbegin(), trimmed.rend(), [](unsigned char ch) {
					return !std::isspace(ch);
				}).base(), trimmed.end());

				// Remove the trailing colon
				if (!trimmed.empty() && trimmed.back() == ':')
					trimmed.pop_back();

				if (trimmed != section_name)		// has incorrect section_name
					continue;

				found_section = true;

				while (std::getline(file_content_buffer, line)) {

					if (util::measure_indentation(line, NUM_OF_INDENTING_SPACES) < m_level_of_indention)	// exit inner loop after section is finished
						break;		

					line = line.substr(NUM_OF_INDENTING_SPACES);

					//  more indented																		beginning of new sub-section
					if (util::measure_indentation(line, NUM_OF_INDENTING_SPACES) > m_level_of_indention -1 || line.back() == ':' || line.front() == '-') {

						m_file_content << line << "\n";
						continue;
					}

					std::string key, value;
					extract_key_value(key, value, line);
					m_key_value_pares[key] = value;
				}

				if (found_section)
					break;
			}

			if (found_section)
				sub_section_function(*this);

			m_key_value_pares = key_value_pares_buffer;
			m_file_content << file_content_buffer.str();
		}

		m_level_of_indention--;
		return *this;
	}

}
