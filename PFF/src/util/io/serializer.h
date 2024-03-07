#pragma once

#define TEST_NAME_CONVERTION(variable)		extract_vaiable_name(#variable)
#define SERIALIZE_KEY_VALUE(variable)		serialize_key_value(extract_vaiable_name(#variable), variable);

#define KEY_VALUE(var)						extract_vaiable_name(#var), var

#include <type_traits> // For std::is_vector

template<typename T>
struct is_vector : std::false_type {};

template<typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type {};


namespace PFF::serializer {

	enum class option {

		save_to_file,
		load_from_file,
	};
	
	class PFF_API yaml {
	public:

		yaml(const std::string& filename, const std::string& section_name, option option);
		~yaml();
		
		//DELETE_COPY_MOVE(yaml);

		// =================== NEW API ===================


		void serialize();
		yaml& deserialize();

		template<typename T>
		yaml& set_vector(const std::string& name, T& value) {

			// NOT FINISHED

			return *this;
		}

		yaml& vector_of_structs(const std::string& vector_name, const u32 vector_size, std::function<void (PFF::serializer::yaml&)> vector_function) {

			m_content_buffer << add_spaces(m_level_of_indention) << vector_name << ":\n";
			m_level_of_indention++;

			if (m_option == PFF::serializer::option::save_to_file) {

				for (u32 x = 0; x < vector_size; x++) {

					vector_function(*this);
				}
			}
			m_level_of_indention--;
			return *this;
		}

		// adds or looks for a subsection in the yaml file
		yaml& sub_section(const std::string& section_name, std::function<void (PFF::serializer::yaml&)> sub_section_function) {

			m_level_of_indention++;
			
			if (m_option == PFF::serializer::option::save_to_file) {

				m_content_buffer << add_spaces(m_level_of_indention - 1) << section_name << ":\n";
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
					//   has correct indentaion                                 has correct section_name                          ends with double-point
					if ((measure_indentation(line) == 0) && (line.find(section_name) != std::string::npos) && (line.back() == ':')) {

						found_section = true;
						//m_level_of_indention++;
						LOG(Debug, "sub_section() found section => line: [" << line << "]");

						//     not end of content
						while (std::getline(file_content_buffer, line) ) {

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

						LOG(Debug, "END OF SUB-SECTION");
					}

					// skip rest of content if section found
					if (found_section)
						break;
				}
				//m_level_of_indention--;

				sub_section_function(*this);

				// restore [m_key_value_pares]
				LOG(Fatal, "Reseting values");
				m_key_value_pares = key_value_pares_buffer;
				m_file_content << file_content_buffer.str();
			}

			m_level_of_indention--;
			return *this;
		}

		// handels convertion for one single variable from and to the yaml file
		template<typename T>
		yaml& entry(const std::string& key_name, T& value) {

			if (m_option == PFF::serializer::option::save_to_file) {

				std::string buffer{};
				if constexpr (is_vector<T>::value) {			// value is a vector

					LOG(Info, "T is a std::vector.");

					m_content_buffer << add_spaces(m_level_of_indention) << key_name << ":\n";
					m_level_of_indention++;
					for (auto interation : value) {

						LOG(Info, "interation: "<< interation)
						PFF::util::convert_to_string<T::value_type>(interation, buffer);
						m_content_buffer << add_spaces(m_level_of_indention) << "- " << buffer << "\n";
					}
					m_level_of_indention--;

				} else {

					PFF::util::convert_to_string<T>(value, buffer);
					m_content_buffer << add_spaces(m_level_of_indention) << key_name << ": " << buffer <<"\n";
					//m_key_value_pares.insert({ add_spaces(m_level_of_indention) + key_name, buffer });

					LOG(Trace, "called: entry() to set: " << key_name << " - " << buffer);
				}

			} else {				// load from file

				if constexpr (is_vector<T>::value) {			// calue is a vector

					// deserialize content of subsections				
					typename T::value_type buffer{};
					u32 section_indentation = 0;
					bool found_section = false;
					std::string line;
					while (std::getline(m_file_content, line)) {

						// skip empty lines or comments
						if (line.empty() || line.front() == '#')
							continue;

						// if line contains desired section enter inner-loop
						//   has correct indentaion                                 has correct section_name                      ends with double-point
						if ((measure_indentation(line) == 0) && (line.find(key_name) != std::string::npos) && (line.back() == ':')) {

							found_section = true;
							LOG(Debug, "sub_section() found section => line: [" << line << "]");

							//     not end of content                     has correct indentaion                                 doesn't end in double-points              
							while (std::getline(m_file_content, line) && (measure_indentation(line) == 1) && (line.back() != ':')) {

								// remove indentation                       remove "- " (array element marker)
								line = line.substr(NUM_OF_INDENTING_SPACES + 2);
								PFF:util::convert_from_string(line, buffer);
								value.push_back(buffer);
							}

							LOG(Debug, "END OF SUB-SECTION");
						}

						// skip rest of content if section found
						if (found_section)
							break;
					}


				} else {

					// CORE_ASSERT(m_is_correct_struct, "", "Current struct is not correct");

					std::string buffer = m_key_value_pares[key_name];
					util::convert_from_string(buffer, value);
					LOG(Trace, "called: entry() to get: " << key_name << " " << value);

				}

			}

			return *this;
		}

	private:

		static const u32 NUM_OF_INDENTING_SPACES = 2;		// should not change

		std::string add_spaces(const u32 multiple_of_indenting_spaces);
		u32 measure_indentation(const std::string& str);

		u32 m_level_of_indention = 0;
		bool m_is_correct_struct = false;

		std::stringstream m_content_buffer{};

		std::stringstream m_file_content{};
		std::string m_name{};
		std::unordered_map<std::string, std::string> m_key_value_pares{};

		std::string m_path{};
		std::filesystem::path m_filename{};
		option m_option;
		std::ofstream m_ostream{};
		std::ifstream m_istream{};
	};

	/*
	class binary {

	}
	*/
}

namespace PFF {

	PFF_API void extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter);

	PFF_API void extract_part_befor_delimiter(std::string& dest, const std::string& input, const char* delimiter);

	PFF_API std::string extract_vaiable_name(const std::string& input);

}

/*
				
*/