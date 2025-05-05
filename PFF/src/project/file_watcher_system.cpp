
#include "util/pffpch.h"

#include <regex>

#include "file_watcher_system.h"

#ifdef PFF_PLATFORM_WINDOWS
	#include "Windows.h"
	static HANDLE m_H_stop_event;
#elif defined(PFF_PLATFORM_LINUX)
	#include <sys/inotify.h>
	#include <unistd.h>
	#include <limits.h>
	#include <sys/select.h>
	#include <errno.h>
#endif

namespace PFF {

#define BIT_FLAG_IF(enum_value, flag)				if (p_notify_filters && enum_value)									\
														flags |= flag;

#define FUNCTION_FILE_ARGUMANT						(std::filesystem::path(std::string(filename)))
#define TRY_CALL_FUNCTION(func_name)				if (func_name!= nullptr) func_name

	static const std::vector<std::string> temp_extensions = { "~", ".TMP", ".tmp", ".temp", ".swp", ".bak" };
	static const std::vector<std::regex> temp_patterns = {
		std::regex(".*~RF[0-9a-f]{6}\\.TMP"),		// Visual Studio temp files
		std::regex(".*\\.[0-9a-zA-Z]{3}~")			// Other common temp file patterns
	};

	static bool is_started = false;
	

	file_watcher_system::file_watcher_system() { }

	void file_watcher_system::start() {
	
		LOG(Trace, "starting file watcher system");
		m_thread = std::thread(&file_watcher_system::start_thread, this);
	}

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

	void file_watcher_system::process_event(const std::filesystem::path& file, int action) {
	
		std::lock_guard<std::mutex> lock(m_mutex);
		auto now = std::chrono::steady_clock::now();
		m_pending_events[file] = { action, now };
	}

#if defined(PFF_PLATFORM_WINDOWS)

	void file_watcher_system::start_thread() {

		if (path.empty())
			return;


		HANDLE dirHandle = CreateFileA(
			path.string().c_str(), 
			GENERIC_READ | FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, 
			OPEN_EXISTING, 
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		if (dirHandle == INVALID_HANDLE_VALUE) {
		
			LOG(Error, "Invalid file access. Could not create file_system_watcher for " << path.string().c_str());
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
			LOG(Error, "Could not create event watcher for FileSystemWatcher " << path.string().c_str());
			return;
		}

		is_started = true;

		bool result = true;
		HANDLE hEvents[2];
		hEvents[0] = pollingOverlap.hEvent;
		hEvents[1] = CreateEventA(NULL, TRUE, FALSE, NULL);
		m_H_stop_event = hEvents[1];
		
		LOG(Debug, "Compiling project");
		TRY_CALL_FUNCTION(compile)();

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

				process_event(FUNCTION_FILE_ARGUMANT, pNotify->Action);
				offset += pNotify->NextEntryOffset;

			} while (pNotify->NextEntryOffset);

			// process debounced events
			std::vector<std::pair<std::filesystem::path, int>> events_to_process;
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				auto now = std::chrono::steady_clock::now();

				for (auto it = m_pending_events.begin(); it != m_pending_events.end();) {
					if (now - it->second.second >= m_debounce_time) {
						events_to_process.emplace_back(it->first, it->second.first);
						it = m_pending_events.erase(it);
					} else
						++it;
				}
			}

			for (const auto& [file, action] : events_to_process) {
				switch (action) {
				case FILE_ACTION_ADDED:				TRY_CALL_FUNCTION(on_created)(file); break;
				case FILE_ACTION_REMOVED:			TRY_CALL_FUNCTION(on_deleted)(file); break;
				case FILE_ACTION_MODIFIED:			TRY_CALL_FUNCTION(on_changed)(file); break;
				case FILE_ACTION_RENAMED_NEW_NAME:	TRY_CALL_FUNCTION(on_renamed)(file); break;
				}
			}

			if (!events_to_process.empty())
				TRY_CALL_FUNCTION(compile)();
		}

		CloseHandle(dirHandle);

	}

	void file_watcher_system::stop() {
	
		LOG(Info, "Stopping thread [file_watcher_system]")
		if (!m_enable_raising_events)
			return;

		m_enable_raising_events = false;
		SetEvent(m_H_stop_event);

		if (is_started)
			if (m_thread.joinable())
				m_thread.join();
		
	}

