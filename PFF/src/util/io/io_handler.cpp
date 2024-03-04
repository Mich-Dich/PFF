
#include "util/pffpch.h"

#include <filesystem>
#include "io_handler.h"

namespace PFF::io_handler {

	//
	bool read_file(const std::string& file_path, std::vector<char>& content) {

		std::ifstream file{ file_path, std::ios::ate | std::ios::binary };
		CORE_VALIDATE(file.is_open(), return false, "", "Failed to open file at: " << file_path);

		size_t file_size = static_cast<size_t>(file.tellg());
		content.clear();
		content.resize(file_size);

		file.seekg(0);
		file.read(content.data(), file_size);
		file.close();

		CORE_LOG(Trace, "loaded file at [" << file_path << "] with length[" << file_size << "]");
		return true;
	}

	//
	bool write_file(const std::string& file_path, const std::vector<char>& content) {

		std::ofstream file{ file_path, std::ios::binary };
		CORE_VALIDATE(file.is_open(), return false, "", "Failed to open file for writing at: " << file_path);

		file.write(content.data(), content.size());
		file.close();

		CORE_LOG(Trace, "Wrote content to file at [" << file_path << "] with length [" << content.size() << "]");
		return true;
	}

	// 
	bool create_directory(const std::string& path) {

		// Check if the directory exists
		if (!(std::filesystem::exists(path) && std::filesystem::is_directory(path))) {
			
			if (!std::filesystem::create_directory(path))		// Create the directory
				return false;

		}
		return true;
	}

}
