
#include "util/pffpch.h"

#include "application.h"

#include "imgui_markdown.h"

namespace PFF::UI {

    //-----------------------------------------------------------------------------
    // Basic types
    //-----------------------------------------------------------------------------

    struct link;
    struct markdown_config;

    struct markdown_link_callback_data { // for both links and images

        const char* text;                  // text between square brackets []
        int                             textLength;
        const char* link;                   // text between brackets ()
        int                             link_length;
        void* userData;
        bool                            is_image;               // true if '!' is detected in front of the link syntax
    };

    struct markdown_tooltip_callback_data { // for tooltips

        markdown_link_callback_data     link_data;
        const char* link_icon;
    };

    struct markdown_image_data {

        bool                            is_valid = false;                    // if true, will draw the image
        bool                            use_link_callback = false;            // if true, linkCallback will be called when image is clicked
        ImTextureID                     user_texture_id = 0;                // see ImGui::Image
        ImVec2                          size = ImVec2(100.0f, 100.0f);    // see ImGui::Image
        ImVec2                          uv0 = ImVec2(0, 0);               // see ImGui::Image
        ImVec2                          uv1 = ImVec2(1, 1);               // see ImGui::Image
        ImVec4                          tint_col = ImVec4(1, 1, 1, 1);    // see ImGui::Image
        ImVec4                          border_col = ImVec4(0, 0, 0, 0);  // see ImGui::Image
    };

    enum class markdown_format_type {
        normal_text,
        header,
        unordered_list,
        link,
        emphasis,
    };

    struct markdown_format_info {

        markdown_format_type            type = markdown_format_type::normal_text;
        int32                           level = 0;                                  // Set for headings: 1 for H1, 2 for H2 etc.
        bool                            item_hovered = false;                       // Currently only set for links when mouse hovered, only valid when start_ == false
        const markdown_config*          config = NULL;
    };

    typedef void                markdown_link_callback(markdown_link_callback_data data);
    typedef void                markdown_tooltip_callback(markdown_tooltip_callback_data data);

    FORCEINLINE void default_markdown_tooltip_callback(markdown_tooltip_callback_data data) {

        if (data.link_data.is_image)
            ImGui::SetTooltip("%.*s", data.link_data.link_length, data.link_data.link);
        else
            ImGui::SetTooltip("%s Open in browser\n%.*s", data.link_icon, data.link_data.link_length, data.link_data.link);
    }

    typedef markdown_image_data         markdown_image_callback(markdown_link_callback_data data);
    typedef void                        markdown_formal_callback(const markdown_format_info& markdown_format_info, bool start);

    FORCEINLINE void default_markdown_format_callback(const markdown_format_info& markdown_format_info, bool start);

    struct markdown_heading_format {

        ImFont*                         font;                               // ImGui font
        bool                            separator;                          // if true, an underlined separator is drawn after the header
    };

    // Configuration struct for Markdown
    // - linkCallback is called when a link is clicked on
    // - linkIcon is a string which encode a "Link" icon, if available in the current font (e.g. linkIcon = ICON_FA_LINK with FontAwesome + IconFontCppHeaders https://github.com/juliettef/IconFontCppHeaders)
    // - headingFormats controls the format of heading H1 to H3, those above H3 use H3 format
    struct markdown_config {

        static const int                NUM_HEADINGS = 3;
        markdown_link_callback* link_callback = NULL;
        markdown_tooltip_callback* tooltip_callback = NULL;
        markdown_image_callback* image_callback = NULL;
        const char* link_icon = "";                      // icon displayd in link tooltip
        markdown_heading_format         heading_formats[NUM_HEADINGS] = { { NULL, true }, { NULL, true }, { NULL, true } };
        void* user_data = NULL;
        markdown_formal_callback* format_callback = default_markdown_format_callback;
    };


    struct text_region;
    struct line;

    struct text_region {

