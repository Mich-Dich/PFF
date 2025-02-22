
#include "util/pffpch.h"

#ifdef PFF_PLATFORM_WINDOWS

	#include <Windows.h>
	#include <Psapi.h>
	#include <TlHelp32.h>

#elif defined(PFF_PLATFORM_LINUX)

	#include <dirent.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>

#else
	#error undefined platform
#endif

#include "io.h"

namespace PFF::io {

	//
	bool read_file(const std::string& file_path, std::vector<char>& content_buffer) {

		std::ifstream file{ file_path, std::ios::ate | std::ios::binary };
		VALIDATE(file.is_open(), return false, "", "Failed to open file at: [" << file_path << "]");

		size_t file_size = static_cast<size_t>(file.tellg());
		content_buffer.clear();
		content_buffer.resize(file_size);

		file.seekg(0);
		file.read(content_buffer.data(), file_size);
		file.close();

		LOG(Trace, "loaded file at [" << file_path << "] with length[" << file_size << "]");
		return true;
	}

	//
	bool write_file(const std::string& file_path, const std::vector<char>& content_buffer) {

		std::ofstream file{ file_path, std::ios::binary };
		VALIDATE(file.is_open(), return false, "", "Failed to open file for writing at: " << file_path);

		file.write(content_buffer.data(), content_buffer.size());
		file.close();

		LOG(Trace, "Wrote content to file at [" << file_path << "] with length [" << content_buffer.size() << "]");
		return true;
	}

	//
	bool copy_file(const std::filesystem::path& full_path_to_file, const std::filesystem::path& target_directory) {

		try {
		
			VALIDATE(std::filesystem::exists(full_path_to_file), return false, "", "Source file does not exist: " << full_path_to_file);

			// Check if the target directory exists, if not, create it
			if (!std::filesystem::exists(target_directory)) {
				if (io::create_directory(target_directory))
					return false;
			}

			std::filesystem::path target_file_path = target_directory / full_path_to_file.filename();
			std::filesystem::copy_file(full_path_to_file, target_file_path, std::filesystem::copy_options::overwrite_existing);
			return true;

		} catch (const std::filesystem::filesystem_error& e) {

			LOG(Error, "Filesystem error: " << e.what());
			return false;
		} catch (const std::exception& e) {

			LOG(Error, "Error copying file: " << e.what());
			return false;
		}
	}

	// 
	bool create_directory(const std::filesystem::path& path) {

		// Check if the directory exists
		if (!std::filesystem::is_directory(path))
			VALIDATE(std::filesystem::create_directories(path), return false, "", "Failed to create directory: " << path);
		
		return true;
	}

	std::vector<std::string> get_processes_using_file(const std::wstring& filePath) {

#ifdef PFF_PLATFORM_WINDOWS

		std::vector<std::string> processNames;
		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hSnapshot == INVALID_HANDLE_VALUE)
			return processNames;

		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hSnapshot, &pe32)) {
			CloseHandle(hSnapshot);
			return processNames;
		}

		do {
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
			if (hProcess == NULL)
				continue;

			HMODULE hMods[1024];
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded)) {
				for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {

					TCHAR szModName[MAX_PATH];
					if (!GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR)))
						continue;

					if (_wcsicmp(szModName, filePath.c_str()) != 0)
						continue;

					std::wstring buffer = pe32.szExeFile;
					int size_needed = WideCharToMultiByte(CP_UTF8, 0, &buffer[0], (int)buffer.size(), NULL, 0, NULL, NULL);
					std::string str(size_needed, 0);
					WideCharToMultiByte(CP_UTF8, 0, &buffer[0], (int)buffer.size(), &str[0], size_needed, NULL, NULL);
					processNames.push_back(str);
					break;
				}
			}
			CloseHandle(hProcess);
		} while (Process32Next(hSnapshot, &pe32));

		CloseHandle(hSnapshot);
		return processNames;

#elif defined(PFF_PLATFORM_LINUX)

	std::vector<std::string> processNames;
    std::string filePathStr(filePath.begin(), filePath.end()); // Convert wstring to string

    // Iterate through the /proc directory
    DIR* procDir = opendir("/proc");
    if (procDir == nullptr) {
        perror("opendir");
        return processNames;
    }

    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {
        // Check if the entry is a number (i.e., a PID)
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            std::string pidDir = "/proc/" + std::string(entry->d_name) + "/fd/";
            DIR* fdDir = opendir(pidDir.c_str());
            if (fdDir) {
                struct dirent* fdEntry;
                while ((fdEntry = readdir(fdDir)) != nullptr) {
                    if (fdEntry->d_type == DT_LNK) {
                        // Construct the full path to the file descriptor
                        std::string linkPath = pidDir + fdEntry->d_name;
                        char targetPath[PATH_MAX];
                        ssize_t len = readlink(linkPath.c_str(), targetPath, sizeof(targetPath) - 1);
                        if (len != -1) {
                            targetPath[len] = '\0'; // Null-terminate the string
                            // Compare the target path with the file path
                            if (filePathStr == targetPath) {
                                // If it matches, get the process name
                                std::string processName = entry->d_name; // PID
                                std::string cmdlinePath = "/proc/" + processName + "/comm";
                                std::ifstream cmdlineFile(cmdlinePath);
                                if (cmdlineFile.is_open()) {
                                    std::string name;
                                    std::getline(cmdlineFile, name);
                                    processNames.push_back(name);
                                }
                                break; // No need to check other fds for this process
                            }
                        }
                    }
                }
                closedir(fdDir);
            }
        }
    }
    closedir(procDir);
    return processNames;

#elif  defined(PFF_PLATFORM_MAC)
	#error undefined platform
#endif

	}

	bool is_directory(const std::filesystem::path& path)								{ return std::filesystem::is_directory(path); }

	bool is_file(const std::filesystem::path& path)										{ return std::filesystem::is_regular_file(path); }

	bool is_hidden(const std::filesystem::path& path)									{ return path.filename().string()[0] == '.'; }

	const std::filesystem::path get_absolute_path(const std::filesystem::path& path)	{ return std::filesystem::absolute(path); }

	std::vector<std::filesystem::path> get_files_in_dir(const std::filesystem::path& path) {

		std::vector<std::filesystem::path> files;
		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (is_file(entry.path()))
				files.push_back(entry.path());

		return files;
	}

	std::vector<std::filesystem::path> get_folders_in_dir(const std::filesystem::path& path) {
		std::vector<std::filesystem::path> folders;
		for (const auto& entry : std::filesystem::directory_iterator(path))
			if (std::filesystem::is_directory(entry.path()))
				folders.push_back(entry.path());
		
		return folders;
	}

	bool write_to_file(const char* data, const std::filesystem::path& filename) {

		std::ofstream outStream(filename.string());
		VALIDATE(outStream.is_open(), return false, "", "could not open " << filename);
		outStream << data;
		outStream.close();
		return true;
	}

}
