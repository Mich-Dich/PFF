
#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"
#include "mesh_import_window.h"
#include "PFF_editor.h"

// DEV-ONLY
#define PFF_DEBUG
#include "util/profiling/instrumentor.h"

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

	struct TEST_STRUCT {

		std::filesystem::path		path;
	};

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
			
			util::random loc_random{};

			std::stringstream ss_befor{};
			std::stringstream ss_after{};

			//CORE_LOG(Info, "Testing Serialization");

			{
				TEST_STRUCT test{};

				u64 length = loc_random.get_u64(10, 30);
				test.path = std::filesystem::path("start");
				for (u64 x = 0; x < length; x++) {

					u64 str_length = loc_random.get_u64(10, 30);
					test.path /= loc_random.get_string((size_t)str_length);
				}

				serializer::binary(PFF_editor::get().get_project_path() / "TEST.txt", "some_stuff", serializer::option::save_to_file)
					.entry(test.path);

				ss_befor << util::to_string(test.path);
				CORE_LOG(Trace, "befor: " << ss_befor.str());
			}

			TEST_STRUCT result{};

			{
				serializer::binary(PFF_editor::get().get_project_path() / "TEST.txt", "some_stuff", serializer::option::load_from_file)
					.entry(result.path);
			}

			ss_after << util::to_string(result.path);
			CORE_LOG(Trace, "befor: " << ss_after.str());

			// Compare the strings
			CORE_ASSERT(ss_befor.str() == ss_after.str(), "", "Error");

		}

		UI::custom_frame_NEW(350, true, IM_COL32(37, 37, 37, 255), [&]() {
			
			show_directory_tree(m_project_directory / "content");

		}, []() {



		});


		ImGui::End();
	}

}
