
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "engine/render/material.h"
#include "engine/resource_management/material/material_headers.h"
#include "engine/resource_management/material/material_serializer.h"
#include "resource_management/factories/material_factory.h"
#include "engine/resource_management/general_resource_manager.h"
#include "engine/resource_management/image_asset_manager.h"
#include "PFF_editor.h"

#include "material_inst_create_window.h"

namespace PFF {

	static material_instance_creation_data        resources{};


	material_inst_create_window::material_inst_create_window(const std::filesystem::path destination_path)
		: destination_path(destination_path) {

		m_asset_alredy_exists = true;														// default to true while in dev
		std::strncpy(m_possible_asset_name, "new_instance", sizeof(m_possible_asset_name) - 1);
		m_possible_asset_name[sizeof(m_possible_asset_name) - 1] = '\0';
		resources = material_instance_creation_data{}; 										// reset resources
	}

	void material_inst_create_window::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

		if (ImGui::Begin("Material Instance creator", &show_window, window_flags)) {

			ImGui::Text("Name of new material instance");
			if (ImGui::InputText("Asset Name", m_possible_asset_name, sizeof(m_possible_asset_name)))
			m_asset_alredy_exists = std::filesystem::exists(destination_path / std::string(m_possible_asset_name));

			UI::begin_table("new material instance settings");
			
			UI::table_row([]() { ImGui::Text("Color Texture"); }, [&]() {

				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				const auto comp_asset_path = resources.color_texture.generic_string();
				if (!comp_asset_path.empty())
					ImGui::Text("%s", comp_asset_path.c_str());
				else
					ImGui::Text("no texture selected");

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_TEXTURE)) {
	
						LOG(Debug, "DROP")
	
						const std::filesystem::path file_path = (const char*)payload->Data;
						// const std::filesystem::path absolute_file_path = application::get().get_project_path() / CONTENT_DIR / (const char*)payload->Data;

						resource_manager::asset_curruption_source asset_curruption_reason;
						asset_file_header general_header{};
						if (resource_manager::try_to_deserialize_file_header(file_path, true, asset_curruption_reason, general_header)) {
	
							LOG(Debug, "Setting color texture")
							resources.color_texture = file_path;
						} else
							LOG(Warn, "Dropped file cant be used because file is currupted [" << (u32)asset_curruption_reason << "]")
						// image_asset_manager::get_from_path(file_path);											// TODO: save image in resources		load image only on "Confirm" button
					}
					ImGui::EndDragDropTarget();
				}
		
			});

			UI::table_row([]() { ImGui::Text("Metal/Roughness Texture"); }, [&]() {

				ImGui::SetNextItemWidth(ImGui::GetColumnWidth());
				const auto comp_asset_path = resources.metal_rough_texture.generic_string();
				if (!comp_asset_path.empty())
					ImGui::Text("%s", comp_asset_path.c_str());
				else
					ImGui::Text("no texture selected");

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DRAG_DROP_TEXTURE)) {
	
						const std::filesystem::path file_path = (const char*)payload->Data;
						// const std::filesystem::path absolute_file_path = application::get().get_project_path() / CONTENT_DIR / (const char*)payload->Data;
						resource_manager::asset_curruption_source asset_curruption_reason;
						asset_file_header general_header{};
						if (resource_manager::try_to_deserialize_file_header(file_path, true, asset_curruption_reason, general_header)) {
	
							resources.metal_rough_texture = file_path;
						}
						// image_asset_manager::get_from_path(file_path);											// TODO: save image in resources		load image only on "Confirm" button
					}
					ImGui::EndDragDropTarget();
				}
		
			});
			UI::end_table();

			
			UI::shift_cursor_pos(0, 10);
			if (m_asset_alredy_exists) {

				ImGui::Image(PFF_editor::get().get_editor_layer()->get_warning_icon()->get_descriptor_set(), ImVec2(15), ImVec2(0, 0), ImVec2(1, 1), ImVec4(.9f, .5f, 0.f, 1.f));
				ImGui::SameLine();
				ImGui::TextWrapped("The asset your trying to import already exists, if you continue you are overwriting the previous asset");				
			}
			UI::shift_cursor_pos(0, 10);

			const f32 width = ImGui::GetContentRegionAvail().x / 2 - (ImGui::GetStyle().ItemSpacing.x * 2);
			if (ImGui::Button("Cancle", ImVec2(width, 0)))
				show_window = false;

			ImGui::SameLine();

			const bool block_continue = resources.color_texture.empty() || resources.metal_rough_texture.empty();
			if (block_continue)
				ImGui::BeginDisabled();
			if (ImGui::Button("Create", ImVec2(width, 0))) {
						
				asset_file_header asset_header;
				asset_header.file_version = version{1, 0, 0};
				asset_header.type = file_type::material_instance;
				asset_header.timestamp = util::get_system_time();

				general_material_instance_file_header general_header;
				general_header.file_version = version{1, 0, 0};

				specific_material_instance_file_header specific_header;
				specific_header.source_file = source_path.generic_string();
				specific_header.parent_material_path = std::filesystem::path("/FAKE_PATH_FOR_DEV/DEV_ONLY.pffasset");           // hard code fake here and in material_asset_manger

				// material_factory::create_instance(destination_path, loc_load_options);
				LOG(Trace, "Serializing an instance to : " << destination_path / (std::string(m_possible_asset_name) + PFF_ASSET_EXTENTION));
				serialize_material_instance(destination_path / (std::string(m_possible_asset_name) + PFF_ASSET_EXTENTION), asset_header, general_header, specific_header, resources, serializer::option::save_to_file);
				show_window = false;
			}
			if (block_continue)
				ImGui::EndDisabled();

		}
		ImGui::End();
	}

}
