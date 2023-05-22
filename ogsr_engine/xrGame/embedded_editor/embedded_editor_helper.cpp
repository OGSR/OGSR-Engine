#include "stdafx.h"

#include "embedded_editor_helper.h"

bool ImGui_ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, const ImVec2& size_arg)
{
    if (!ImGui::ListBoxHeader(label, size_arg))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different or variable sizes you can
    // create a custom version of ListBox() in your code without using the clipper.
    ImGuiListClipper clipper; // We know exactly our line height
                              // here so we pass it as a minor
                              // optimization, but generally you
                              // don't need to.

    clipper.ItemsCount = -1;
    clipper.Begin(items_count, ImGui::GetTextLineHeightWithSpacing());

    bool value_changed = false;

    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const bool item_selected = (i == *current_item);
            const char* item_text;
            if (!items_getter(data, i, &item_text))
                item_text = "*Unknown item*";

            ImGui::PushID(i);
            if (ImGui::Selectable(item_text, item_selected))
            {
                *current_item = i;
                value_changed = true;
            }
            ImGui::PopID();
        }
    }

    ImGui::ListBoxFooter();

    return value_changed;
}