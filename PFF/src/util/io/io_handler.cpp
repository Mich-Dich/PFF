
#include "util/pffpch.h"

#include <filesystem>
#include "io_handler.h"

namespace PFF::io_handler {

	//
	bool read_file(const std::string& file_path, std::vector<char>& content_buffer) {

		std::ifstream file{ file_path, std::ios::ate | std::ios::binary };
		CORE_VALIDATE(file.is_open(), return false, "", "Failed to open file at: [" << file_path << "]");

		size_t file_size = static_cast<size_t>(file.tellg());
		content_buffer.clear();
		content_buffer.resize(file_size);

		file.seekg(0);
		file.read(content_buffer.data(), file_size);
		file.close();

		CORE_LOG(Trace, "loaded file at [" << file_path << "] with length[" << file_size << "]");
		return true;
	}

	//
	bool write_file(const std::string& file_path, const std::vector<char>& content_buffer) {

		std::ofstream file{ file_path, std::ios::binary };
		CORE_VALIDATE(file.is_open(), return false, "", "Failed to open file for writing at: " << file_path);

		file.write(content_buffer.data(), content_buffer.size());
		file.close();

		CORE_LOG(Trace, "Wrote content to file at [" << file_path << "] with length [" << content_buffer.size() << "]");
		return true;
	}

	// 
	bool create_directory(const std::filesystem::path& path) {

		// Check if the directory exists
		if (!std::filesystem::is_directory(path))
			CORE_VALIDATE(std::filesystem::create_directories(path), return false, "", "could not create directory: " << path);
		
		return true;
	}

	bool is_directory(const std::filesystem::path& path)								{ return std::filesystem::is_directory(path); }

	bool is_file(const std::filesystem::path& path)										{ return std::filesystem::is_regular_file(path); }

	bool is_hidden(const std::filesystem::path& path)									{ return path.filename().string()[0] == '.'; }

	const std::filesystem::path get_absolute_path(const std::filesystem::path& path)	{ return std::filesystem::absolute(path); }

	std::vector<std::filesystem::path> get_files_in_dir(const std::filesystem::path& path) {

		std::vector<std::filesystem::path> files;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (is_file(entry.path())) {
				files.push_back(entry.path());
			}
		}
		return files;
	}

	std::vector<std::filesystem::path> get_folders_in_dir(const std::filesystem::path& path) {
		std::vector<std::filesystem::path> folders;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (std::filesystem::is_directory(entry.path()))
				folders.push_back(entry.path());
		}
		return folders;
	}

	bool write_to_file(const char* data, const std::filesystem::path& filename) {

		std::ofstream outStream(filename.string());
		CORE_VALIDATE(outStream.is_open(), return false, "", "could not open " << filename);
		outStream << data;
		outStream.close();
		return true;
	}

}
