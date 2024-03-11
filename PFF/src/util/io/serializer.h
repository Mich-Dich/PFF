#pragma once

#define TEST_NAME_CONVERTION(variable)		extract_vaiable_name(#variable)
#define SERIALIZE_KEY_VALUE(variable)		serialize_key_value(extract_vaiable_name(#variable), variable);

#define KEY_VALUE(var)						extract_vaiable_name(#var), var

namespace PFF::serializer {

	enum class option {

		save_to_file,
		load_from_file,
	};
	
	class PFF_API yaml {
	public:

		yaml(const std::string& filename, const std::string& section_name, option option);
		~yaml();
		
		DELETE_COPY_MOVE(yaml);

		// @brief This function adds or looks for a subsection with the specified section name in the YAML file.
		//          If the serializer option is set to save to file, it adds the subsection to the YAML content
		//          and executes the provided function within that subsection. If the serializer option is set to
		//          load from file, it looks for the subsection in the YAML content, deserializes its content,
		//          and executes the provided function within that subsection.
		// @param [section_name] The name of the subsection to be added or looked for.
		// @param [sub_section_function] The function to be executed within the subsection.
		//                               The function should accept a reference to a yaml object as its parameter.
		// @return A reference to the YAML object for chaining function calls.
		yaml& sub_section(const std::string& section_name, std::function<void(PFF::serializer::yaml&)> sub_section_function);

		// @brief This function is responsible for serializing or deserializing a single variable 
		//          to or from the YAML file based on the specified serialization option. If the 
		//          option is set to save to file, it converts the variable to its string 
		//          representation and writes it to the YAML file. If the option is set to load from
		//          file, it reads the variable's value from the YAML file and converts it back to 
		//          its original type. For vectors, it handles serialization and deserialization 
		//          of each element individually.
		// @param [key_name] The key name associated with the variable in the YAML file.
		// @param [value] Reference to the variable to be serialized or deserialized.
		// @return A reference to the YAML object for chaining function calls.
		template<typename T>
		yaml& entry(const std::string& key_name, T& value) {

			if (m_option == PFF::serializer::option::save_to_file) {

				std::string buffer{};
				if constexpr (PFF::util::is_vector<T>::value) {			// value is a vector

					//LOG(Info, "T is a std::vector.");

					m_file_content << add_spaces(m_level_of_indention) << m_prefix << key_name << ":\n";
					for (auto interation : value) {

						//LOG(Info, "interation: "<< interation)
						PFF::util::convert_to_string<T::value_type>(interation, buffer);
						m_file_content << add_spaces(m_level_of_indention + 1) << "- " << buffer << "\n";
					}

				} else {

					PFF::util::convert_to_string<T>(value, buffer);
					m_file_content << add_spaces(m_level_of_indention) << m_prefix << key_name << ": " << buffer << "\n";

					//LOG(Trace, "called: entry() to set: " << key_name << " - " << buffer);
				}

			} else {				// load from file

				if constexpr (PFF::util::is_vector<T>::value) {			// calue is a vector

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
							//LOG(Debug, "sub_section() found section => line: [" << line << "]");

							//     not end of content                     has correct indentaion                                 doesn't end in double-points              
							while (std::getline(m_file_content, line) && (measure_indentation(line) == 1) && (line.back() != ':')) {

								// remove indentation                       remove "- " (array element marker)
								line = line.substr(NUM_OF_INDENTING_SPACES + 2);
								PFF:util::convert_from_string(line, buffer);
								value.push_back(buffer);
							}

							//LOG(Debug, "END OF SUB-SECTION");
						}

						// skip rest of content if section found
						if (found_section)
							break;
					}

				} else {

					std::string buffer{};
					auto iterator = m_key_value_pares.find(key_name);
					if (iterator == m_key_value_pares.end())				// key is not in map
						return *this;

					buffer = iterator->second;
					util::convert_from_string(buffer, value);
				}
			}

