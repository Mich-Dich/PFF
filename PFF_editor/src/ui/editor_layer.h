#pragma once

#include "engine/layer/layer.h"
#include "engine/io_handler/input_mapping.h"
#include "editor_window.h"
#include "world_viewport.h"

struct ImGuiContext;

namespace PFF {
	
	class renderer;
	//class content_browser;

	class editor_layer : public layer {
	public:

		editor_layer(ImGuiContext* context);
		~editor_layer();

		template<typename window_type, typename... Args>
		void add_window(Args&&... args) {
			static_assert(std::is_base_of<editor_window, window_type>::value, "[window_type] must derive from editor_window");
			m_editor_windows.emplace_back(create_scoped_ref<window_type>(std::forward<Args>(args)...));
		}

		void on_attach() override;
		void on_detach() override;
		void on_update(f32 delta_time) override;
		void on_event(event& event) override;
		void on_imgui_render() override;

		PFF_DEFAULT_GETTER(ref<image>,		transfrom_translation_image)
		PFF_DEFAULT_GETTER(ref<image>,		transfrom_rotation_image)
		PFF_DEFAULT_GETTER(ref<image>,		transfrom_scale_image)
		PFF_DEFAULT_GETTER(ref<image>,		folder_icon)
		PFF_DEFAULT_GETTER(ref<image>,		folder_add_icon)
		PFF_DEFAULT_GETTER(ref<image>,		folder_open_icon)
		PFF_DEFAULT_GETTER(ref<image>,		folder_big_icon)
		PFF_DEFAULT_GETTER(ref<image>,		world_icon)
		PFF_DEFAULT_GETTER(ref<image>,		mesh_asset_icon)
		PFF_DEFAULT_GETTER(ref<image>,		relation_icon)
		PFF_DEFAULT_GETTER(ref<image>,		file_icon)
		PFF_DEFAULT_GETTER(ref<image>,		file_proc_icon)
		PFF_DEFAULT_GETTER(ref<image>,		mesh_mini_icon)
		PFF_DEFAULT_GETTER(ref<image>,		show_icon)
		PFF_DEFAULT_GETTER(ref<image>,		hide_icon)

	private:

		void serialize(serializer::option option);
		
		std::vector<scope_ref<editor_window>> m_editor_windows{};		// generic

		// ImGui windows
		void window_main_title_bar();
		void window_main_viewport();
		void window_main_content();
		void window_editor_settings();
		void window_general_settings();
		void main_menu_bar();

		bool m_show_main_menu_bar = false;
		bool m_show_options = false;
		bool m_show_graphics_engine_settings = false;
		bool m_show_editor_settings = false;
		bool m_show_general_settings = false;

#ifdef PFF_EDITOR_DEBUG
		bool style_editor = false;
		bool demo_window = true;
#endif

		std::vector<VkPresentModeKHR>		m_swapchain_supported_presentmodes;
		std::vector<const char*>			m_swapchain_supported_presentmodes_str;

		ImGuiContext*						m_context;
		f32									m_font_size = 16.0f;
		f32									m_titlebar_height = 60.f;


		world_viewport_window*				m_world_viewport_window;

		ref<image>							m_transfrom_translation_image;
		ref<image>							m_transfrom_rotation_image;
		ref<image>							m_transfrom_scale_image;
		ref<image>							m_folder_icon;
		ref<image>							m_folder_add_icon;
		ref<image>							m_folder_open_icon;
		ref<image>							m_folder_big_icon;
		ref<image>							m_world_icon;
		ref<image>							m_mesh_asset_icon;
		ref<image>							m_relation_icon;
		ref<image>							m_file_icon;
		ref<image>							m_file_proc_icon;
		ref<image>							m_mesh_mini_icon;
		ref<image>							m_show_icon;
		ref<image>							m_hide_icon;
	};
}
