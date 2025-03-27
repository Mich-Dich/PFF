#pragma once

#include "engine/io_handler/input_mapping.h"

#include "engine/layer/layer.h"

namespace PFF {

	class event;
	class key_event;
	class mouse_event;
	class world_layer;

	class player_controller {	
	public:

		player_controller();
		~player_controller();

		PFF_DELETE_COPY_CONSTRUCTOR(player_controller);

		USE_IN_EDITOR void set_world_layer_ref(world_layer* world_layer);
		USE_IN_EDITOR FORCEINLINE const ref<input_mapping> get_input_mapping() const { return m_input_mapping; }

		virtual void init();
		virtual void update(f32 delta);
		void update_internal(f32 delta);
		void handle_event(event& event);

		template <typename T>
		ref<T> register_mapping() {

			LOG(Trace, "register Input mapping");
			ref<T> mapping = create_ref<T>();
			m_input_mapping = std::static_pointer_cast<input_mapping>(mapping);
			return mapping;
		}

		world_layer* m_world_layer{};
	private:

		ref<input_mapping> m_input_mapping{};

		bool handle_key_events(key_event& event);
		bool handle_mouse_events(mouse_event& event);

	};

}
