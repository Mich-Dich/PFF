#pragma once

#include "util/util.h"

namespace PFF {

	namespace io_handler {
	
		std::vector<char> read_file(const std::string& file_path);

		bool write_file(const std::string& file_path, std::vector<char> content);

	}
}
