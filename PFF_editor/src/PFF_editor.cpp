
#include "util/pch_editor.h"

#include "project/project_data_struct.h"
#include "native_scripting/project_file_watcher.h"

#include "PFF_editor.h"


namespace PFF {

	ref<project_file_watcher> p_project_file_watcher{};
	std::thread import_dialog_thread;

	void PFF_editor::serialize(serializer::option option) {

		// list all serialize functions
		m_editor_controller->serialize(option);

		serializer::yaml(config::get_filepath_from_configtype(util::get_executable_path(), config::file::editor), "editor_settings", option)
			.entry(KEY_VALUE(m_editor_settings.display_rotator_in_degrees));
	}

	bool PFF_editor::init() { 

		m_editor_layer = new editor_layer(application::get().get_imgui_layer()->get_context());
		push_overlay(m_editor_layer);

		io::create_directory(application::get().get_project_path() / "metadata" / "project_files");			// create location

		LOG(Trace, "register editor controller");
		m_editor_controller = std::make_shared<editor_controller>();
		register_player_controller(m_editor_controller);

		serialize(serializer::option::load_from_file);

		// TODO: load from project data
		p_project_file_watcher = create_ref<project_file_watcher>();
		return true;
	}

	bool PFF_editor::shutdown() {

		LOG(Trace, "serializing editor data")
		serialize(serializer::option::save_to_file);

		// LOG(Info, "TODO: making sure the import thread is closed")
		// if (import_dialog_thread.joinable())
		// 	import_dialog_thread.join();

		LOG(Trace, "Removing editor layer")
		pop_overlay(m_editor_layer);
		delete m_editor_layer;

		LOG_SHUTDOWN(); 
		return true;
	}

	bool PFF_editor::render(const f32 delta_time) { return true; }

	bool PFF_editor::update(const f32 delta_time) { return true; }

}
