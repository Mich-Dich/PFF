#pragma once

#include "util/util.h"

namespace PFF::io_handler {

	// Reads the content of a file into a vector of characters.
	// @param [file_path] The path to the file to be read.
	// @param [content] Reference to a vector that will store the content of the file.
	// @return [bool] true if the file is successfully read, false otherwise.
	PFF_API bool read_file(const std::string& file_path, std::vector<char>& content);
		
	// Writes [content] to a file, overriding the previous content
	// @param [file_path] The path to the file to be written.
	// @param [content] The vector of characters to be written to the file.
	// @return [bool] true if the file is successfully written, false otherwise.
	PFF_API bool write_file(const std::string& file_path, const std::vector<char>& content);

	// @brief Creates a directory at the specified path if it doesn't already exist.
	// @param [path] The path to the directory to be created.
	// @return [bool] Returns true if the directory is successfully created or already exists; false otherwise.
	bool create_directory(const std::string& path);
	
}
