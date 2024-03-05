#pragma once

namespace PFF::serializer {

	enum class serializer_option {

		save_to_file,
		load_from_file,
	};

	class yaml {
	public:

		yaml(const std::string& path, const std::string& filename, serializer_option option);
		~yaml();

		void serialize_struct_begining(const std::string& name);

		template<typename T>
		void serialize_key_value(const std::string& key, T& value) {

			CORE_ASSERT(m_ostream.is_open(), "", "output-file-stream is not open");

			std::string buffer{};
			util::convert_to_string(buffer, value);
			m_ostream << "  " << key << ": " << buffer << "\n";
		}

	private:
		std::filesystem::path m_path{};
		std::filesystem::path m_filename{};
		std::filesystem::path m_filepath{};

		serializer_option m_option{};
		std::ofstream m_ostream{};
		std::ifstream m_istream{};
	};


	namespace binary {

	}
}
