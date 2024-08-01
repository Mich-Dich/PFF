
#include "util/pffpch.h"

#include "application.h"
#include <imgui.h>
#include <stdint.h>

#include "imgui_markdown.h"

namespace PFF::UI {

// ==================================================== DEBUGGING ====================================================
//#define MARKDOWN_LINK_DEBUG


// ====================================================================================================================================
// USER DATA
// ====================================================================================================================================

#define FONT_ITALLIC                application::get().get_imgui_layer()->get_font("italic")
#define FONT_BOLD                   application::get().get_imgui_layer()->get_font("bold")
#define FONT_HEADER_0               application::get().get_imgui_layer()->get_font("header_0")
#define FONT_HEADER_1               application::get().get_imgui_layer()->get_font("header_1")
#define FONT_HEADER_2               application::get().get_imgui_layer()->get_font("header_2")
#define FONT_HEADER_DEFAULT         application::get().get_imgui_layer()->get_font("header_default")

    const f32 INDENTATION_SPACING = 16;
    static const bool skip_empty_lines = true;



    // ====================================================================================================================================
    // DATA STRUCTS
    // ====================================================================================================================================
    
    struct link {

        link(u64 text_start = 0)
            : text_start(text_start), text_end(text_start) {}

        u64     text_start = 0, text_end = 0;
        u64     content_start = 0, content_end = 0;
        void*   user_data{};
    };

    struct markdown_context {

        markdown_context(const char* markdown_text, size_t markdown_length)
            : markdown_text(markdown_text), markdown_length(markdown_length) {}

        const char* markdown_text;

        size_t  markdown_length;
        u64     indentation_last_line = 0;
        u64     indentation = 0;
        u64     line_start = 0;
        u64     line_end = 0;
    };

    // ImGui::TextWrapped will wrap at the starting position
    // so to work around this we render using our own wrapping for the first line
    FORCEINLINE static void render_text_wrapped(const char* text, const char* text_end) {

        float scale = ImGui::GetIO().FontGlobalScale;
        float width_left = ImGui::GetContentRegionAvail().x;
        const char* endLine = ImGui::GetFont()->CalcWordWrapPositionA(scale, text, text_end, width_left);
        ImGui::TextUnformatted(text, endLine);
        width_left = ImGui::GetContentRegionAvail().x;
        while (endLine < text_end) {

            text = endLine;
            if (*text == ' ')      // skip a space at start of line
                ++text;

            endLine = ImGui::GetFont()->CalcWordWrapPositionA(scale, text, text_end, width_left);
            if (text == endLine)
                endLine++;

            ImGui::TextUnformatted(text, endLine);
        }
    }

#define VALIDATE_IF_POINTER_AT_TARGET_CHAR(pointer, target_char)                                                                                                                    \
    if (markdown_context.markdown_text[pointer] != target_char)                                                                                                                     \
        return;


#define MOVE_POINTER_TO_TARGET_CHAR(pointer, target_char)                                                                                                                           \
    while (markdown_context.markdown_text[pointer] != target_char && markdown_context.markdown_text[pointer] != '\n' && start_position < markdown_context.markdown_length)          \
        pointer++;                                                                                                                                                                  \
    if (markdown_context.markdown_text[pointer] != target_char)                                                                                                                     \
        return

    FORCEINLINE static void check_for_link(markdown_context& markdown_context, u64& start_position) {

        link loc_link(start_position + 1);

        // ------------------------------------ locate link text ------------------------------------
        MOVE_POINTER_TO_TARGET_CHAR(loc_link.text_end, ']');

        // ------------------------------------ locate link content ------------------------------------
        VALIDATE_IF_POINTER_AT_TARGET_CHAR(loc_link.text_end + 1, '(')
        loc_link.content_start = loc_link.content_end = loc_link.text_end + 2;
        MOVE_POINTER_TO_TARGET_CHAR(loc_link.content_end, ')');

#ifdef MARKDOWN_LINK_DEBUG
        ImGui::Text("TEXT: [");
        ImGui::SameLine();

        render_text_wrapped(markdown_context.markdown_text + markdown_context.line_start + markdown_context.indentation, markdown_context.markdown_text + loc_link.text_start-1);

        ImGui::SameLine();
        ImGui::Text("] LINK-TEXT: [");
        ImGui::SameLine();

        render_text_wrapped(markdown_context.markdown_text + loc_link.text_start, markdown_context.markdown_text + loc_link.text_end);

        ImGui::SameLine();
        ImGui::Text("] LINK-CONTENT: [");
        ImGui::SameLine();

        render_text_wrapped(markdown_context.markdown_text + loc_link.content_start, markdown_context.markdown_text + loc_link.content_end);

        ImGui::SameLine();
        ImGui::Text("]");
#else
        render_text_wrapped(markdown_context.markdown_text + markdown_context.line_start + markdown_context.indentation, markdown_context.markdown_text + loc_link.text_start - 1);
        ImGui::SameLine();
        render_text_wrapped(markdown_context.markdown_text + loc_link.text_start, markdown_context.markdown_text + loc_link.text_end);
        ImGui::SameLine();
#endif

        markdown_context.line_start = loc_link.content_end;
        start_position = loc_link.content_end;
    }

