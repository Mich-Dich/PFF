#pragma once

#include "engine/layer/layer.h"
#include "engine/map/game_map.h"

namespace PFF {

	class world_layer : public layer {

	public:

		world_layer();
		~world_layer();

		DELETE_COPY(world_layer);

		FORCEINLINE std::shared_ptr<game_map> get_current_map() const { return m_current_map; }

		virtual void on_attach();
		virtual void on_detach();
		virtual void on_update(f32 delta_time);
		virtual void on_event(event& event);
		virtual void on_imgui_render();

	private:

		std::shared_ptr<game_map> m_current_map = nullptr;

	};

}
