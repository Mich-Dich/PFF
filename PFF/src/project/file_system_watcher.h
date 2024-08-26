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


	class PFF_API file_system_watcher {
	public:

		file_system_watcher();
		~file_system_watcher() { stop(); }
		void start();
		void stop();

	public:
		typedef void (*on_changed_fnuc)(const std::filesystem::path& file);
		typedef void (*on_renamed_fnuc)(const std::filesystem::path& file);
		typedef void (*on_deleted_fnuc)(const std::filesystem::path& file);
		typedef void (*on_created_fnuc)(const std::filesystem::path& file);

		on_changed_fnuc			on_changed = nullptr;
		on_renamed_fnuc			on_renamed = nullptr;
		on_deleted_fnuc			on_deleted = nullptr;
		on_created_fnuc			on_created = nullptr;

		int						p_notify_filters = 0;
		bool					include_sub_directories = false;
		std::string				filter = "";
		std::filesystem::path	path = {};

	private:
		void start_thread();

	private:
		bool					m_enable_raising_events = true;
		std::thread				m_thread;

	};

}
