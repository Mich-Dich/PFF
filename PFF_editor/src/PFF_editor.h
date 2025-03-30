#pragma once

#include "application.h"
#include "controller/editor_controller.h"
#include "ui/editor_layer.h"
#include "editor_settings.h"

namespace PFF {

	class PFF_editor : public application {
	public:

		PFF_editor() {}
		~PFF_editor() {}

		FORCEINLINE editor_layer* get_editor_layer()						{ return m_editor_layer; }
		FORCEINLINE std::filesystem::path get_editor_executable_path()		{ return util::get_executable_path(); }
		FORCEINLINE static PFF_editor& get()								{ return static_cast<PFF_editor&>(application::get()); }
		PFF_DEFAULT_GETTER_REF(editor_settings, editor_settings);

		void serialize(serializer::option option);

		bool init() override;
		bool render(const f32 delta_time) override;
		bool update(const f32 delta_time) override;
		bool shutdown() override;

	private:


		editor_settings						m_editor_settings{};
		std::shared_ptr<editor_controller>	m_editor_controller{};
		editor_layer*						m_editor_layer;
	};

}
