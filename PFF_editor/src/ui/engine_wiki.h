#pragma once

#include "util/pch_editor.h"

#include "util/ui/pannel_collection.h"

#define ENGINE_WIKI_WINDOW_NAME     "Engine Wiki"

namespace PFF::UI {

	static bool show_engine_wiki = false;
	static std::string wiki_text{};
	static const std::filesystem::path path_to_wiki = util::get_executable_path() / "wiki";

	std::string read_file_to_string(const std::filesystem::path filepath) {

		std::ifstream stream(filepath);
		VALIDATE(stream.is_open(), return std::string("Could not open file"), "", "File [" << filepath << "] could not be opend");

		std::stringstream buffer;
		buffer << stream.rdbuf();
		stream.close();
		return buffer.str();
	}
	
	void display_directory_tree(const std::filesystem::path& dir_path, int depth = 0) {

        std::vector<std::filesystem::directory_entry> directories;
        std::vector<std::filesystem::directory_entry> files;
        std::filesystem::directory_entry home_file;

        // Collect directories and files
        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_directory()) {
                directories.push_back(entry);
            } else {
                if (entry.path().extension() == ".md") {
                    if (entry.path().filename() == "home.md") {
                        home_file = entry;  // Save the home file separately
                    } else {
                        files.push_back(entry);
                    }
                }
            }
        }

        // Sort directories and files
        std::sort(directories.begin(), directories.end(),
            [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) { return a.path().filename().string() < b.path().filename().string(); }
        );
        std::sort(files.begin(), files.end(),
            [](const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b) { return a.path().filename().string() < b.path().filename().string(); }
        );

        std::string indent(depth * 2, ' ');

        // Display home file first if it exists
        if (!home_file.path().empty()) {
            std::string filename = home_file.path().filename().replace_extension("").string();
            std::replace(filename.begin(), filename.end(), '_', ' ');
            
            ImGui::TreeNodeEx((indent + filename).c_str(), ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemClicked())
                wiki_text = read_file_to_string(home_file.path());
            ImGui::TreePop();
        }

        // Display sorted directories
        for (const auto& dir_entry : directories) {
            if (ImGui::TreeNode((indent + dir_entry.path().filename().string()).c_str())) {
                display_directory_tree(dir_entry.path(), depth + 1);
                ImGui::TreePop();
            }
        }

        // Display sorted files
        for (const auto& file_entry : files) {
            std::string filename = file_entry.path().filename().replace_extension("").string();
            std::replace(filename.begin(), filename.end(), '_', ' ');

            ImGui::TreeNodeEx((indent + filename).c_str(), ImGuiTreeNodeFlags_Leaf);
            if (ImGui::IsItemClicked())
                wiki_text = read_file_to_string(file_entry.path());
            ImGui::TreePop();
        }
	}

	void engine_wiki_window() {

		if (!show_engine_wiki)
			return;

		if (wiki_text.empty())
			wiki_text = read_file_to_string(path_to_wiki / "home.md");
        
		ImGuiWindowFlags flags = 0;
		if (!ImGui::Begin(ENGINE_WIKI_WINDOW_NAME, &show_engine_wiki, flags)) {

			ImGui::End();
			return;
		}

		UI::custom_frame(200, [] {

			display_directory_tree(path_to_wiki);

		}, [] {

			ImGui::BeginChild("ayos�fdhg");
			UI::markdown(wiki_text);
			ImGui::EndChild();

		});

		ImGui::End();
	}

}
