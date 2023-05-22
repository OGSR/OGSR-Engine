#pragma once

#include "imgui.h"

struct ImguiWnd
{
    ImguiWnd(const char* name, bool* pOpen = nullptr) { Collapsed = !ImGui::Begin(name, pOpen); }
    ~ImguiWnd() { ImGui::End(); }

    bool Collapsed;
};

bool ImGui_ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, const ImVec2& size_arg = ImVec2(0, 0));