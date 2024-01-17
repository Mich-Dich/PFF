#pragma once

#include "engine/io_handler/input_mapping.h"

#include "engine/layer/layer.h"

namespace PFF {

	class event;
	class key_event;
	class mouse_event;
	class world_layer;

	class PFF_API player_controller {	
	public:

		player_controller();
		~player_controller();

		DELETE_COPY(player_controller);

		USE_IN_EDITOR void set_world_layer_ref(world_layer* world_layer);
		USE_IN_EDITOR FORCEINLINE const std::shared_ptr<input_mapping> get_input_mapping() const { return m_input_mapping; }

		virtual void init();
		virtual void update(f32 delta);
		void update_internal(f32 delta);
		void handle_event(event& event);

		template <typename T>
		std::shared_ptr<T> register_mapping() {
			
			std::shared_ptr<T> mapping = std::make_shared<T>();
			m_input_mapping = std::static_pointer_cast<input_mapping>(mapping);
			return mapping;
		}

		// Struct for input values
		template <typename T>
		struct input_value_type {};

		template <>
		struct input_value_type < bool > {
			using type = bool;
		};

		template <>
		struct input_value_type < glm::vec2 > {
			using type = glm::vec2;
		};

		template <>
		struct input_value_type < f32 > {
			using type = f32;
		};

		// Templated function to get the relevant data field
		template <typename T>
		typename input_value_type < T > ::type get_data(const T& input) {
			return input.data.axis_2d; // Adjust based on your actual data structure
		}

		world_layer* m_world_layer{};
	private:

		std::shared_ptr<input_mapping> m_input_mapping{};

		bool handle_key_events(key_event& event);
		bool handle_mouse_events(mouse_event& event);

	};

}