#elif defined(PFF_PLATFORM_LINUX)

	void file_watcher_system::start_thread() {

		VALIDATE(!path.empty(), return, "", "Provided path is empty");
		m_inotify_fd = inotify_init1(IN_NONBLOCK);
		VALIDATE(m_inotify_fd != -1, return, "", "Failed to initialize inotify: " << strerror(errno));
		VALIDATE(pipe(m_stop_pipe) != -1, return, "", "Failed to create stop pipe: " << strerror(errno));

		// Map to track watch descriptors to their paths
		std::unordered_map<int, std::filesystem::path> wd_to_path;

		// Lambda to add watches and update map
		auto add_watch = [&](const std::filesystem::path& dir) {
			int wd = inotify_add_watch(m_inotify_fd, dir.c_str(), 
									IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO | IN_ATTRIB);
			if (wd == -1) {
				LOG(Error, "Failed to add watch for " << dir << ": " << strerror(errno));
				return -1;
			}
			wd_to_path[wd] = dir;
			return wd;
		};

		// Add main directory watch
		if (add_watch(path) == -1) {
			close(m_stop_pipe[0]);
			close(m_stop_pipe[1]);
			close(m_inotify_fd);
			return;
		}

		// Add subdirectory watches if needed
		if (include_sub_directories) {
			try {
				for (auto& entry : std::filesystem::recursive_directory_iterator(path)) {
					if (entry.is_directory()) {
						add_watch(entry.path());
					}
				}
			} catch (const std::filesystem::filesystem_error& e) {
				LOG(Error, "Directory iterator error: " << e.what());
			}
		}

		is_started = true;

		LOG(Debug, "Compiling project");
		TRY_CALL_FUNCTION(compile)();

		constexpr size_t BUFFER_SIZE = (sizeof(struct inotify_event) + NAME_MAX + 1) * 100;
		char buffer[BUFFER_SIZE];

		while (m_enable_raising_events) {
			fd_set fds;
			FD_ZERO(&fds);
			FD_SET(m_inotify_fd, &fds);
			FD_SET(m_stop_pipe[0], &fds);

			int max_fd = std::max(m_inotify_fd, m_stop_pipe[0]);

			int ready = select(max_fd + 1, &fds, nullptr, nullptr, nullptr);
			if (ready == -1) {
				if (errno == EINTR) continue;
				LOG(Error, "select error: " << strerror(errno));
				break;
			}

			if (FD_ISSET(m_stop_pipe[0], &fds)) {
				char tmp;
				read(m_stop_pipe[0], &tmp, sizeof(tmp));
				break;
			}

			if (FD_ISSET(m_inotify_fd, &fds)) {
				ssize_t len = read(m_inotify_fd, buffer, BUFFER_SIZE);
				if (len == -1) {
					if (errno == EAGAIN) continue;
					LOG(Error, "read error: " << strerror(errno));
					break;
				}

				for (char* ptr = buffer; ptr < buffer + len; ) {
					struct inotify_event* event = reinterpret_cast<struct inotify_event*>(ptr);
					ptr += sizeof(struct inotify_event) + event->len;

					auto it = wd_to_path.find(event->wd);
					if (it == wd_to_path.end()) continue;

					std::filesystem::path dir_path = it->second;
					std::string filename(event->name);
					std::filesystem::path full_path = dir_path / filename;

					// Handle directory creation for recursive watching
					if ((event->mask & IN_ISDIR) && (event->mask & IN_CREATE) && include_sub_directories) {
						if (add_watch(full_path) != -1) {
							LOG(Debug, "Added watch for new directory: " << full_path);
						}
					}

					if (should_ignore_file(filename))
						continue;

					// Map inotify events to actions
					int action = 0;
					if (event->mask & (IN_CREATE | IN_MOVED_TO)) {
						action = event->mask & IN_ISDIR ? 0 : FILE_ACTION_ADDED;
					} else if (event->mask & (IN_DELETE | IN_MOVED_FROM)) {
						action = event->mask & IN_ISDIR ? 0 : FILE_ACTION_REMOVED;
					} else if (event->mask & IN_MODIFY) {
						action = FILE_ACTION_MODIFIED;
					} else if (event->mask & IN_ATTRIB) {
						action = FILE_ACTION_ATTRIBUTES_CHANGED;
					}

					if (action != 0) {
						process_event(full_path, action);
					}
				}

				// Process debounced events
				std::vector<std::pair<std::filesystem::path, int>> events_to_process;
				{
					std::lock_guard<std::mutex> lock(m_mutex);
					auto now = std::chrono::steady_clock::now();

					for (auto it = m_pending_events.begin(); it != m_pending_events.end();) {
						if (now - it->second.second >= m_debounce_time) {
							events_to_process.emplace_back(it->first, it->second.first);
							it = m_pending_events.erase(it);
						} else {
							++it;
						}
					}
				}

				for (const auto& [file, action] : events_to_process) {
					switch (action) {
						case FILE_ACTION_ADDED: TRY_CALL_FUNCTION(on_created)(file); break;
						case FILE_ACTION_REMOVED: TRY_CALL_FUNCTION(on_deleted)(file); break;
						case FILE_ACTION_MODIFIED: TRY_CALL_FUNCTION(on_changed)(file); break;
						case FILE_ACTION_RENAMED_NEW_NAME: TRY_CALL_FUNCTION(on_renamed)(file); break;
					}
				}

				if (!events_to_process.empty())
					TRY_CALL_FUNCTION(compile)();
			}
		}

		// Cleanup
		for (auto& [wd, _] : wd_to_path) {
			inotify_rm_watch(m_inotify_fd, wd);
		}
		close(m_inotify_fd);
		close(m_stop_pipe[0]);
		close(m_stop_pipe[1]);
		is_started = false;
	}

	void file_watcher_system::stop() {

		LOG(Info, "Stopping thread [file_watcher_system]");
		if (!m_enable_raising_events)
			return;
	
		m_enable_raising_events = false;
		if (m_stop_pipe[1] != -1) {
			char dummy = 0;
			write(m_stop_pipe[1], &dummy, 1);
		}
	
		if (is_started && m_thread.joinable())
			m_thread.join();
	}


#endif

}