        text_region() : indent_X(0.0f) {}
        ~text_region() { reset_indent(); }

        // ImGui::TextWrapped will wrap at the starting position
        // so to work around this we render using our own wrapping for the first line
        void render_text_wrapped(const char* text, const char* text_end, bool indent_to_here = false) {

            float scale = ImGui::GetIO().FontGlobalScale;
            float width_left = ImGui::GetContentRegionAvail().x;
            const char* endLine = ImGui::GetFont()->CalcWordWrapPositionA(scale, text, text_end, width_left);

            ImGui::TextUnformatted(text, endLine);
            if (indent_to_here) {
                float indentNeeded = ImGui::GetContentRegionAvail().x - width_left;
                if (indentNeeded) {
                    ImGui::Indent(indentNeeded);
                    indent_X += indentNeeded;
                }
            }
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

        void render_list_text_wrapped(const char* text, const char* text_end) {

            ImGui::Bullet();
            ImGui::SameLine();
            render_text_wrapped(text, text_end, true);
        }

        bool render_link_text(const char* text, const char* text_end, const link& link, const char* markdown, const markdown_config& md_config, const char** link_hover_start);

        void render_link_text_wrapped(const char* text, const char* text_end, const link& link, const char* markdown, const markdown_config& md_config, const char** link_hover_start, bool indent_to_here = false);

        void reset_indent() {
            if (indent_X > 0.0f)
                ImGui::Unindent(indent_X);

            indent_X = 0.0f;
        }

    private:
        float       indent_X;

    };

    // Text that starts after a new line (or at beginning) and ends with a newline (or at end)
    struct line {

        bool is_heading = false;
        bool is_emphasis = false;
        bool is_unordered_list_start = false;
        bool is_leading_space = true;     // spaces at start of line
        int  lead_space_count = 0;
        int  heading_count = 0;
        int  emphasis_count = 0;
        int  line_start = 0;
        int  line_end = 0;
        int  last_render_position = 0;     // lines may get rendered in multiple pieces
    };

    struct text_block {                  // subset of line

        int start = 0;
        int stop = 0;
        int size() const { return stop - start; }
    };

    struct link {

        enum link_state {
            no_link,
            has_sqare_bracket_open,
            has_sqare_brackets,
            has_sqare_brackets_round_bracket_open,
        };
        link_state          state = no_link;
        text_block          text;
        text_block          url;
        bool                is_image = false;
        int                 num_brackets_open = 0;
    };

    struct emphasis {

        enum emphasis_state {
            none,
            left,
            middle,
            right,
        };
        emphasis_state      state = none;
        text_block          text;
        char                sym;
    };

    //-----------------------------------------------------------------------------
    // Implementation
    //-----------------------------------------------------------------------------

    FORCEINLINE void under_line(ImColor color) {

        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        min.y = max.y;
        ImGui::GetWindowDrawList()->AddLine(min, max, color, 1.0f);
    }

