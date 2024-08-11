#pragma once

#include "serializer_data.h"

namespace PFF::serializer {

	class PFF_API binary {
	public:

		PFF_DELETE_COPY_MOVE_CONSTRUCTOR(binary);

		binary(const std::filesystem::path filename, const std::string& section_name, option option);
		~binary();

		template<typename T>
		binary& entry(T& value) {

			if (m_option == option::save_to_file)
				m_ostream.write(reinterpret_cast<const char*>(&value), sizeof(T));
			else
				m_istream.read(reinterpret_cast<char*>(&value), sizeof(T));

			return *this;
		}


		template<typename T>
		binary& vector(std::vector<T>& vector) {

			if (m_option == option::save_to_file) {

				size_t size = vector.size();
				m_ostream.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
				m_ostream.write(reinterpret_cast<const char*>(vector.data()), sizeof(T) * size);

			} else {

				size_t vector_size = 0;
				m_istream.read(reinterpret_cast<char*>(&vector_size), sizeof(size_t));
				vector.resize(vector_size);
				m_istream.read(reinterpret_cast<char*>(vector.data()), sizeof(T) * vector_size);
			}

			return *this;
		}


		template<typename T>
		binary& vector(std::vector<T>& vector, std::function<void(PFF::serializer::yaml&, const u64 iteration)> vector_function) {

			// TODO: implement

			return *this;
		}



	private:

		std::string m_name{};
		std::filesystem::path m_filename{};
		std::ofstream m_ostream{};
		std::ifstream m_istream{};
		option m_option;

	};

}
