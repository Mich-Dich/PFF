
#include "util/ui/pannel_collection.h"


namespace PFF::util {

	template <typename T>
	struct selected_items {

		T				main_item;
		std::set<T>		item_set;

		void reset() {

			item_set.clear();
			main_item = T();
		}

		inline bool empty() { return main_item.empty() && item_set.empty(); }
	};


	template <typename T>
	ImVec4& get_item_selection_color(util::selected_items<T>& selected_items, const T& item) {

		return (item == selected_items.main_item) ? UI::get_action_color_00_active_ref()
			: (selected_items.item_set.find(item) != selected_items.item_set.end()) ? UI::get_action_color_00_faded_ref()
			: UI::get_action_color_gray_hover_ref();
	}


	template <typename T>
	void process_selection_input(util::selected_items<T>& selected_items, const UI::mouse_interation interation, T item, const char* popup_name, std::function<void()> range_selection) {

		switch (interation) {
		// case UI::mouse_interation::left_double_clicked:				double_clicked(); break;

		case UI::mouse_interation::right_clicked: {

			if (selected_items.item_set.empty() && selected_items.main_item.empty())
				selected_items.main_item = item;
			
			else if (!selected_items.main_item.empty() && selected_items.main_item != item) {

				selected_items.item_set.insert(selected_items.main_item);
				selected_items.main_item = item;
			}
			ImGui::OpenPopup(popup_name);
		} break;

		case UI::mouse_interation::left_pressed:

			if (ImGui::GetIO().KeyShift) {

				if (!selected_items.main_item.empty()) {										// If main item selected -> perform range selection.

					range_selection();
				}
				else
					selected_items.main_item = item;										// If main item not selected -> simply mark clicked item as main selection.

			}
			else if (ImGui::GetIO().KeyCtrl) {


				if (!selected_items.main_item.empty())
					selected_items.item_set.insert(selected_items.main_item);
				selected_items.main_item = item;

			}
			else {

				selected_items.item_set.clear();
				selected_items.main_item = item;
			}
			break;

		default: break;
		}

	}

}
