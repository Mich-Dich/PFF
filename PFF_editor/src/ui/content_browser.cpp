
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "mesh_import_window.h"
#include "PFF_editor.h"

#include "content_browser.h"

namespace PFF {

	FORCEINLINE void content_browser::show_directory_tree(const std::filesystem::path& path) {

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_SpanAvailWidth;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {

			if (!entry.is_directory())
				continue;

			bool has_sub_folders = false;
			for (const auto& sub_entry : std::filesystem::directory_iterator(entry.path())) {		// <= HERE I GET AN ERROR
				if (!sub_entry.is_directory())
					continue;

				has_sub_folders = true;
				break;
			}

			if (has_sub_folders) {

				bool buffer = ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags);
				if (ImGui::IsItemClicked())
					m_selected_directory = entry.path();

				if (buffer) {

					show_directory_tree(entry.path());
					ImGui::TreePop();
				}

			} else {

				ImGui::TreeNodeEx(entry.path().filename().string().c_str(), base_flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen);
				if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
					m_selected_directory = entry.path();

			}
		}

    }


	content_browser::content_browser() { 

		m_project_directory = PFF_editor::get().get_project_path();
		m_selected_directory = m_project_directory / "content";
		window_name = "Content Browser"; 
	}

	#pragma pack(push, 1)
	struct TEST_STRUCT {

		bool test;
		std::filesystem::path path;
	};
	#pragma pack(pop)

	void content_browser::window() {

		if (!show_window)
			return;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
		ImGui::Begin(window_name.c_str(), &show_window, window_flags);

		if (ImGui::Button(" import ##content_browser_import")) {

			std::filesystem::path source_path = util::file_dialog();

			if (source_path.extension() == ".gltf" || source_path.extension() == ".glb")
				PFF_editor::get().get_editor_layer()->add_window<mesh_import_window>( source_path, m_selected_directory);

			// TODO: add more import dialogs depending on extention

		}

		ImGui::SameLine();
		if (ImGui::Button(" DO some useless stuff ")) {
			
			{
				TEST_STRUCT test{};
				test.test = false;
				test.path = util::get_executable_path() / "SERIALIZATION";

				serializer::binary(PFF_editor::get().get_project_path() / "TEST.txt", "some_stuff", serializer::option::save_to_file)
					.entry(test);

				CORE_LOG(Info, "TEST_STRUCT: " << util::to_string(test.test) << " | " << test.path);
			}
			
			TEST_STRUCT result{};

			CORE_LOG(Info, "Deserialization");
			serializer::binary(PFF_editor::get().get_project_path() / "TEST.txt", "some_stuff", serializer::option::load_from_file)
				.entry(result);
			CORE_LOG(Info, "result");

			CORE_LOG(Info, "TEST_STRUCT: " << util::to_string(result.test) << " | " << result.path);
		}

		UI::custom_frame_NEW(350, true, IM_COL32(37, 37, 37, 255), [&]() {
			
			show_directory_tree(m_project_directory / "content");

		}, []() {



		});


		ImGui::End();
	}

}
