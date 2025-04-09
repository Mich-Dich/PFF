#pragma once

#include "editor_window.h"
#include "content_browser.h"

namespace PFF {

	class world_viewport_window : public editor_window {
	public:

		world_viewport_window();
		~world_viewport_window();

		void window() override;
		virtual void show_possible_sub_window_options() override;

		PFF_DEFAULT_GETTER_REF(transform_operation, gizmo_operation)

	private:

		void serialize(serializer::option option);
		void window_general_debugger();								// TODO: convert into editor window
		void window_world_settings();								// TODO: convert into editor window
		void window_main_viewport();
		void window_outliner();
		void window_details();
		void window_renderer_backgrond_effect();

		void list_all_components(const PFF::entity entity);
		void add_show_hide_icon(PFF::entity& entity);
		void display_entity_children(ref<map> loc_map, PFF::entity entity);
		void outliner_entity_popup(const char* name, ref<map> map, PFF::entity entity);

		bool									m_show_renderer_backgrond_effect = false;
		bool									m_show_world_settings = false;
		bool									m_show_general_debugger = true;
		bool									m_show_log_display = true;
		bool									m_show_content_browser_0 = true;
		bool									m_show_content_browser_1 = false;
		bool									m_show_details = false;
		bool									m_show_outliner = true;

		content_browser							m_content_browser{};

		ImGuiTreeNodeFlags						outliner_base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen;
		entity									m_selected_entity = entity();

		ref<image>								m_transfrom_translation_image;
		ref<image>								m_transfrom_rotation_image;
		ref<image>								m_transfrom_scale_image;
		ref<image>								m_folder_add_icon;
		ref<image>								m_mesh_asset_icon;
		ref<image>								m_relation_icon;
		ref<image>								m_file_icon;
		ref<image>								m_file_proc_icon;
		ref<image>								m_mesh_mini_icon;
		ref<image>								m_show_icon;
		ref<image>								m_hide_icon;

		transform_operation						m_gizmo_operation = transform_operation::translate;
		util::simple_deletion_queue				m_deletion_queue{};
	};

}