    FORCEINLINE void render_line(const char* markdown_, line& line_, text_region& text_region, const markdown_config& mdConfig_) {
        // indent
        int indentStart = 0;
        if (line_.is_unordered_list_start)    // ImGui unordered list render always adds one indent
            indentStart = 1;

        for (int j = indentStart; j < line_.lead_space_count / 2; ++j)    // add indents
            ImGui::Indent();

        // render
        markdown_format_info formatInfo;
        formatInfo.config = &mdConfig_;
        int textStart = line_.last_render_position + 1;
        int textSize = line_.line_end - textStart;
        if (line_.is_unordered_list_start) {    // render unordered list

            formatInfo.type = markdown_format_type::unordered_list;
            mdConfig_.format_callback(formatInfo, true);
            const char* text = markdown_ + textStart + 1;
            text_region.render_list_text_wrapped(text, text + textSize - 1);

        } else if (line_.is_heading) {          // render heading

            formatInfo.level = line_.heading_count;
            formatInfo.type = markdown_format_type::header;
            mdConfig_.format_callback(formatInfo, true);
            const char* text = markdown_ + textStart + 1;
            text_region.render_text_wrapped(text, text + textSize - 1);

        } else if (line_.is_emphasis) {         // render emphasis

            formatInfo.level = line_.emphasis_count;
            formatInfo.type = markdown_format_type::emphasis;
            mdConfig_.format_callback(formatInfo, true);
            const char* text = markdown_ + textStart;
            text_region.render_text_wrapped(text, text + textSize);

        } else {                                // render a normal paragraph chunk

            formatInfo.type = markdown_format_type::normal_text;
            mdConfig_.format_callback(formatInfo, true);
            const char* text = markdown_ + textStart;
            text_region.render_text_wrapped(text, text + textSize);
        }
        mdConfig_.format_callback(formatInfo, false);

        // unindent
        for (int j = indentStart; j < line_.lead_space_count / 2; ++j)
            ImGui::Unindent();
    }

    FORCEINLINE bool text_region::render_link_text(const char* text, const char* text_end, const link& link, const char* markdown, const markdown_config& md_config, const char** link_hover_start) {

        markdown_format_info format_info;
        format_info.config = &md_config;
        format_info.type = markdown_format_type::link;
        md_config.format_callback(format_info, true);
        ImGui::PushTextWrapPos(-1.0f);
        ImGui::TextUnformatted(text, text_end);
        ImGui::PopTextWrapPos();

        bool is_this_item_hovered = ImGui::IsItemHovered();
        if (is_this_item_hovered)
            *link_hover_start = markdown + link.text.start;

        bool is_hovered = is_this_item_hovered || (*link_hover_start == (markdown + link.text.start));
        format_info.item_hovered = is_hovered;
        md_config.format_callback(format_info, false);

        if (is_hovered) {
            if (ImGui::IsMouseReleased(0) && md_config.link_callback)
                md_config.link_callback({ markdown + link.text.start, link.text.size(), markdown + link.url.start, link.url.size(), md_config.user_data, false });

            if (md_config.tooltip_callback)
                md_config.tooltip_callback({ { markdown + link.text.start, link.text.size(), markdown + link.url.start, link.url.size(), md_config.user_data, false }, md_config.link_icon });
        }
        return is_this_item_hovered;
    }

    // IsCharInsideWord based on ImGui's CalcWordWrapPositionA
    FORCEINLINE bool is_char_inside_word(char c) { return c != ' ' && c != '.' && c != ',' && c != ';' && c != '!' && c != '?' && c != '\"'; }

    FORCEINLINE void text_region::render_link_text_wrapped(const char* text, const char* text_end, const link& link, const char* markdown, const markdown_config& md_config, const char** link_hover_start, bool indent_to_here) {

        float scale = ImGui::GetIO().FontGlobalScale;
        float widthLeft = ImGui::GetContentRegionAvail().x;
        const char* end_line = text;
        if (widthLeft > 0.0f)
            end_line = ImGui::GetFont()->CalcWordWrapPositionA(scale, text, text_end, widthLeft);

        if (end_line > text && end_line < text_end) {
            if (is_char_inside_word(*end_line)) { // see if we can do a better cut.
                float widthNextLine = widthLeft + ImGui::GetCursorScreenPos().x - ImGui::GetWindowPos().x; // was GetContentRegionMax().x on IMGUI_VERSION_NUM < 19099
                const char* endNextLine = ImGui::GetFont()->CalcWordWrapPositionA(scale, text, text_end, widthNextLine);
                if (endNextLine == text_end || (endNextLine <= text_end && !is_char_inside_word(*endNextLine)))     // can possibly do better if go to next line
                    end_line = text;
            }
        }

        bool is_hovered = render_link_text(text, end_line, link, markdown, md_config, link_hover_start);
        if (indent_to_here) {
            float indentNeeded = ImGui::GetContentRegionAvail().x - widthLeft;
            if (indentNeeded) {
                ImGui::Indent(indentNeeded);
                indent_X += indentNeeded;
            }
        }

        widthLeft = ImGui::GetContentRegionAvail().x;
        while (end_line < text_end) {
            text = end_line;
            if (*text == ' ')   // skip a space at start of line
                ++text;

            end_line = ImGui::GetFont()->CalcWordWrapPositionA(scale, text, text_end, widthLeft);
            if (text == end_line)
                end_line++;

            bool is_this_line_hovered = render_link_text(text, end_line, link, markdown, md_config, link_hover_start);
            is_hovered = is_hovered || is_this_line_hovered;
        }

        if (!is_hovered && *link_hover_start == markdown + link.text.start)
            *link_hover_start = NULL;

    }


