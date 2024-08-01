#pragma once

#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"


namespace PFF::UI {

	static bool show_engine_wiki = false;
	static std::string wiki_text{};
	static const std::filesystem::path path_to_wiki = std::filesystem::path() / ".." / ".github" / "wiki";

	std::string read_file_to_string(const std::filesystem::path filepath) {

		std::ifstream stream(filepath);
		CORE_VALIDATE(stream.is_open(), return std::string("Could not open file"), "", "File [" << filepath << "] could not be opend");

		std::stringstream buffer;
		buffer << stream.rdbuf();
		stream.close();
		return buffer.str();
	}
	
	void display_directory_tree(const std::filesystem::path& dir_path, int depth = 0) {

		//static std::string selected_file;
		for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {

			std::string indent(depth * 2, ' ');
			if (entry.is_directory()) {
				if (ImGui::TreeNode((indent + entry.path().filename().string()).c_str())) {
					display_directory_tree(entry.path(), depth + 1);
					ImGui::TreePop();
				}
			}
			
			else if (UI::gray_button((indent + entry.path().filename().string()).c_str()))
				wiki_text = read_file_to_string(entry.path());
			
		}

	}

	void engine_wiki_window() {

		if (!show_engine_wiki)
			return;

		if (wiki_text.empty())
			wiki_text = read_file_to_string(path_to_wiki / "home.md");

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 500, viewport->Size.y - 300), ImGuiCond_Appearing);

		ImGuiWindowFlags flags = 0;
		if (!ImGui::Begin("Engine Wiki", &show_engine_wiki, flags)) {

			ImGui::End();
			return;
		}

		UI::custom_frame(200, [] {

			display_directory_tree(path_to_wiki);

		}, [] {

			ImGui::BeginChild("ayosöfdhg");
			UI::markdown(wiki_text);
			ImGui::EndChild();

		});

		ImGui::End();
	}

}
//
//
//CORE_LOG(Trace, "Home is clicked");
//
//if (UI::add_gray_button("Home")) {
//
//	CORE_LOG(Trace, "BUTTON is clicked");
//}
