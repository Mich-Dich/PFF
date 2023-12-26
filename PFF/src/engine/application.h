#pragma once
#include "engine/util.h"

#include "engine/core/window.h"
#include "engine/types.h"

namespace PFF {

	class PFF_API application {
	public:
		
		application();
		virtual ~application();

		void Run();

		void OnEvent(Event& e);

	private:

		std::unique_ptr<Window> m_Window = nullptr;
		bool running = true;
	};

	application* Create_application();

}

