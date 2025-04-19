#pragma once

#include "serializer_data.h"

namespace PFF::serializer {

	class binary {
	public:

		PFF_DELETE_COPY_MOVE_CONSTRUCTOR(binary);

		PFF_DEFAULT_GETTER(option, option);

		binary(const std::filesystem::path filename, const std::string& section_name, option option);
		~binary();

		template<typename T>
		binary& entry(T& value) {

			if (m_option == option::save_to_file) {

				if constexpr (std::is_same_v<T, std::filesystem::path>) {

					std::string path_str = value.generic_string();
					entry<std::string>(path_str);

				} else if constexpr (std::is_same_v<T, std::string>) {

					size_t length = value.size();
					m_ostream.write(reinterpret_cast<const char*>(&length), sizeof(length));
					m_ostream.write(reinterpret_cast<const char*>(value.data()), length);

				} else
					m_ostream.write(reinterpret_cast<const char*>(&value), sizeof(T));

			} else {

				if constexpr (std::is_same_v<T, std::filesystem::path>) {

					std::string buffer;
					entry<std::string>(buffer);
					value = std::filesystem::path(buffer);

				} else if constexpr (std::is_same_v<T, std::string>) {

					size_t length = 0;
					m_istream.read(reinterpret_cast<char*>(&length), sizeof(length));
					
					ASSERT(length < 65565, "", "Corrupted path length")

					value.resize(length);
					m_istream.read(value.data(), length);

				} else
					m_istream.read(reinterpret_cast<char*>(&value), sizeof(T));
			}

			return *this;
		}


		template<typename T>
		binary& entry(std::vector<T>& vector) {

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


		// When used with option [load_from_file] will create a new array with "new" -> called has ownership
		template<typename T>
		binary& array(T*& array_start, size_t array_size) {

			const size_t total_bytes = sizeof(T) * array_size;
			if (m_option == option::save_to_file) {
				m_ostream.write(reinterpret_cast<const char*>(array_start), total_bytes);
			} else {

				array_start = (T*)malloc(total_bytes);
				LOG(Trace, "Deserializing [" << total_bytes << "] bytes into [" << array_start << "]")
				m_istream.read(reinterpret_cast<char*>(array_start), total_bytes);
			}

			return *this;
		}


		template<typename T>
		binary& vector(std::vector<T>& vector, std::function<void(PFF::serializer::binary&, const u64 iteration)> vector_function) {

			LOG(Error, "NOT IMPLEMENTED YET");
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
