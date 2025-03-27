#pragma once

namespace PFF {

	enum notify_filters {
		FileName = 1,
		DirectoryName = 2,
		Attributes = 4,
		Size = 8,
		LastWrite = 16,
		LastAccess = 32,
		CreationTime = 64,
		Security = 256
	};


	class file_watcher_system {
	public:

		file_watcher_system();
		~file_watcher_system() { stop(); }
		void start();
		void stop();

	public:
		typedef void (*on_changed_fnuc)(const std::filesystem::path& file);
		typedef void (*on_renamed_fnuc)(const std::filesystem::path& file);
		typedef void (*on_deleted_fnuc)(const std::filesystem::path& file);
		typedef void (*on_created_fnuc)(const std::filesystem::path& file);
		typedef void (*compile_fnuc)();

		on_changed_fnuc				on_changed = nullptr;
		on_renamed_fnuc				on_renamed = nullptr;
		on_deleted_fnuc				on_deleted = nullptr;
		on_created_fnuc				on_created = nullptr;
		compile_fnuc				compile = nullptr;

		int							p_notify_filters = 0;
		bool						include_sub_directories = false;
		std::string					filter = "";
		std::filesystem::path		path = {};

	private:

		void start_thread();
		void process_event(const std::filesystem::path& file, int action);

		bool						m_enable_raising_events = true;
		std::thread					m_thread;

		// for debouncing
		std::mutex					m_mutex;
		std::chrono::milliseconds	m_debounce_time{ 100 };
		std::unordered_map<std::filesystem::path, std::pair<int, std::chrono::steady_clock::time_point>> m_pending_events;

	};

}
