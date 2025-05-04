#pragma once

namespace PFF::UI {

	enum class notification_type {
		none = 0,
		success,
		warning,					// add "close" button to notification and wait for user to close notification
		error,						// add "close" button to notification and wait for user to close notification
	};
	
	void add_notification(std::string title, std::string message, notification_type severity = notification_type::none);

}