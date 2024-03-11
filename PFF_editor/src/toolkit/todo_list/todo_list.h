#pragma once

namespace PFF::toolkit::todo {

#define CHAR_BUFFER_DEFAULT_SIZE 64

	// forward declareation
	class todo_list_serializer;

	// ============================================= serializer =============================================

	namespace buffer {

		struct topic {

			bool input_enabled = false;
			char name[CHAR_BUFFER_DEFAULT_SIZE] = "";

			void reset() {

				std::fill(std::begin(name), std::end(name), '\0');
				input_enabled = false;
			}
		};

		struct task {

			bool input_enabled = false;
			char name[CHAR_BUFFER_DEFAULT_SIZE] = "";
			char description[CHAR_BUFFER_DEFAULT_SIZE] = "";

			void reset() {

				std::fill(std::begin(name), std::end(name), '\0');
				std::fill(std::begin(description), std::end(description), '\0');
				input_enabled = false;
			}
		};
	}

	struct entry {

		std::string title{};
		std::string description{};
		bool done = false;
	};

	struct topic {

		std::string name{};
		bool selected = false;
		bool hovered = false;
		std::vector<entry> tasks{};
		
		bool add_task(const buffer::task& task) {

			if (strlen(task.name) <= 0)
				return false;

			tasks.push_back({task.name, task.description, false});
			return true;
		}
	};

	class todo_list {
	public:

		todo_list();
		~todo_list();

		void window_todo_list();
		void add_topic(buffer::topic buffer);
		void remove_topic(const u64 index);

		bool m_show_todo_list = false;			// DEV-ONLY

	private:
		std::vector<topic> m_topics{};

		friend class todo_list_serializer;
	};

	class todo_list_serializer {
	public:
		
		// can deine here because header is only included in editor_layer.cpp file
		todo_list_serializer(todo_list* todo_list, serializer::option option, const std::string& filename = "./config/todo_list.txt") {

			serializer::yaml(filename, "todo_list", option)
				.vector_of_structs(KEY_VALUE(todo_list->m_topics), [&](serializer::yaml& yaml, const u64 x) {

					yaml.entry(KEY_VALUE(todo_list->m_topics[x].name));
					yaml.vector_of_structs(KEY_VALUE(todo_list->m_topics[x].tasks), [&](serializer::yaml& inner, const u64 y) {

						inner.entry(KEY_VALUE(todo_list->m_topics[x].tasks[y].title));
						inner.entry(KEY_VALUE(todo_list->m_topics[x].tasks[y].description));
						inner.entry(KEY_VALUE(todo_list->m_topics[x].tasks[y].done));
						});
					yaml.entry(KEY_VALUE(todo_list->m_topics[x].selected));
				});

		}

		~todo_list_serializer() {}

	};

	// ============================================= implementation =============================================

	todo_list::todo_list() {

		todo_list_serializer(this, serializer::option::load_from_file);

		//m_topics.push_back({ "Editor", true, false, {{"Serialize ToDo-List", "Serialize Content into yaml-file for readability"}} });
		//m_topics.push_back({ "Renderer", false, false, {{"Test", "Descr"}} });
		//m_topics.push_back({ "Asset Manager", false, false, {{"Test", "Descr"}} });
		//m_topics.push_back({ "Procedural Generator", false, false, {{"Sub-mesh buffer for procedural generator", "Add a sub-mesh buffer array to procedural generator to enable fast copy past of already generated meshes (copy/past around axis to make turbine blades, along spline to make extruded mesh)"}} });
	}

	todo_list::~todo_list() {

		todo_list_serializer(this, serializer::option::save_to_file);
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
		ImGui::Begin("ToDo List", &m_show_todo_list, window_flags);

		static buffer::topic topic_buf{};
		static buffer::task task_buf{};
		const f32 first_width = 220.f;
		const f32 inner_offset_x = 30.f;
		const f32 inner_padding = 20.f;
		const ImVec2 button_size = { 50.f, ImGui::GetTextLineHeightWithSpacing() + style->FramePadding.y * 2 };
		const ImVec2 button_size_small = { 50.f, 21 };

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

				const ImVec2 text_size = ImGui::CalcTextSize("Add X ");
				const f32 textbox_width = inner_width - (text_size.x + (style->ItemSpacing.x * 2) + (style->FramePadding.x * 4) + 1);

				ImGui::BeginHorizontal("##Add_todo_dialog");

				PFF::UI::shift_cursor_pos(inner_padding, (button_size.y - ImGui::GetTextLineHeightWithSpacing()) / 2);
				ImGui::SetNextItemWidth(textbox_width);
				ImGui::InputText("##name_of_new_todo_topic", topic_buf.name, CHAR_BUFFER_DEFAULT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue);
					
				if (ImGui::Button("Add##todo_add_topic")) {

					add_topic(topic_buf);
					topic_buf.reset();
				};

				if (UI::add_gray_button(" X ##cancle_todo_add_topic"))
					topic_buf.reset();

				ImGui::EndHorizontal();
			}

			UI::shift_cursor_pos(inner_padding + 5, 10);

