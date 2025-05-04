
#include "util/pffpch.h"

#include "application.h"
#include "util/io/config.h"
#include "util/io/serializer_yaml.h"

#include "input_action.h"
#include "input_mapping.h"

namespace PFF {

	// forward declaration

	std::string key_code_to_string(key_code code);
	key_code string_to_key_code(const std::string& str);

	input_mapping::input_mapping() {

		PFF_PROFILE_FUNCTION();

		m_actions.clear();
	}

	void input_mapping::serialize_action(input_action* action, bool force_override, std::filesystem::path path) {

		auto serializer_option = force_override ? serializer::option::save_to_file : serializer::option::load_from_file;

		serializer::yaml(config::get_filepath_from_configtype(path, config::file::input), action->m_name, serializer_option)
			.entry(KEY_VALUE(action->triger_when_paused))
			.entry(KEY_VALUE(action->duration_in_sec))
			.entry(KEY_VALUE(action->value))
			.vector(KEY_VALUE(action->keys_bindings), [&](serializer::yaml& yaml, u64 x) {

			std::string key_name = key_code_to_string(action->keys_bindings[x].key);
			yaml.entry(KEY_VALUE(key_name));
			action->keys_bindings[x].key = string_to_key_code(key_name);

			yaml.entry(KEY_VALUE(action->keys_bindings[x].trigger_flags));
			yaml.entry(KEY_VALUE(action->keys_bindings[x].modefier_flags));
			});

	}

	void input_mapping::register_action(input_action* action, bool force_override) {

		PFF_PROFILE_FUNCTION();

		serialize_action(action, force_override, application::get().get_project_path());
		m_actions.emplace_back(action);
	}

	void input_mapping::register_action(input_action* action, bool force_override, std::filesystem::path path) {

		PFF_PROFILE_FUNCTION();

		serialize_action(action, force_override, path);
		m_actions.emplace_back(action);
	}