    // Text `highlight` Text
    FORCEINLINE static void check_for_highlight(markdown_context& markdown_context, u64& start_position) {

        u64 emphasis_start = start_position;

        u64 emphasis_end = start_position + 1;
        MOVE_POINTER_TO_TARGET_CHAR(emphasis_end, '`');

        render_text_wrapped(markdown_context.markdown_text + markdown_context.line_start + markdown_context.indentation, markdown_context.markdown_text + emphasis_start);
        ImGui::SameLine();

        ImGui::PushFont(FONT_ITALLIC);
        render_text_wrapped(markdown_context.markdown_text + emphasis_start + 1, markdown_context.markdown_text + emphasis_end);
        ImGui::SameLine();
        ImGui::PopFont();

        markdown_context.line_start = emphasis_end + 1;
        start_position = emphasis_end + 1;
    }

    FORCEINLINE static void check_for_emphasis(markdown_context& markdown_context, u64& start_position) {

        u64 emphasis_start = start_position;
        VALIDATE_IF_POINTER_AT_TARGET_CHAR(emphasis_start + 1, '*');                    // emphasis needs 2 starts
        
        u64 emphasis_end = start_position + 2;
        MOVE_POINTER_TO_TARGET_CHAR(emphasis_end, '*');
        VALIDATE_IF_POINTER_AT_TARGET_CHAR(emphasis_end + 1, '*');                      // emphasis needs 2 starts


        render_text_wrapped(markdown_context.markdown_text + markdown_context.line_start + markdown_context.indentation, markdown_context.markdown_text + emphasis_start);
        ImGui::SameLine();
        
        ImGui::PushFont(FONT_BOLD);
        render_text_wrapped(markdown_context.markdown_text + emphasis_start +2, markdown_context.markdown_text + emphasis_end);
        ImGui::SameLine();
        ImGui::PopFont();

        markdown_context.line_start = emphasis_end + 2;
        start_position = emphasis_end +2;
    }


    // =================================================================================================================================================
    // MAIN FUNCTION
    // =================================================================================================================================================

    void markdown(const std::string_view& markdown_text) {

        markdown(markdown_text.data(), markdown_text.size());
    }

    // render markdown
    void markdown(const char* markdown_text, size_t markdown_length) {

        u32 indentation_last_line = 0;
        markdown_context loc_context{ markdown_text, markdown_length };
        char c = 0;

        ImGui::BeginGroup();

        for (u64 x = 0; x < (u64)markdown_length; x++) {

            loc_context.line_start = x;

            c = markdown_text[x];          // get the character at index
            if (c == 0) { break; }         // shouldn't happen but don't go beyond 0.

            loc_context.indentation = 0;

            {   // ------------------------------ handle indentation ------------------------------ 
                while (markdown_text[x] == '\t' || markdown_text[x] == ' ') {

                    x++;
                    loc_context.indentation++;
                }

                //ImGui::Text("indentation: %d                      ", loc_context.indentation);
                //ImGui::SameLine();

                if (loc_context.indentation > loc_context.indentation_last_line)
                    ImGui::Indent((f32)(loc_context.indentation - loc_context.indentation_last_line) * INDENTATION_SPACING);
                else if (loc_context.indentation_last_line > loc_context.indentation)
                    ImGui::Unindent((f32)(loc_context.indentation_last_line - loc_context.indentation) * INDENTATION_SPACING);

                loc_context.indentation_last_line = loc_context.indentation;
            }

            switch (markdown_text[x]) {         // check start of line
                case '#': {                     // line is a header
                    u32 heading_counter = 1;
                    while (markdown_text[++x] == '#' && heading_counter <= 3)
                        heading_counter++;
                                        
                    while (markdown_text[x] != '\n' && x < markdown_length)     // move pointer to end of line
                        x++;

                    ImGui::NewLine();

                    switch (heading_counter) {
                    case 1:
                        ImGui::PushFont(FONT_HEADER_0);
                        break;
                    case 2:
                        ImGui::PushFont(FONT_HEADER_1);
                        break;
                    case 3:
                        ImGui::PushFont(FONT_HEADER_2);
                        break;
                    default:
                        ImGui::PushFont(FONT_HEADER_DEFAULT);
                        break;
                    }

                    render_text_wrapped(markdown_text + loc_context.line_start + loc_context.indentation + heading_counter, markdown_text + x);
                    ImGui::Separator();
                    ImGui::PopFont();
                    continue;
                } break;

                case '-': {                     // Line beginns with bullet point
                    loc_context.line_start++;
                    //x++;
                    ImGui::Bullet();
                    ImGui::SameLine();
                }break;

                case '\n':
                    if (skip_empty_lines) {

                        //x++;
                        continue;
                    }
                break;

                default: break;
            }

            while (markdown_text[x] != '\n' && x < markdown_length) {
                switch (markdown_text[x]) {                                 // check for spetial characters in line

                    case '[':                                               // check if '[' starts a link
                        check_for_link(loc_context, x);
                        break;

                    //case '`':                                               // check if '*' is used as emphasis
                    //    check_for_highlight(loc_context, x);
                    //    break;

                    case '*':                                               // check if '*' is used as emphasis
                        check_for_emphasis(loc_context, x);
                        break;

                    default: break;
                }
                x++;
            }

            // normal text
            render_text_wrapped(markdown_text + loc_context.line_start + loc_context.indentation, markdown_text + x);
        }

        ImGui::EndGroup();
    }

}
