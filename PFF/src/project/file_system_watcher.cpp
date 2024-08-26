
#include "util/pffpch.h"

#include "file_system_watcher.h"

#ifdef PFF_PLATFORM_WINDOWS
	#include "Windows.h"

	static HANDLE m_H_stop_event;
#endif

namespace PFF {

	static bool is_started = false;
	
	file_system_watcher::file_system_watcher() { }


	void file_system_watcher::start() { m_thread = std::thread(&file_system_watcher::start_thread, this); }


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
		
			CORE_LOG(Error, "Invalid file access. Could not create FileSystemWatcher for " << path.string().c_str());
			return;
		}

		// Set up notification flags 
		int flags = 0;
		if (p_notify_filters && notify_filters::FileName)
			flags |= FILE_NOTIFY_CHANGE_FILE_NAME;

		if (p_notify_filters && notify_filters::DirectoryName)
			flags |= FILE_NOTIFY_CHANGE_DIR_NAME;
		
		if (p_notify_filters && notify_filters::Attributes)
			flags |= FILE_NOTIFY_CHANGE_ATTRIBUTES;
		
		if (p_notify_filters && notify_filters::Size)
			flags |= FILE_NOTIFY_CHANGE_SIZE;
		
		if (p_notify_filters && notify_filters::LastWrite)
			flags |= FILE_NOTIFY_CHANGE_LAST_WRITE;
		
		if (p_notify_filters && notify_filters::LastAccess)
			flags |= FILE_NOTIFY_CHANGE_LAST_ACCESS;
		
		if (p_notify_filters && notify_filters::CreationTime)
			flags |= FILE_NOTIFY_CHANGE_CREATION;
		
		if (p_notify_filters && notify_filters::Security)
			flags |= FILE_NOTIFY_CHANGE_SECURITY;

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
				switch (pNotify->Action) {
				case FILE_ACTION_ADDED:
					if (on_created != nullptr) 
						on_created(std::filesystem::path(std::string(filename)));
					
					break;
				case FILE_ACTION_REMOVED:
					if (on_deleted != nullptr) 
						on_deleted(std::filesystem::path(std::string(filename)));
					
					break;
				case FILE_ACTION_MODIFIED:
					if (on_changed != nullptr) 
						on_changed(std::filesystem::path(std::string(filename)));
					
					break;
				case FILE_ACTION_RENAMED_OLD_NAME:
					// Logger::Info("The file was renamed and this is the old name: [%s]", filename);
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					if (on_renamed != nullptr) 
						on_renamed(std::filesystem::path(std::string(filename)));
					
					break;
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