	static const std::unordered_map<key_code, std::string> key_map{
		{key_code::mouse_bu_1,				"mouse_bu_1"},
		{key_code::mouse_bu_2,				"mouse_bu_2"},
		{key_code::mouse_bu_3,				"mouse_bu_3"},
		{key_code::mouse_bu_4,				"mouse_bu_4"},
		{key_code::mouse_bu_5,				"mouse_bu_5"},
		{key_code::mouse_bu_6,				"mouse_bu_6"},
		{key_code::mouse_bu_7,				"mouse_bu_7"},
		{key_code::mouse_bu_8,				"mouse_bu_8"},
		{key_code::mouse_bu_last,			"mouse_bu_last"},
		{key_code::mouse_bu_left,			"mouse_bu_left"},
		{key_code::mouse_bu_right,			"mouse_bu_right"},
		{key_code::mouse_bu_middle,			"mouse_bu_middle"},
		{key_code::mouse_moved,				"mouse_moved"},
		{key_code::mouse_moved_x,			"mouse_moved_x"},
		{key_code::mouse_moved_y,			"mouse_moved_y"},
		{key_code::mouse_scrolled_x,		"mouse_scrolled_x"},
		{key_code::mouse_scrolled_y,		"mouse_scrolled_y"},
		{key_code::key_unknown,				"key_unknown"},
		{key_code::key_space,				"key_space"},
		{key_code::key_apostrophe,			"key_apostrophe"},
		{key_code::key_comma,				"key_comma"},
		{key_code::key_minus,				"key_minus"},
		{key_code::key_period,				"key_period"},
		{key_code::key_slash,				"key_slash"},
		{key_code::key_0,					"key_0"},
		{key_code::key_1,					"key_1"},
		{key_code::key_2,					"key_2"},
		{key_code::key_3,					"key_3"},
		{key_code::key_4,					"key_4"},
		{key_code::key_5,					"key_5"},
		{key_code::key_6,					"key_6"},
		{key_code::key_7,					"key_7"},
		{key_code::key_8,					"key_8"},
		{key_code::key_9,					"key_9"},
		{key_code::key_semicolon,			"key_semicolon"},
		{key_code::key_equal,				"key_equal"},
		{key_code::key_A,					"key_A"},
		{key_code::key_B,					"key_B"},
		{key_code::key_C,					"key_C"},
		{key_code::key_D,					"key_D"},
		{key_code::key_E,					"key_E"},
		{key_code::key_F,					"key_F"},
		{key_code::key_G,					"key_G"},
		{key_code::key_H,					"key_H"},
		{key_code::key_I,					"key_I"},
		{key_code::key_J,					"key_J"},
		{key_code::key_K,					"key_K"},
		{key_code::key_L,					"key_L"},
		{key_code::key_M,					"key_M"},
		{key_code::key_N,					"key_N"},
		{key_code::key_O,					"key_O"},
		{key_code::key_P,					"key_P"},
		{key_code::key_Q,					"key_Q"},
		{key_code::key_R,					"key_R"},
		{key_code::key_S,					"key_S"},
		{key_code::key_T,					"key_T"},
		{key_code::key_U,					"key_U"},
		{key_code::key_V,					"key_V"},
		{key_code::key_W,					"key_W"},
		{key_code::key_X,					"key_X"},
		{key_code::key_Y,					"key_Y"},
		{key_code::key_Z,					"key_Z"},
		{key_code::key_left_bracket,		"key_left_bracket"},
		{key_code::key_backslach,			"key_backslach"},
		{key_code::key_right_bracket,		"key_right_bracket"},
		{key_code::key_grave_accent,		"key_grave_accent"},
		{key_code::key_world_1,				"key_world_1"},
		{key_code::key_world_2,				"key_world_2"},
		{key_code::key_escape,				"key_escape"},
		{key_code::key_enter,				"key_enter"},
		{key_code::key_tab,					"key_tab"},
		{key_code::key_backspace,			"key_backspace"},
		{key_code::key_insert,				"key_insert"},
		{key_code::key_delete,				"key_delete"},
		{key_code::key_right,				"key_right"},
		{key_code::key_left,				"key_left"},
		{key_code::key_down,				"key_down"},
		{key_code::key_up,					"key_up"},
		{key_code::key_page_up,				"key_page_up"},
		{key_code::key_page_down,			"key_page_down"},
		{key_code::key_home,				"key_home"},
		{key_code::key_end,					"key_end"},
		{key_code::key_caps_lock,			"key_caps_lock"},
		{key_code::key_scroll_lock,			"key_scroll_lock"},
		{key_code::key_num_lock,			"key_num_lock"},
		{key_code::key_print_screen,		"key_print_screen"},
		{key_code::key_pause,				"key_pause"},
		{key_code::key_F1,					"key_F1"},
		{key_code::key_F2,					"key_F2"},
		{key_code::key_F3,					"key_F3"},
		{key_code::key_F4,					"key_F4"},
		{key_code::key_F5,					"key_F5"},
		{key_code::key_F6,					"key_F6"},
		{key_code::key_F7,					"key_F7"},
		{key_code::key_F8,					"key_F8"},
		{key_code::key_F9,					"key_F9"},
		{key_code::key_F10,					"key_F10"},
		{key_code::key_F11,					"key_F11"},
		{key_code::key_F12,					"key_F12"},
		{key_code::key_F13,					"key_F13"},
		{key_code::key_F14,					"key_F14"},
		{key_code::key_F15,					"key_F15"},
		{key_code::key_F16,					"key_F16"},
		{key_code::key_F17,					"key_F17"},
		{key_code::key_F18,					"key_F18"},
		{key_code::key_F19,					"key_F19"},
		{key_code::key_F20,					"key_F20"},
		{key_code::key_F21,					"key_F21"},
		{key_code::key_F22,					"key_F22"},
		{key_code::key_F23,					"key_F23"},
		{key_code::key_F24,					"key_F24"},
		{key_code::key_F25,					"key_F25"},
		{key_code::key_kp_0,				"key_kp_0"},
		{key_code::key_kp_1,				"key_kp_1"},
		{key_code::key_kp_2,				"key_kp_2"},
		{key_code::key_kp_3,				"key_kp_3"},
		{key_code::key_kp_4,				"key_kp_4"},
		{key_code::key_kp_5,				"key_kp_5"},
		{key_code::key_kp_6,				"key_kp_6"},
		{key_code::key_kp_7,				"key_kp_7"},
		{key_code::key_kp_8,				"key_kp_8"},
		{key_code::key_kp_9,				"key_kp_9"},
		{key_code::key_kp_decimal,			"key_kp_decimal"},
		{key_code::key_kp_divide,			"key_kp_divide"},
		{key_code::key_kp_multiply,			"key_kp_multiply"},
		{key_code::key_kp_subtrace,			"key_kp_subtrace"},
		{key_code::key_kp_add,				"key_kp_add"},
		{key_code::key_kp_enter,			"key_kp_enter"},
		{key_code::key_kp_equal,			"key_kp_equal"},
		{key_code::key_left_shift,			"key_left_shift"},
		{key_code::key_left_control,		"key_left_control"},
		{key_code::key_left_alt,			"key_left_alt"},
		{key_code::key_left_super,			"key_left_super"},
		{key_code::key_right_shift,			"key_right_shift"},
		{key_code::key_right_control,		"key_right_control"},
		{key_code::key_right_alt,			"key_right_alt"},
		{key_code::key_right_super,			"key_right_super"},
		{key_code::key_menu,				"key_menu"},
	};

	std::string key_code_to_string(key_code code) {

		auto it = key_map.find(code);
		if (it != key_map.end())
			return it->second;
		else
			return "Unknown";

	}

	key_code string_to_key_code(const std::string& str) {

		for (const auto& pair : key_map) {
			if (pair.second == str)
				return pair.first;
		}
		return key_code::key_unknown;
	}

}
