#pragma once

#include "util/pffpch.h"

#include "entity.h"

namespace PFF {

	class PFF_API entity_script {
	public:

		virtual ~entity_script() {}

		// simple wrappers around entity functions
		template<typename T, typename... Args>
		FORCEINLINE T& add_component(Args&&... args) { return m_entity.add_component<T>(std::forward<Args>(args)...); }

		template<typename T, typename... Args>
		FORCEINLINE T& add_or_replace_component(Args&&... args) { return m_entity.add_or_replace_component<T>(std::forward<Args>(args)...); }

		template<typename T>
		FORCEINLINE T& get_component() { return m_entity.get_component<T>(); }

		template<typename T>
		FORCEINLINE void remove_component() { return m_entity.remove_component<T>(); }

		template<typename T>
		FORCEINLINE bool has_component() { return m_entity.has_component<T>(); }

	protected:
		virtual void on_create() {}
		virtual void on_destroy() {}
		virtual void on_update(f32 delta_time) {}

	private:
		entity			m_entity;
		friend class map;
	};

}