#pragma once


namespace PFF {

	class editor_window {
	public:

		editor_window() {};
		virtual ~editor_window() {};

		virtual void window() {};
		FORCEINLINE bool should_close() const { return !show_window; }
		FORCEINLINE virtual void show_possible_sub_window_options() {}

	protected:
		void make_window_name(const char* base_name);
		std::string window_name{};
		bool show_window = true;

	private:

	};

}
