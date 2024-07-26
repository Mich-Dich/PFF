#pragma once

#include "engine/layer/layer.h"
#include "engine/game_objects/camera.h"
#include "engine/game_objects/player_controller.h"

namespace PFF {
	
	class world_layer : public layer {
	public:

		world_layer();
		~world_layer();

		DELETE_COPY(world_layer);

		PFF_DEFAULT_GETTER_C(ref<camera>, editor_camera)
		PFF_GETTER_C(ref<player_controller>, current_player_controller, m_player_controller)

		void register_player_controller(ref<player_controller> player_controller) {

			m_player_controller = player_controller;
			m_player_controller->set_world_layer_ref(this);
		}

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_update(const f32 delta_time);
		virtual void on_event(event& event);
		virtual void on_imgui_render();

	private:

		//camera m_camera{};
		ref<camera> m_editor_camera{};
		ref<player_controller> m_player_controller{};
		//ref<game_map> m_current_map{};

	};
	
}