    FORCEINLINE void default_markdown_format_callback(const markdown_format_info& markdown_format_info, bool start) {

        switch (markdown_format_info.type) {
        case markdown_format_type::normal_text:
            break;

        case markdown_format_type::emphasis: {

            if (markdown_format_info.level == 1) {

                // normal emphasis
                if (start)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_TextDisabled]);
                else
                    ImGui::PopStyleColor();

            } else {

                // strong emphasis
                if (start) {
                    ImGui::PushFont(application::get().get_imgui_layer()->get_font("bold"));

                } else {
                    ImGui::PopFont();
                }
            }
            break;
        }

        case markdown_format_type::header: {
            
            markdown_heading_format fmt;
            if (markdown_format_info.level > markdown_config::NUM_HEADINGS)
                fmt = markdown_format_info.config->heading_formats[markdown_config::NUM_HEADINGS - 1];
            else
                fmt = markdown_format_info.config->heading_formats[markdown_format_info.level - 1];

            if (start) {

                ImGui::PushFont(application::get().get_imgui_layer()->get_font("regular_big"));
                ImGui::NewLine();

            } else {
                if (fmt.separator) {
                    ImGui::Separator();
                } else
                    ImGui::NewLine();

                ImGui::PopFont();
            }
            break;
        }
        case markdown_format_type::unordered_list:
            break;

        case markdown_format_type::link:
            if (start)
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);

