#include "stdafx.h"

#include "embedded_editor_helper.h"

bool ImGui_ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, const ImVec2& size_arg)
{
    if (!ImGui::ListBoxHeader(label, size_arg))
        return false;

    ImGuiListClipper clipper;

    clipper.ItemsCount = -1;
    clipper.Begin(items_count, ImGui::GetTextLineHeightWithSpacing());

    bool value_changed = false;

    const bool scrollTo = ImGui::IsWindowAppearing();

    if (scrollTo)
        clipper.ForceDisplayRangeByIndices(*current_item - 5, *current_item + 5);

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
            if (scrollTo && item_selected)
                ImGui::SetScrollHereY(0.5f);
            ImGui::PopID();
        }
    }

    ImGui::ListBoxFooter();

    return value_changed;
}