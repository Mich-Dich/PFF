#include "application.h"
#include "stdio.h"

#include "engine/core/Logger.h"

namespace PFF {

	application::application() {

		Logger::Init("LogCoreFile", "LogFile", "[$B$T:$J$E] [$B$L$X - $K - $P:$G$E] $C$Z");

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(std::bind(&application::OnEvent, this, std::placeholders::_1));
	}

	application::~application() {}

	void application::Run() {

        CORE_LOG(Trace, "application Run\n");
		while (running) {



		}
	}

	void application::OnEvent(Event& e) {
		
		CORE_LOG(Info, "Event" << e.GetName());
	}
}