            else {
                ImGui::PopStyleColor();
                if (markdown_format_info.item_hovered)
                    under_line(ImGui::GetStyle().Colors[ImGuiCol_ButtonHovered]);
                else
                    under_line(ImGui::GetStyle().Colors[ImGuiCol_Button]);
            }
            break;
        }
    }



    // =================================================================================================================================================
    // MAIN FUNCTION
	// =================================================================================================================================================

    void markdown(const std::string& markdown_text) {
        
        markdown(markdown_text.c_str(), markdown_text.size());
    }

    // render markdown
    void markdown(const char* markdown_text, size_t markdown_length) {

        markdown_config md_config{};
        static const char* linkHoverStart = NULL; // we need to preserve status of link hovering between frames
        ImGuiStyle& style = ImGui::GetStyle();
        line line;
        link link;
        emphasis em;
        text_region textRegion;

        ImGui::BeginGroup();

        char c = 0;
        for (int i = 0; i < (int)markdown_length; ++i) {
            c = markdown_text[i];               // get the character at index
            if (c == 0) { break; }         // shouldn't happen but don't go beyond 0.

            // If we're at the beginning of the line, count any spaces
            if (line.is_leading_space) {
                if (c == ' ') {
                    ++line.lead_space_count;
                    continue;

                } else {
                    line.is_leading_space = false;
                    line.last_render_position = i - 1;
                    if ((c == '*') && (line.lead_space_count >= 2)) {
                        if (((int)markdown_length > i + 1) && (markdown_text[i + 1] == ' ')) {    // space after '*'

                            line.is_unordered_list_start = true;
                            ++i;
                            ++line.last_render_position;
                        }
                        // carry on processing as could be emphasis
                    } else if (c == '#') {
                        line.heading_count++;
                        bool bContinueChecking = true;
                        int j = i;
                        while (++j < (int)markdown_length && bContinueChecking) {
                            c = markdown_text[j];
                            switch (c) {
                            case '#':
                                line.heading_count++;
                                break;

                            case ' ':
                                line.last_render_position = j - 1;
                                i = j;
                                line.is_heading = true;
                                bContinueChecking = false;
                                break;

                            default:
                                line.is_heading = false;
                                bContinueChecking = false;
                                break;
                            }
                        }
                        if (line.is_heading) {

                            em = emphasis();    // reset emphasis status, does not support emphasis around headers for now
                            continue;
                        }
                    }
                }
            }

            // Test to see if we have a link
            switch (link.state) {
            case link::no_link:
                if (c == '[' && !line.is_heading) { // we do not support headings with links for now

                    link.state = link::has_sqare_bracket_open;
                    link.text.start = i + 1;
                    if (i > 0 && markdown_text[i - 1] == '!')
                        link.is_image = true;

                } break;

            case link::has_sqare_bracket_open:
                if (c == ']') {
                    link.state = link::has_sqare_brackets;
                    link.text.stop = i;
                } break;

            case link::has_sqare_brackets:
                if (c == '(') {
                    link.state = link::has_sqare_brackets_round_bracket_open;
                    link.url.start = i + 1;
                    link.num_brackets_open = 1;
                } break;

            case link::has_sqare_brackets_round_bracket_open:
                if (c == '(')
                    ++link.num_brackets_open;

                else if (c == ')')
                    --link.num_brackets_open;

                if (link.num_brackets_open == 0) {
                    // reset emphasis status, we do not support emphasis around links for now
                    em = emphasis();
                    // render previous line content
                    line.line_end = link.text.start - (link.is_image ? 2 : 1);
                    render_line(markdown_text, line, textRegion, md_config);
                    line.lead_space_count = 0;
                    link.url.stop = i;
                    line.is_unordered_list_start = false;    // the following text shouldn't have bullets
                    ImGui::SameLine(0.0f, 0.0f);
                    if (link.is_image)   // it's an image, render it.
                    {
                        bool drawnImage = false;
                        bool useLinkCallback = false;
                        if (md_config.image_callback) {
                            markdown_image_data imageData = md_config.image_callback({ markdown_text + link.text.start, link.text.size(), markdown_text + link.url.start, link.url.size(), md_config.user_data, true });
                            useLinkCallback = imageData.use_link_callback;
                            if (imageData.is_valid) {
                                ImGui::Image(imageData.user_texture_id, imageData.size, imageData.uv0, imageData.uv1, imageData.tint_col, imageData.border_col);
                                drawnImage = true;
                            }
                        }
                        if (!drawnImage)
                            ImGui::Text("( Image %.*s not loaded )", link.url.size(), markdown_text + link.url.start);

                        if (ImGui::IsItemHovered()) {
                            if (ImGui::IsMouseReleased(0) && md_config.link_callback && useLinkCallback)
                                md_config.link_callback({ markdown_text + link.text.start, link.text.size(), markdown_text + link.url.start, link.url.size(), md_config.user_data, true });

                            if (link.text.size() > 0 && md_config.tooltip_callback)
                                md_config.tooltip_callback({ { markdown_text + link.text.start, link.text.size(), markdown_text + link.url.start, link.url.size(), md_config.user_data, true }, md_config.link_icon });

                        }
                    } else                 // it's a link, render it.
                    {
                        textRegion.render_link_text_wrapped(markdown_text + link.text.start, markdown_text + link.text.start + link.text.size(), link, markdown_text, md_config, &linkHoverStart, false);
                    }
                    ImGui::SameLine(0.0f, 0.0f);
                    // reset the link by reinitializing it
                    link = PFF::UI::link();
                    line.last_render_position = i;
                    break;
                }
            }

            // Test to see if we have emphasis styling
            switch (em.state) {
            case emphasis::none:
                if (link.state == link::no_link && !line.is_heading) {
                    int next = i + 1;
                    int prev = i - 1;
                    if ((c == '*' || c == '_')
                        && (i == line.line_start
                        || markdown_text[prev] == ' '
                        || markdown_text[prev] == '\t') // empasis must be preceded by whitespace or line start
                        && (int)markdown_length > next // emphasis must precede non-whitespace
                        && markdown_text[next] != ' '
                        && markdown_text[next] != '\n'
                        && markdown_text[next] != '\t') {
                        em.state = emphasis::left;
                        em.sym = c;
                        em.text.start = i;
                        line.emphasis_count = 1;
                        continue;
                    }
                } break;

            case emphasis::left:
                if (em.sym == c) {
                    ++line.emphasis_count;
                    continue;
                } else {
                    em.text.start = i;
                    em.state = emphasis::middle;
                } break;

            case emphasis::middle:
                if (em.sym == c) {
                    em.state = emphasis::right;
                    em.text.stop = i;
                    // pass through to case Emphasis::RIGHT
                } else {
                    break;
                }

            case emphasis::right:
                if (em.sym == c) {
                    if (line.emphasis_count < 3 && (i - em.text.stop + 1 == line.emphasis_count)) {
                        // render text up to emphasis
                        int lineEnd = em.text.start - line.emphasis_count;
                        if (lineEnd > line.line_start) {
                            line.line_end = lineEnd;
                            render_line(markdown_text, line, textRegion, md_config);
                            ImGui::SameLine(0.0f, 0.0f);
                            line.is_unordered_list_start = false;
                            line.lead_space_count = 0;
                        }
                        line.is_emphasis = true;
                        line.last_render_position = em.text.start - 1;
                        line.line_start = em.text.start;
                        line.line_end = em.text.stop;
                        render_line(markdown_text, line, textRegion, md_config);
                        ImGui::SameLine(0.0f, 0.0f);
                        line.is_emphasis = false;
                        line.last_render_position = i;
                        em = emphasis();
                    }
                    continue;
                } else {
                    em.state = emphasis::none;
                    // render text up to here
                    int start = em.text.start - line.emphasis_count;
                    if (start < line.line_start) {
                        line.line_end = line.line_start;
                        line.line_start = start;
                        line.last_render_position = start - 1;
                        render_line(markdown_text, line, textRegion, md_config);
                        line.line_start = line.line_end;
                        line.last_render_position = line.line_start - 1;
                    }
                } break;
            }

            // handle end of line (render)
            if (c == '\n') {
                // first check if the line is a horizontal rule
                line.line_end = i;
                if (em.state == emphasis::middle && line.emphasis_count >= 3 &&
                    (line.line_start + line.emphasis_count) == i) {
                    ImGui::Separator();
                } else
                    render_line(markdown_text, line, textRegion, md_config);   // render the line: multiline emphasis requires a complex implementation so not supported

                // reset the line and emphasis state
                line = PFF::UI::line();
                em = PFF::UI::emphasis();
                line.line_start = i + 1;
                line.last_render_position = i;
                textRegion.reset_indent();
                link = PFF::UI::link();
            }
        }

        if (em.state == emphasis::left && line.emphasis_count >= 3)
            ImGui::Separator();

        else {
            // render any remaining text if last char wasn't 0
            if (markdown_length && line.line_start < (int)markdown_length && markdown_text[line.line_start] != 0) {
                // handle both null terminated and non null terminated strings
                line.line_end = (int)markdown_length;
                if (0 == markdown_text[line.line_end - 1])
                    --line.line_end;

                render_line(markdown_text, line, textRegion, md_config);
            }
        }
        
        ImGui::EndGroup();

    }

}