			m_prefix = m_prefix_fallback;
			return *this;
		}
		

		template<typename T>
		yaml& vector_of_structs(const std::string& vector_name, std::vector<T>& vector, std::function<void(PFF::serializer::yaml&, const u64 iteration)> vector_function) {

			vector_func_index++;

			if (vector_func_index != 1) 
				m_level_of_indention++;

			if (m_option == PFF::serializer::option::save_to_file) {			// save to file

				const u32 indent_buffer = vector_func_index != 1 ? m_level_of_indention - 1 : m_level_of_indention;
				m_file_content << add_spaces(indent_buffer) << m_prefix << vector_name << ":\n";
				for (u64 x = 0; x < vector.size(); x++) {

					// start of array element
					m_prefix = "- ";
					m_prefix_fallback = "  ";
					vector_function(*this, x);
				}
		
				m_prefix = "";
				m_prefix_fallback = "";

			} else {		// load from file

				// buffer [m_key_value_pares] for duration of function
				std::unordered_map<std::string, std::string> key_value_pares_buffer = m_key_value_pares;
				std::vector<std::unordered_map<std::string, std::string>> vector_of_key_value_pares{};
				m_key_value_pares = {};

				// buffer [m_file_content] for duration of function
				std::stringstream file_content_buffer;
				std::vector<std::stringstream> vector_of_file_content{};		// for array element in file
				file_content_buffer << m_file_content.str();
				m_file_content = {};

				// deserialize content of subsections				
				u64 index = -1;
				bool found_section = false;
				const u32 section_indentation = 0;
				std::string line;
				while (std::getline(file_content_buffer, line)) {

					// skip empty lines or comments
					if (line.empty() || line.front() == '#')
						continue;

					// if line contains desired section enter inner-loop
					//   has correct indentaion              has correct vector_name                          ends with double-point
					if ((measure_indentation(line) == 0) && (line.find(vector_name) != std::string::npos) && (line.back() == ':')) {

						found_section = true;

						//     not end of content
						while (std::getline(file_content_buffer, line)) {

							if (line.front() == '-') {

								vector_of_key_value_pares.push_back({});
								vector_of_file_content.push_back({});
								index++;
							}

							// remove array-prefix "- " or "  "
							line = line.substr(NUM_OF_INDENTING_SPACES);

							//  more indented                                        beginning of new sub-section
							if (measure_indentation(line) != 0 || line.back() == ':' || line.front() == '-') {

								//m_file_content << line << "\n";
								vector_of_file_content[index] << line << "\n";
								continue;
							}

							line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
							std::istringstream iss(line);
							std::string key, value;
							std::getline(iss, key, ':');
							std::getline(iss, value);
							vector_of_key_value_pares[index][key] = value;
						}
					}
				}

				ASSERT(vector_of_key_value_pares.size() == vector_of_file_content.size(), "", "two buffers are of diffrent size");

				if (vector_of_key_value_pares.size() > 0) {

					vector.resize(vector_of_key_value_pares.size());
					for (u64 x = 0; x < vector.size(); x++) {

						m_key_value_pares = vector_of_key_value_pares[x];
						m_file_content = {};
						auto temp_buffer = vector_of_file_content[x].str();
						m_file_content << temp_buffer;
						vector_function(*this, x);
					}
				}

				// restore
				m_key_value_pares = key_value_pares_buffer;
				auto temp_buffer = file_content_buffer.str();
				m_file_content << temp_buffer;

			}

			if (vector_func_index != 1)
				m_level_of_indention--;

			vector_func_index--;
			return *this;
		}


	private:

		void serialize();
		yaml& deserialize();

		static const u32 NUM_OF_INDENTING_SPACES = 2;		// should not change

		std::string add_spaces(const u32 multiple_of_indenting_spaces);
		u32 measure_indentation(const std::string& str);

		u32 m_level_of_indention = 0;
		u64 vector_func_index = 0;
		std::string m_prefix{};				// can maybe be a [char*]
		std::string m_prefix_fallback{};	// can maybe be a [char*]

		// content data
		bool m_is_correct_struct = false;
		option m_option;
		std::string m_name{};
		std::stringstream m_file_content{};
		std::unordered_map<std::string, std::string> m_key_value_pares{};

		// file data
		std::filesystem::path m_filename{};
		std::ofstream m_ostream{};
		std::ifstream m_istream{};
	};

	/*
	class binary {

	}
	*/
}

namespace PFF {

	// @brief Searches for the last occurrence of the specified delimiter in the input string,
	//          and if found, extracts the substring after the delimiter into the 'dest' string.
	//          If the delimiter is not found, the 'dest' string remains unchanged.
	// @param [dest] Reference to a string where the extracted part will be stored.
	// @param [input] The input string from which the part is to be extracted.
	// @param [delimiter] The character delimiter used to identify the part to extract.
	// @return None
	PFF_API void extract_part_after_delimiter(std::string& dest, const std::string& input, const char* delimiter);

	// @brief Searches for the last occurrence of the specified delimiter in the input string,
	//          and if found, extracts the substring before the delimiter into the 'dest' string.
	//          If the delimiter is not found, the 'dest' string remains unchanged.
	// @param [dest] Reference to a string where the extracted part will be stored.
	// @param [input] The input string from which the part is to be extracted.
	// @param [delimiter] The character delimiter used to identify the part to extract.
	// @return None
	PFF_API void extract_part_befor_delimiter(std::string& dest, const std::string& input, const char* delimiter);

	// @brief Given a string representing a variable access chain (e.g., "object1->object2.variable"),
	//          this function extracts and returns the name of the variable ("variable" in this example).
	//          The extraction process considers both "->" and "." as delimiters for nested access.
	// @param [input] The input string representing the variable access chain.
	// @return A string containing the name of the variable extracted from the input string.
	PFF_API std::string extract_vaiable_name(const std::string& input);

}
