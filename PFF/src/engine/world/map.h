#pragma once


namespace PFF {

	class map {
	public:

		map();
		~map();

		void on_runtime_start();
		void on_runtime_stop();

		void on_simulation_start();
		void on_simulation_stop();

		void on_component_added();
		void on_component_removed();

	private:

		entt::registry m_registry{};

		friend class entity;
	};
}
