#pragma once

namespace PFF::toolkit::todo {

	struct entry {

		std::string title;
		std::string description;
	};

	struct topic {

		std::string name;
		bool selected = false;
		bool hovered = false;
		std::vector<entry> tasks_open;
		std::vector<entry> tasks_done;
	};

	struct topic_buffer {

		bool input_enabled = false;
		char name[64] = "";

		void reset() {

			std::fill(std::begin(name), std::end(name), '\0');
			input_enabled = false;
		}
	};

	class todo_list {
	public:

		todo_list();
		~todo_list();

		void window_todo_list();
		void add_topic(topic_buffer buffer);
		void remove_topic(const int index);

		bool m_show_todo_list = false;

	private:
		std::vector<topic> m_topics{};
		u64 m_selected_topic = 0;
	};

	todo_list::todo_list() {

		m_topics.push_back({ "Test Topic", false, {}, {} });
		m_topics.push_back({ "SECOND Topic", false, {}, {} });
	}

	todo_list::~todo_list() {
	}


	void todo_list::window_todo_list() {

		if (!m_show_todo_list)
			return;

		ImGuiStyle* style = &ImGui::GetStyle();
		ImGuiWindowFlags window_flags{};
		const ImVec2 window_padding = { style->WindowPadding.x + 10.f, style->WindowPadding.y + 35.f };

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(viewport->Size.x - 500, viewport->Size.y - 300), ImGuiCond_Appearing);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		if (ImGui::Begin("ToDo List", &m_show_todo_list, window_flags)) {

			static topic_buffer topic_buf{};
			const f32 first_width = 220.f;
			const f32 inner_offset_x = 30.f;
			const ImVec2 button_size = { 50.f, ImGui::GetTextLineHeightWithSpacing() + style->FramePadding.y * 2 };

			UI::custom_frame(first_width, [&] {

				const f32 start_pos = ImGui::GetCursorPosX();
				const f32 inner_padding = 20.f;
				const f32 inner_width = first_width - (inner_padding * 2);

				PFF::UI::shift_cursor_pos(inner_offset_x, (button_size.y - ImGui::GetTextLineHeightWithSpacing()) / 2);
				PFF::UI::draw_big_text("Topics");

				ImGui::SameLine();
				ImGui::SetCursorPosX( start_pos + (first_width - button_size.x - inner_offset_x));
				PFF::UI::shift_cursor_pos(0, -(button_size.y - ImGui::GetTextLineHeightWithSpacing()) / 2);
				if (ImGui::Button("New##todo_add_topic_dialog", button_size))
					topic_buf.input_enabled = true;

				if (topic_buf.input_enabled) {

					ImVec2 text_size = ImGui::CalcTextSize("Add X ");
					const f32 textbox_width = inner_width - (text_size.x + (style->ItemSpacing.x * 2) + (style->FramePadding.x * 4) + 1);

					ImGui::BeginHorizontal("##Add_todo_dialog");

					PFF::UI::shift_cursor_pos(inner_padding, (button_size.y - ImGui::GetTextLineHeightWithSpacing()) / 2);
					ImGui::SetNextItemWidth(textbox_width);
					ImGui::InputText("##name_of_new_todo_topic", topic_buf.name, 64, ImGuiInputTextFlags_EnterReturnsTrue);
					
					if (ImGui::Button("Add##todo_add_topic")) {

						add_topic(topic_buf);
						topic_buf.reset();
					};

					if (ImGui::Button(" X ##cancle_todo_add_topic"))
						topic_buf.reset();

					ImGui::EndHorizontal();
				}

				UI::shift_cursor_pos(inner_padding, 10);

				ImGui::PushStyleColor(ImGuiCol_ChildBg, UI::THEME::highlited_window_bg);
				ImGui::BeginChild("Child##for_tpdp_topics", { inner_width, ImGui::GetContentRegionAvail().y - inner_padding }, true, 0);
				ImGui::PopStyleColor();

				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(.5f, .5f));
				for (u32 n = 0; n < m_topics.size(); n++) {

					const bool is_selected = (m_selected_topic == n);
					if (ImGui::Selectable(m_topics[n].name.c_str(), m_topics[n].selected, 0, { inner_width - inner_padding - style->ItemSpacing.x, 20})) {

						if (!ImGui::GetIO().KeyCtrl) {			// Clear selection when CTRL is not held
							for (u64 x = 0; x < m_topics.size(); x++)
								m_topics[x].selected = false;
						}
						m_topics[n].selected ^= 1;
					}

					char buf[32];
					sprintf(buf, "X##kill_%d", n);

					ImGui::SameLine();
					if (m_topics[n].hovered) {
						if (ImGui::Button(buf, { inner_padding, 22 }))
							remove_topic(n);
						else
							m_topics[n].hovered = ImGui::IsItemHovered();
					} else {
						ImGui::InvisibleButton(buf, { inner_padding, 22 });
						m_topics[n].hovered = ImGui::IsItemHovered();
					}
					

				}
				ImGui::PopStyleVar();

				ImGui::EndChild();
			}
			, [] {

				if (ImGui::CollapsingHeader("Open Items", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth)) {

					ImGui::Indent();
					for (u32 x = 0; x < 3; x++) {

						bool test = false;

						ImGui::Checkbox("##xx", &test);
						ImGui::SameLine();
						PFF::UI::draw_big_text("Test Title");
						ImGui::Text("test description of todo item");
					}
					ImGui::Unindent();
				}

				ImGui::Spacing();

				if (ImGui::CollapsingHeader("done Items")) {

					ImGui::Indent();
					for (u32 x = 0; x < 3; x++) {

						bool test = false;
						ImGui::Checkbox("##xx", &test);

						ImGui::SameLine();
						ImGui::BeginGroup();

						UI::draw_big_text("Test Title");
						ImGui::Text("test description of todo item");

						ImGui::EndGroup();

					}
					ImGui::Unindent();
				}

			});

		}
		ImGui::End();
		ImGui::PopStyleVar();
	}
	

	void todo_list::add_topic(topic_buffer buffer) {

		LOG(Trace, "Adding a new topic to ToDo-list");
		m_topics.push_back({ buffer.name, false, {}, {} });
		m_topics.push_back({ buffer.name, false, {}, {} });
		m_topics.push_back({ buffer.name, false, {}, {} });
		m_topics.push_back({ buffer.name, false, {}, {} });
		m_topics.push_back({ buffer.name, false, {}, {} });
		m_topics.push_back({ buffer.name, false, {}, {} });
	}

	void todo_list::remove_topic(const int index) {

		ASSERT(index >= 0 && index < m_topics.size(), "", "Tried to remove a nonvalid index")
		m_topics.erase(m_topics.begin() + index);
	}
}