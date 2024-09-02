
#include "util/pffpch.h"

#include <regex>

#include "file_system_watcher.h"

#ifdef PFF_PLATFORM_WINDOWS
	#include "Windows.h"

	static HANDLE m_H_stop_event;
#endif

namespace PFF {

#define BIT_FLAG_IF(enum_value, flag)				if (p_notify_filters && enum_value)									\
														flags |= flag;

#define TRY_CALL_FUNCTION(func_name)				if (func_name!= nullptr)											\
														func_name(std::filesystem::path(std::string(filename)));

	static const std::vector<std::string> temp_extensions = { "~", ".TMP", ".tmp", ".temp", ".swp", ".bak" };
	static const std::vector<std::regex> temp_patterns = {
		std::regex(".*~RF[0-9a-f]{6}\\.TMP"),		// Visual Studio temp files
		std::regex(".*\\.[0-9a-zA-Z]{3}~")			// Other common temp file patterns
	};

	static bool is_started = false;
	

	file_system_watcher::file_system_watcher() { }

	void file_system_watcher::start() { m_thread = std::thread(&file_system_watcher::start_thread, this); }

	static bool should_ignore_file(const std::string filename) {

		if (filename[0] == '.')			// Ignore files that start with a dot (hidden files)
			return true;

		for (const auto& ext : temp_extensions) {
			if (filename.length() >= ext.length() && filename.compare(filename.length() - ext.length(), ext.length(), ext) == 0)
				return true;
		}

		for (const auto& pattern : temp_patterns) {
			if (std::regex_match(filename, pattern))
				return true;
		}

		return false;
	}

	void file_system_watcher::start_thread() {

		if (path.empty())
			return;

#ifdef PFF_PLATFORM_WINDOWS

		HANDLE dirHandle = CreateFileA(
			path.string().c_str(), 
			GENERIC_READ | FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, 
			OPEN_EXISTING, 
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if (dirHandle == INVALID_HANDLE_VALUE) {
		
			CORE_LOG(Error, "Invalid file access. Could not create file_system_watcher for " << path.string().c_str());
			return;
		}

		// Set up notification flags 
		int flags = 0;
		BIT_FLAG_IF(notify_filters::FileName, FILE_NOTIFY_CHANGE_FILE_NAME);
		BIT_FLAG_IF(notify_filters::DirectoryName, FILE_NOTIFY_CHANGE_DIR_NAME);
		BIT_FLAG_IF(notify_filters::Attributes, FILE_NOTIFY_CHANGE_ATTRIBUTES);
		BIT_FLAG_IF(notify_filters::Size, FILE_NOTIFY_CHANGE_SIZE);
		BIT_FLAG_IF(notify_filters::LastWrite, FILE_NOTIFY_CHANGE_LAST_WRITE);
		BIT_FLAG_IF(notify_filters::LastAccess, FILE_NOTIFY_CHANGE_LAST_ACCESS);
		BIT_FLAG_IF(notify_filters::CreationTime, FILE_NOTIFY_CHANGE_CREATION);
		BIT_FLAG_IF(notify_filters::Security, FILE_NOTIFY_CHANGE_SECURITY);

		char filename[MAX_PATH];
		char buffer[2048];
		DWORD bytesReturned;
		FILE_NOTIFY_INFORMATION* pNotify;
		int offset = 0;
		OVERLAPPED pollingOverlap;
		pollingOverlap.OffsetHigh = 0;
		pollingOverlap.hEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
		
		if (pollingOverlap.hEvent == NULL) {
			CORE_LOG(Error, "Could not create event watcher for FileSystemWatcher " << path.string().c_str());
			return;
		}

		is_started = true;

		bool result = true;
		HANDLE hEvents[2];
		hEvents[0] = pollingOverlap.hEvent;
		hEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);
		m_H_stop_event = hEvents[1];

		while (result && m_enable_raising_events) {
			result = ReadDirectoryChangesW(
				dirHandle,                   // handle to the directory to be watched
				&buffer,                     // pointer to the buffer to receive the read results
				sizeof(buffer),              // length of lpBuffer
				include_sub_directories,     // flag for monitoring directory or directory tree
				flags,
				&bytesReturned,              // number of bytes returned
				&pollingOverlap,             // pointer to structure needed for overlapped I/O
				NULL
			);

			DWORD event = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE);
			offset = 0;
			int rename = 0;

			if (event == WAIT_OBJECT_0 + 1) 
				break;
			
			do {
				pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buffer + offset);
				strcpy(filename, "");
				int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
				filename[pNotify->FileNameLength / 2] = '\0';

				if (should_ignore_file(std::string(filename))) {

					offset += pNotify->NextEntryOffset;
					continue;
				}

				switch (pNotify->Action) {
				case FILE_ACTION_ADDED:				TRY_CALL_FUNCTION(on_created) break;
				case FILE_ACTION_REMOVED:			TRY_CALL_FUNCTION(on_deleted) break;
				case FILE_ACTION_MODIFIED:			TRY_CALL_FUNCTION(on_changed) break;
				case FILE_ACTION_RENAMED_NEW_NAME:	TRY_CALL_FUNCTION(on_renamed) break;
				case FILE_ACTION_RENAMED_OLD_NAME:	LOG(Info, "The file was renamed and this is the old name: [" << filename << "]"); break;
				default:
					CORE_LOG(Error, "Default error. Unknown file action [" << pNotify->Action << "] for FileSystemWatcher " << path.string().c_str());
					break;
				}

				offset += pNotify->NextEntryOffset;
			} while (pNotify->NextEntryOffset);
		}

		CloseHandle(dirHandle);
	}

	void file_system_watcher::stop() {
	
		if (!m_enable_raising_events)
			return;

		m_enable_raising_events = false;
		SetEvent(m_H_stop_event);

		if (is_started)
			m_thread.join();
		
	}

#endif

}
