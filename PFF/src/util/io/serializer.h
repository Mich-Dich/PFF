#pragma once

#define TEST_NAME_CONVERTION(variable)		extract_vaiable_name(#variable)
#define SERIALIZE_KEY_VALUE(variable)		serialize_key_value(extract_vaiable_name(#variable), variable);

#define KEY_VALUE(var)						extract_vaiable_name(#var), var

namespace PFF::serializer {

	enum class serializer_option {

		save_to_file,
		load_from_file,
	};
	
	class yaml {
	public:

		yaml(const std::string& filename, serializer_option option);
		~yaml();
				
		// =================== NEW API ===================

		yaml& struct_name(const std::string& name);

		void serialize();

		yaml& deserialize();
		
		template<typename T>
		yaml& set(const std::string& name, T& value) {

			std::string buffer{};
			PFF::util::convert_to_string<T>(value, buffer);
			m_key_value_pares.insert({ name, buffer });

			LOG(Trace, "called: yaml& add(const std::string& name, T& value) with: " << name << " - " << buffer);
			return *this;
		}

		template<typename T>
		yaml& set_vector(const std::string& name, T& value) {

			// NOT FINISHED

			return *this;
		}

		template<typename T>
		yaml& get(const std::string& name, T& value) {

			CORE_ASSERT(m_is_correct_struct, "", "Current struct is not correct");

			std::string buffer = m_key_value_pares[name];
			util::convert_from_string(buffer, value);
			LOG(Trace, "called: yaml& get() => " << name << " " << value);

			return *this;
		}



	private:

		bool m_is_correct_struct = false;

		std::string m_name{};
		std::unordered_map<std::string, std::string> m_key_value_pares;

		std::string m_path{};
		std::filesystem::path m_filename{};

		serializer_option m_option{};
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