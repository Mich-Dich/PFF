#include "io_handler.h"

namespace PFF {

	namespace io_handler {

		//
		std::vector<char> read_file(const std::string& file_path) {

			std::ifstream file{ file_path, std::ios::ate | std::ios::binary };

			CORE_ASSERT(file.is_open(), "", "Failed to open file at: " << file_path);

			size_t file_size = static_cast<size_t>(file.tellg());
			std::vector<char> buffer(file_size);

			file.seekg(0);
			file.read(buffer.data(), file_size);
			file.close();

			CORE_LOG(Trace, "loaded file at [" << file_path << "] with length[" << file_size << "]");

			return buffer;
		}

		//
		bool write_file(const std::string& file_path, std::vector<char> content) {


			return false;
		}
	}
}
