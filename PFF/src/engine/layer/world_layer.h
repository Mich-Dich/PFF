#pragma once

//#include "engine/layer/layer.h"
//#include "engine/map/game_map.h"
//#include "engine/game_objects/camera.h"
//#include "engine/game_objects/player_controller.h"

namespace PFF {
	/*
	class world_layer : public layer {
	public:

		world_layer();
		~world_layer();

		DELETE_COPY(world_layer);

		//FORCEINLINE camera get_camera() const { return m_camera; }
		FORCEINLINE ref<camera> get_editor_camera() const { return m_editor_camera; }
		//FORCEINLINE ref<game_map> get_current_map() const { return m_current_map; }
		FORCEINLINE ref<player_controller> get_current_player_controller() const { return m_player_controller; }
		void register_player_controller(ref<player_controller> player_controller) {

			m_player_controller = player_controller;
			m_player_controller->set_world_layer_ref(this);
		}

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_update(const f32 delta_time);
		virtual void on_event(event& event);
		virtual void on_imgui_render();			#error Implement performance display

	private:

		//camera m_camera{};
		ref<camera> m_editor_camera{};
		ref<player_controller> m_player_controller{};
		//ref<game_map> m_current_map{};

	};
	*/
}
