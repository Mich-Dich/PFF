#pragma once

#include "application.h"
#include "controller/editor_controller.h"
#include "ui/editor_layer.h"

namespace PFF {

	class PFF_editor : public application {

	public:

		PFF_editor();
		~PFF_editor();

		//FORCEINLINE editor_layer* get_editor_layer() { return m_editor_layer; }

		bool init() override;
		bool shutdown() override;
		bool update(const f32 delta_time) override;

	private:
		std::shared_ptr<editor_controller> m_editor_controller{};
		//editor_layer* m_editor_layer;
		//PFF::game_object* m_test_game_object;
	};

}
