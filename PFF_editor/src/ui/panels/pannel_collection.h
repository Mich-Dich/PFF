#pragma once

#include "util/pffpch.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>


namespace PFF {


	template<typename T>
	void display_colum(const std::string& lable, T value) {

		ImGui::Columns(2, nullptr, true);

		ImGui::Text("%s", lable.c_str());


		ImGui::NextColumn();
		ImGui::SetNextItemWidth(ImGui::GetColumnWidth());

		if constexpr (std::is_same_v<T, bool>)
			ImGui::Text("%s", bool_to_str(value));

		else if constexpr (std::is_arithmetic_v<T>)
			ImGui::Text("%s", std::to_string(value).c_str());
			
		else if constexpr (std::is_convertible_v<T, std::string>)
			ImGui::Text("%s", std::to_string(value).c_str());

		else if constexpr (std::is_same_v<T, glm::vec2>)
			ImGui::InputFloat2("", &value[0], "%.2f");

		else if constexpr (std::is_same_v<T, glm::vec3>)
			ImGui::InputFloat3("", &value[0], "%.2f");

		else if constexpr (std::is_same_v<T, glm::vec4> || std::is_same_v<T, ImVec4>)
			ImGui::InputFloat4("", &value[0] - 1, "%.2f", ImGuiInputTextFlags_None, ImVec2(columnWidth, 0));

		ImGui::Columns(1);
	}

}

/*
// Flags for ImGui::InputText()
// (Those are per-item flags. There are shared flags in ImGuiIO: io.ConfigInputTextCursorBlink and io.ConfigInputTextEnterKeepActive)
enum ImGuiInputTextFlags_ {
    ImGuiInputTextFlags_None = 0,
    ImGuiInputTextFlags_CharsDecimal = 1 << 0,   // Allow 0123456789.+-
ImGuiInputTextFlags_CharsHexadecimal = 1 << 1,   // Allow 0123456789ABCDEFabcdef
ImGuiInputTextFlags_CharsUppercase = 1 << 2,   // Turn a..z into A..Z
ImGuiInputTextFlags_CharsNoBlank = 1 << 3,   // Filter out spaces, tabs
ImGuiInputTextFlags_AutoSelectAll = 1 << 4,   // Select entire text when first taking mouse focus
ImGuiInputTextFlags_EnterReturnsTrue = 1 << 5,   // Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider looking at the IsItemDeactivatedAfterEdit() function.
ImGuiInputTextFlags_CallbackCompletion = 1 << 6,   // Callback on pressing TAB (for completion handling)
ImGuiInputTextFlags_CallbackHistory = 1 << 7,   // Callback on pressing Up/Down arrows (for history handling)
ImGuiInputTextFlags_CallbackAlways = 1 << 8,   // Callback on each iteration. User code may query cursor position, modify text buffer.
ImGuiInputTextFlags_CallbackCharFilter = 1 << 9,   // Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
ImGuiInputTextFlags_AllowTabInput = 1 << 10,  // Pressing TAB input a '\t' character into the text field
ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,  // In multi-line mode, unfocus with Enter, add new line with Ctrl+Enter (default is opposite: unfocus with Ctrl+Enter, add line with Enter).
ImGuiInputTextFlags_NoHorizontalScroll = 1 << 12,  // Disable following the cursor horizontally
ImGuiInputTextFlags_AlwaysOverwrite = 1 << 13,  // Overwrite mode
ImGuiInputTextFlags_ReadOnly = 1 << 14,  // Read-only mode
ImGuiInputTextFlags_Password = 1 << 15,  // Password mode, display all characters as '*'
ImGuiInputTextFlags_NoUndoRedo = 1 << 16,  // Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
ImGuiInputTextFlags_CharsScientific = 1 << 17,  // Allow 0123456789.+- eE (Scientific notation input)
ImGuiInputTextFlags_CallbackResize = 1 << 18,  // Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
ImGuiInputTextFlags_CallbackEdit = 1 << 19,  // Callback on any edit (note that InputText() already returns true on edit, the callback is useful mainly to manipulate the underlying buffer while focus is active)
ImGuiInputTextFlags_EscapeClearsAll = 1 << 20,  // Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)

// Obsolete names
//ImGuiInputTextFlags_AlwaysInsertMode  = ImGuiInputTextFlags_AlwaysOverwrite   // [renamed in 1.82] name was not matching behavior
}; */