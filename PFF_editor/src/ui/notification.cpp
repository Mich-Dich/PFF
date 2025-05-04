
#include "util/pch_editor.h"

#include "notification.h"

namespace PFF::UI {

	struct notification {

		std::string									title;
		std::string									message;
		notification_type							severity = notification_type::none;
		std::chrono::steady_clock::time_point		start_time = std::chrono::steady_clock::time_point::min();
	};

	static std::vector<notification> notifications_to_display{};			// uses for entire duration of editor



	void add_notification(std::string title, std::string message, notification_type severity) {

		notifications_to_display.emplace_back(std::move(title), std::move(message), severity, std::chrono::steady_clock::now());

	}

}