			ImGui::PushStyleColor(ImGuiCol_ChildBg, UI::THEME::highlited_window_bg);
			ImGui::BeginChild("Child##for_todo_topics", { inner_width + (inner_padding - 10), ImGui::GetContentRegionAvail().y - inner_padding }, true, 0);
			ImGui::PopStyleColor();

			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(.5f, .5f));
			for (u64 n = 0; n < m_topics.size(); n++) {

				if (ImGui::Selectable(m_topics[n].name.c_str(), m_topics[n].selected, 0, { inner_width - 10 - style->ItemSpacing.x, 20})) {

					if (!ImGui::GetIO().KeyCtrl) {			// Clear selection when CTRL is not held
						for (u64 x = 0; x < m_topics.size(); x++)
							m_topics[x].selected = false;
					}
					m_topics[n].selected ^= 1;
				}

				char buf[32];
				sprintf_s(buf, 32, "X##kill_%llu", n);

				ImGui::SameLine();
				if (m_topics[n].hovered) {

					if (UI::add_gray_button(buf, { inner_padding, 22 }))
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
		, [&] {

			//ImGui::PushStyleColor(ImGuiCol_ChildBg, UI::THEME::highlited_window_bg);
			ImGui::BeginChild("Child##for_todo_tasks");
			//ImGui::PopStyleColor();

			const ImVec2 start_pos = ImGui::GetCursorPos();
			UI::draw_big_text("Open Tasks");

			//ImGui::SameLine();
			ImGui::SetCursorPos(start_pos);
			//ImGui::SetCursorPosX(start_pos.x + (ImGui::GetColumnWidth() - button_size.x));
			PFF::UI::shift_cursor_pos((ImGui::GetColumnWidth() - button_size.x - 10), 0);
			if (ImGui::Button("New##add_new_todo_task", button_size))
				task_buf.input_enabled = true;

			char buf[32];

			UI::shift_cursor_pos(0, 10);
			for (u64 x = 0; x < m_topics.size(); x++) {

				if (m_topics[x].selected) {

					for (u64 y = 0; y < m_topics[x].tasks.size(); y++) {

						if (!m_topics[x].tasks[y].done) {

							sprintf_s(buf, 32, "##topic_%llutask_%llu", x, y);
							ImGui::Checkbox(buf, &m_topics[x].tasks[y].done);
							ImGui::SameLine();
							UI::draw_big_text(m_topics[x].tasks[y].title.c_str(), true);
							UI::shift_cursor_pos(inner_offset_x + 5, 0);
							ImGui::TextWrapped(m_topics[x].tasks[y].description.c_str());
						}
					}
				}
			}

			if (task_buf.input_enabled) {

				ImVec2 start_pos{};
				const f32 max_width = ImGui::GetColumnWidth();
				const ImVec2 input_size = { max_width - 60, 22 };
				//const ImVec2 button_size = { 50, 22 };
				static bool show_hint_title = true;
				static bool show_hint_descr = true;

				UI::shift_cursor_pos(0, 10);
				if (show_hint_title) {

					start_pos = ImGui::GetCursorPos();
					UI::shift_cursor_pos(5, 4);
					ImGui::Text("Title");
					ImGui::SetCursorPos(start_pos);
				}
				ImGui::InputTextEx("##new_task_title", NULL, task_buf.name, CHAR_BUFFER_DEFAULT_SIZE, input_size, 0);
				show_hint_title = strlen(task_buf.name) == 0;

				ImGui::SameLine();
				if (ImGui::Button("Add##new_task_to_todo", button_size_small)) {

					for (u64 x = 0; x < m_topics.size(); x++) {

						if (m_topics[x].selected) {

							m_topics[x].add_task(task_buf);
						}
					}
					task_buf.reset();
				}

				if (show_hint_descr) {

					start_pos = ImGui::GetCursorPos();
					UI::shift_cursor_pos(5, 4);
					ImGui::Text("Description");
					ImGui::SetCursorPos(start_pos);
				}
				ImGui::InputTextEx("##new_task_description", NULL, task_buf.description, CHAR_BUFFER_DEFAULT_SIZE, input_size, 0);
				show_hint_descr = strlen(task_buf.description) == 0;

				ImGui::SameLine();

				if (UI::add_gray_button(" X ##stop_adding_task_to_todo", button_size_small))
					task_buf.reset();
			}

			ImGui::Spacing();

			if (ImGui::CollapsingHeader("done Items")) {

				ImGui::Indent();
				for (u64 x = 0; x < m_topics.size(); x++) {

					if (m_topics[x].selected) {

						for (u64 y = 0; y < m_topics[x].tasks.size(); y++) {

							if (m_topics[x].tasks[y].done) {

								sprintf_s(buf, 32, "##topic_%llutask_%llu", x, y);
								ImGui::Checkbox(buf, &m_topics[x].tasks[y].done);
								ImGui::SameLine();
								UI::draw_big_text(m_topics[x].tasks[y].title.c_str());
								UI::shift_cursor_pos(inner_offset_x + 5, 0);
								ImGui::Text(m_topics[x].tasks[y].description.c_str());
							}
						}
					}
				}
				ImGui::Unindent();
			}

			ImGui::EndChild();
		});

		ImGui::End();
		ImGui::PopStyleVar();
	}

	void todo_list::add_topic(buffer::topic buffer) {

		LOG(Trace, "Adding a new topic to ToDo-list");
		m_topics.push_back({ buffer.name, false, false, {{"Test", "Descr"}} });
	}

	void todo_list::remove_topic(const u64 index) {

		ASSERT(index >= 0 && index < m_topics.size(), "", "Tried to remove a nonvalid index")
		m_topics.erase(m_topics.begin() + index);
	}

}

// , const std::string& filename = "testlog.txt"