#include "stdafx.h"

#include "imgui.h"
#include "embedded_editor_settings.h"
#include <array>

static void SetupImGuiRoseQuartzStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Soft & Round) ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 5.0f);
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    // --- 2. Borders & Rounding (Maximum Softness) ---
    style.WindowRounding = 10.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    // --- 3. The Rose Quartz Palette ---

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.90f, 0.95f, 1.00f); // Soft white-pink
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.45f, 0.55f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.10f, 0.12f, 1.00f); // Deep Plum-Grey
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.08f, 0.10f, 0.96f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.35f, 0.25f, 0.35f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.22f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.28f, 0.40f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.15f, 0.10f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.15f, 0.25f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.06f, 0.08f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.10f, 0.15f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.06f, 0.08f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.25f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.70f, 0.45f, 0.70f, 1.00f);

    // Interactables (The Rose Pop)
    colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.60f, 0.75f, 1.00f); // Rose Pink
    colors[ImGuiCol_SliderGrab] = ImVec4(0.85f, 0.50f, 0.65f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.95f, 0.60f, 0.75f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.45f, 0.25f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.65f, 0.35f, 0.50f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.45f, 0.65f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.45f, 0.25f, 0.35f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.30f, 0.45f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.35f, 0.55f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.20f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.65f, 0.35f, 0.50f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.45f, 0.25f, 0.35f, 1.00f);

    // Misc
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.95f, 0.60f, 0.75f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.95f, 0.60f, 0.75f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.95f, 0.60f, 0.75f, 0.40f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.12f, 0.10f, 0.12f, 1.00f);
#endif
}

static void SetupForestGreenStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.CellPadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    // --- 2. Borders ---
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.TabBorderSize = 0.0f;

    // --- 3. Rounding ---
    style.WindowRounding = 6.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.LogSliderDeadzone = 4.0f;
    style.TabRounding = 4.0f;

    // --- 4. Full Color Palette ---

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.85f, 0.90f, 0.85f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.55f, 0.50f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.09f, 0.06f, 1.00f); // Deep pine
    colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.11f, 0.08f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.10f, 0.07f, 0.96f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.18f, 0.28f, 0.18f, 0.80f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Inputs, Checkboxes, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.18f, 0.12f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.30f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.42f, 0.24f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.14f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.26f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.08f, 0.05f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.14f, 0.09f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.08f, 0.05f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.18f, 0.28f, 0.18f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.38f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.32f, 0.48f, 0.32f, 1.00f);

    // Interactables
    colors[ImGuiCol_CheckMark] = ImVec4(0.45f, 0.75f, 0.45f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.35f, 0.55f, 0.35f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.70f, 0.45f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.35f, 0.18f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.45f, 0.25f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.32f, 0.55f, 0.32f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.35f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.45f, 0.25f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.32f, 0.55f, 0.32f, 1.00f);

    // Separators and Resizing
    colors[ImGuiCol_Separator] = ImVec4(0.18f, 0.28f, 0.18f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.45f, 0.25f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.32f, 0.55f, 0.32f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.35f, 0.18f, 0.80f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.45f, 0.25f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.32f, 0.55f, 0.32f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.22f, 0.12f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.45f, 0.25f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.38f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.15f, 0.08f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.22f, 0.12f, 1.00f);

    // Plots
    colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.70f, 0.40f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.50f, 0.85f, 0.50f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.70f, 0.40f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.50f, 0.85f, 0.50f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.12f, 0.22f, 0.12f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.35f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.15f, 0.25f, 0.15f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.08f, 0.14f, 0.08f, 0.50f);

    // Misc
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.55f, 0.25f, 0.50f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.60f, 0.90f, 0.60f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.40f, 0.80f, 0.40f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.85f, 0.90f, 0.85f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.10f, 0.15f, 0.10f, 0.50f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.05f, 0.08f, 0.05f, 0.60f);

#ifdef IMGUI_HAS_DOCK
    // Docking (If using the docking branch)
    colors[ImGuiCol_DockingPreview] = ImVec4(0.25f, 0.55f, 0.25f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.06f, 0.09f, 0.06f, 1.00f);
#endif
}

static void SetupImGuiCyberpunkStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Sharp & Aggressive) ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ScrollbarSize = 13.0f;
    style.GrabMinSize = 10.0f;

    // --- 2. Borders & Rounding (Cyberpunk = Hard Edges) ---
    style.WindowRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;

    // --- 3. The Neon Palette ---
    // Background: Pitch Black / Deep Navy
    // Neon Cyan: #00ff9f | Neon Pink: #ff003f | Neon Yellow: #fcee0a

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 0.62f, 1.00f); // Neon Green/Cyan
    colors[ImGuiCol_TextDisabled] = ImVec4(0.20f, 0.40f, 0.35f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.02f, 0.02f, 0.04f, 1.00f); // Near black
    colors[ImGuiCol_ChildBg] = ImVec4(0.02f, 0.02f, 0.04f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.02f, 0.02f, 0.04f, 0.98f);

    // Borders (The "Glow" look)
    colors[ImGuiCol_Border] = ImVec4(1.00f, 0.00f, 0.25f, 0.60f); // Neon Pink Border
    colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 0.00f, 0.25f, 0.20f);

    // Frames
    colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 0.00f, 0.25f, 0.20f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(1.00f, 0.00f, 0.25f, 0.40f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.02f, 0.02f, 0.04f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.02f, 0.02f, 0.04f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.04f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(1.00f, 0.93f, 0.04f, 0.60f); // Neon Yellow
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.93f, 0.04f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1.00f, 0.93f, 0.04f, 1.00f);

    // Interactables
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.93f, 0.04f, 1.00f); // Yellow
    colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.25f, 0.80f); // Pink
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.00f, 0.25f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.00f, 1.00f, 0.62f, 0.20f); // Cyan Ghost
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 1.00f, 0.62f, 0.50f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 0.62f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(1.00f, 0.00f, 0.25f, 0.30f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.25f, 0.50f);
    colors[ImGuiCol_HeaderActive] = ImVec4(1.00f, 0.00f, 0.25f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.05f, 0.05f, 0.10f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.00f, 0.25f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.80f, 0.00f, 0.20f, 1.00f);

    // Misc
    colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 0.93f, 0.04f, 0.30f);
    colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.25f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.00f, 1.00f, 0.62f, 0.40f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.02f, 0.02f, 0.04f, 1.00f);
#endif
}

static void SetupImGuiCrimsonVesuviusStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Sleek & Sharp) ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(5.0f, 3.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ScrollbarSize = 13.0f;
    style.GrabMinSize = 10.0f;

    // --- 2. Borders & Rounding ---
    style.WindowRounding = 3.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 3.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    // --- 3. The Crimson Palette ---

    // Text
    colors[ImGuiCol_Text] = ImVec4(1.00f, 0.90f, 0.90f, 1.00f); // Slight pinkish tint to off-white
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.40f, 0.40f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f); // Deep charcoal
    colors[ImGuiCol_ChildBg] = ImVec4(0.10f, 0.09f, 0.09f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.07f, 0.07f, 0.96f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.15f, 0.15f, 0.80f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.20f, 0.20f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.08f, 0.08f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.20f, 0.20f, 1.00f);

    // Interactables (The High-Intensity Red)
    colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.15f, 0.15f, 1.00f); // Sharp Red
    colors[ImGuiCol_SliderGrab] = ImVec4(0.60f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.30f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.30f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.70f, 0.25f, 0.25f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.10f, 0.10f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.50f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.12f, 0.12f, 1.00f);

    // Misc
    colors[ImGuiCol_PlotLines] = ImVec4(0.85f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.85f, 0.15f, 0.15f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.85f, 0.15f, 0.15f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.15f, 0.15f, 0.40f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.07f, 0.07f, 1.00f);
#endif
}

static void SetupImGuiGruvboxHardStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Industrial & Square) ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    // --- 2. Borders & Rounding (Gruvbox usually looks best with sharp or low rounding) ---
    style.WindowRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 2.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;

    // --- 3. The Gruvbox Dark Hard Palette ---
    // Background: #1d2021 (Dark Hard) | Foreground: #ebdbb2
    // Red: #fb4934 | Green: #b8bb26 | Yellow: #fabd2f | Blue: #83a598
    // Gray: #928374 | Orange: #fe8019

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.86f, 0.70f, 1.00f); // #ebdbb2
    colors[ImGuiCol_TextDisabled] = ImVec4(0.57f, 0.51f, 0.45f, 1.00f); // #928374

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f); // #1d2021
    colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.13f, 0.13f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.13f, 0.13f, 0.95f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f); // #504945
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames
    colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f); // #3c3836
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f); // #504945
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.36f, 0.33f, 1.00f); // #665c54

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.14f, 0.13f, 1.00f); // #282828

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.36f, 0.33f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.57f, 0.51f, 0.45f, 1.00f);

    // Interactables
    colors[ImGuiCol_CheckMark] = ImVec4(0.72f, 0.73f, 0.15f, 1.00f); // #b8bb26 (Green)
    colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.65f, 0.60f, 1.00f); // #83a598 (Blue)
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.73f, 0.67f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.98f, 0.29f, 0.20f, 1.00f); // #fb4934 (Red)
    colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.20f, 0.15f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.40f, 0.36f, 0.33f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.24f, 0.22f, 0.21f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);

    // Misc
    colors[ImGuiCol_PlotLines] = ImVec4(0.98f, 0.74f, 0.18f, 1.00f); // #fabd2f (Yellow)
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.31f, 0.29f, 0.27f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.98f, 0.29f, 0.20f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.72f, 0.73f, 0.15f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
#endif
}

static void SetupImGuiCatppuccinMochaStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Soft & Modern) ---
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    // --- 2. Borders & Rounding ---
    style.WindowRounding = 8.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 5.0f;
    style.TabRounding = 5.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f; // Minimalist look
    style.PopupBorderSize = 1.0f;

    // --- 3. The Catppuccin Mocha Palette ---
    // Base: #1e1e2e | Mantle: #181825 | Crust: #11111b
    // Text: #cdd6f4 | Subtext0: #a6adc8 | Surface0: #313244
    // Lavender: #b4befe | Sapphire: #74c7ec | Mauve: #cba6f7

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.80f, 0.84f, 0.96f, 1.00f); // Text
    colors[ImGuiCol_TextDisabled] = ImVec4(0.42f, 0.45f, 0.55f, 1.00f); // Surface1

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f); // Base
    colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f); // Mantle
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.11f, 0.96f); // Crust

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.19f, 0.20f, 0.27f, 1.00f); // Surface0
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Inputs, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.19f, 0.20f, 0.27f, 1.00f); // Surface0
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.26f, 0.35f, 1.00f); // Surface1
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.31f, 0.32f, 0.42f, 1.00f); // Surface2

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f); // Mantle
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f); // Base
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.07f, 0.11f, 1.00f); // Crust

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.32f, 0.42f, 1.00f); // Surface2
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.37f, 0.38f, 0.51f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.42f, 0.45f, 0.55f, 1.00f);

    // Interactables
    colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.75f, 1.00f, 1.00f); // Lavender
    colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.78f, 0.93f, 1.00f); // Sapphire
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.78f, 0.93f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.19f, 0.20f, 0.27f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.80f, 0.65f, 0.97f, 1.00f); // Mauve
    colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.55f, 0.87f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.19f, 0.20f, 0.27f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.26f, 0.35f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.32f, 0.42f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.31f, 0.32f, 0.42f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.20f, 0.27f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);

    // Misc
    colors[ImGuiCol_PlotLines] = ImVec4(0.94f, 0.72f, 0.42f, 1.00f); // Marigold
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.31f, 0.32f, 0.42f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.71f, 0.75f, 1.00f, 1.00f); // Lavender

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.71f, 0.75f, 1.00f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
#endif
}

static void SetupImGuiDraculaStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Clean & Balanced) ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 6.0f);
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;

    // --- 2. Borders & Rounding ---
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 4.0f;
    style.TabRounding = 4.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    // --- 3. The Dracula Color Palette ---
    // Background: #282a36 | Selection: #44475a | Foreground: #f8f8f2
    // Comment: #6272a4    | Cyan: #8be9fd      | Green: #50fa7b
    // Orange: #ffb86c     | Pink: #ff79c6      | Purple: #bd93f9
    // Red: #ff5555        | Yellow: #f1fa8c

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.97f, 0.97f, 0.95f, 1.00f); // #f8f8f2
    colors[ImGuiCol_TextDisabled] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f); // #6272a4

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f); // #282a36
    colors[ImGuiCol_ChildBg] = ImVec4(0.16f, 0.16f, 0.21f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.16f, 0.21f, 0.96f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f); // #44475a
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Inputs, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f); // #44475a
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f); // #6272a4
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.48f, 0.55f, 0.74f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f); // Darker
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.48f, 0.55f, 0.74f, 1.00f);

    // Interactables
    colors[ImGuiCol_CheckMark] = ImVec4(0.31f, 0.98f, 0.48f, 1.00f); // #50fa7b (Green)
    colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.58f, 0.98f, 1.00f); // #bd93f9 (Purple)
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.84f, 0.68f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.47f, 0.78f, 1.00f); // #ff79c6 (Pink)
    colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.37f, 0.62f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.55f, 0.74f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.38f, 0.45f, 0.64f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);

    // Misc
    colors[ImGuiCol_PlotLines] = ImVec4(0.55f, 0.91f, 0.99f, 1.00f); // #8be9fd (Cyan)
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.27f, 0.28f, 0.35f, 1.00f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.74f, 0.58f, 0.98f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.74f, 0.58f, 0.98f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.16f, 0.16f, 0.21f, 1.00f);
#endif
}

static void SetupImGuiAmberYellowStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Sharp & Technical) ---
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(5.0f, 3.0f);
    style.CellPadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 10.0f;

    // --- 2. Borders & Rounding (Low rounding for a technical look) ---
    style.WindowRounding = 2.0f;
    style.ChildRounding = 2.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 2.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    // --- 3. Color Palette ---

    // Text
    colors[ImGuiCol_Text] = ImVec4(1.00f, 0.95f, 0.80f, 1.00f); // Soft cream-yellow
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.45f, 0.30f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.06f, 1.00f); // Near black
    colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.08f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.06f, 0.96f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.30f, 0.25f, 0.10f, 0.80f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Inputs, Checkboxes, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.14f, 0.10f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.22f, 0.12f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.30f, 0.15f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.11f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.18f, 0.10f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.05f, 0.04f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.11f, 0.08f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.04f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.30f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.40f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.50f, 0.20f, 1.00f);

    // Interactables (The High-Vis Amber)
    colors[ImGuiCol_CheckMark] = ImVec4(0.95f, 0.80f, 0.10f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.60f, 0.10f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.95f, 0.80f, 0.10f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.30f, 0.25f, 0.05f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.45f, 0.38f, 0.10f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.60f, 0.50f, 0.15f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.30f, 0.25f, 0.05f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.38f, 0.10f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.60f, 0.50f, 0.15f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.14f, 0.10f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.38f, 0.10f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.30f, 0.10f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.07f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.14f, 0.10f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.16f, 0.10f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.35f, 0.30f, 0.15f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.25f, 0.20f, 0.10f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

    // Misc
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.95f, 0.80f, 0.10f, 0.25f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 0.85f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.95f, 0.80f, 0.10f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.95f, 0.80f, 0.10f, 0.40f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.07f, 0.07f, 0.06f, 1.00f);
#endif
}

static void SetupImGuiAmethystStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing (Modern & Tight) ---
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(5.0f, 3.0f);
    style.CellPadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(6.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style.ScrollbarSize = 13.0f;
    style.GrabMinSize = 10.0f;

    // --- 2. Borders & Rounding ---
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    style.WindowRounding = 4.0f;
    style.ChildRounding = 3.0f;
    style.FrameRounding = 3.0f;
    style.PopupRounding = 3.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 3.0f;

    // --- 3. Color Palette ---

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.90f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.50f, 0.60f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.07f, 0.12f, 1.00f); // Deep charcoal-purple
    colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.09f, 0.14f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.07f, 0.12f, 0.96f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.20f, 0.35f, 0.80f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Inputs, Checkboxes, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.12f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.20f, 0.38f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.25f, 0.55f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.09f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.14f, 0.32f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.07f, 0.05f, 0.10f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.09f, 0.18f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.05f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.20f, 0.35f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.30f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.40f, 0.65f, 1.00f);

    // Interactables (The "Pop" colors)
    colors[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.45f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.35f, 0.75f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.65f, 0.45f, 0.95f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.20f, 0.40f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.28f, 0.62f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.35f, 0.80f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.20f, 0.40f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.28f, 0.62f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.35f, 0.80f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.12f, 0.25f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.38f, 0.28f, 0.62f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.20f, 0.45f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.12f, 0.25f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.15f, 0.28f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.25f, 0.20f, 0.40f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.20f, 0.15f, 0.30f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);

    // Misc
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.50f, 0.35f, 0.80f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.80f, 0.65f, 1.00f, 0.95f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.45f, 0.90f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.50f, 0.35f, 0.80f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.09f, 0.07f, 0.12f, 1.00f);
#endif
}

static void SetupImGuiSapphireStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // --- 1. Sizing and Spacing ---
    style.WindowPadding = ImVec2(10.0f, 10.0f);
    style.FramePadding = ImVec2(6.0f, 4.0f);
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ScrollbarSize = 15.0f;
    style.GrabMinSize = 10.0f;

    // --- 2. Borders & Rounding ---
    style.WindowRounding = 5.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 12.0f;
    style.GrabRounding = 3.0f;
    style.TabRounding = 4.0f;

    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;

    // --- 3. Color Palette ---

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.90f, 0.93f, 0.97f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.50f, 0.65f, 1.00f);

    // Backgrounds
    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.09f, 0.12f, 1.00f); // Deep midnight
    colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.09f, 0.12f, 0.95f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.15f, 0.25f, 0.35f, 0.70f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames (Inputs, Checkboxes, etc.)
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.18f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.28f, 0.40f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.38f, 0.55f, 1.00f);

    // Title Bars
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.14f, 0.22f, 0.35f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.08f, 0.12f, 1.00f);

    // Menus
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.16f, 0.22f, 1.00f);

    // Scrollbars
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.08f, 0.11f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.32f, 0.48f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.42f, 0.60f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.50f, 0.75f, 1.00f);

    // Interactables
    colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.55f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.45f, 0.75f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.18f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.48f, 0.75f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.60f, 0.90f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.48f, 0.75f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.60f, 0.90f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.20f, 0.32f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.45f, 0.70f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.20f, 0.32f, 1.00f);

    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.25f, 0.40f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.20f, 0.35f, 0.55f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.15f, 0.25f, 0.40f, 1.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.05f);

    // Misc
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.55f, 0.85f, 0.40f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.50f, 0.80f, 1.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);

#ifdef IMGUI_HAS_DOCK
    colors[ImGuiCol_DockingPreview] = ImVec4(0.25f, 0.50f, 0.80f, 0.50f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.07f, 0.09f, 0.12f, 1.00f);
#endif
}

enum ogsr_imgui_style : u32
{
    ogsr_imgui_style_Dark,
    ogsr_imgui_style_Classic,
    ogsr_imgui_style_RoseQuartz,
    ogsr_imgui_style_ForestGreen,
    ogsr_imgui_style_Cyberpunk,
    ogsr_imgui_style_CrimsonVesuvius,
    ogsr_imgui_style_GruvboxHard,
    ogsr_imgui_style_CatppuccinMocha,
    ogsr_imgui_style_Dracula,
    ogsr_imgui_style_AmberYellow,
    ogsr_imgui_style_Amethyst,
    ogsr_imgui_style_Sapphire,
    _ogsr_imgui_styles_count_
};

constexpr std::array<const char*, _ogsr_imgui_styles_count_> ogsr_imgui_style_names{"Dark",        "Classic",         "RoseQuartz", "ForestGreen", "Cyberpunk", "CrimsonVesuvius",
                                                                                   "GruvboxHard", "CatppuccinMocha", "Dracula",    "AmberYellow", "Amethyst",  "Sapphire"};

void SetupStyle(const u32 style_idx)
{
    switch (style_idx)
    {
    case ogsr_imgui_style_Dark: ImGui::StyleColorsDark(); break;
    case ogsr_imgui_style_Classic: ImGui::StyleColorsClassic(); break;
    case ogsr_imgui_style_RoseQuartz: SetupImGuiRoseQuartzStyle(); break;
    case ogsr_imgui_style_ForestGreen: SetupForestGreenStyle(); break;
    case ogsr_imgui_style_Cyberpunk: SetupImGuiCyberpunkStyle(); break;
    case ogsr_imgui_style_CrimsonVesuvius: SetupImGuiCrimsonVesuviusStyle(); break;
    case ogsr_imgui_style_GruvboxHard: SetupImGuiGruvboxHardStyle(); break;
    case ogsr_imgui_style_CatppuccinMocha: SetupImGuiCatppuccinMochaStyle(); break;
    case ogsr_imgui_style_Dracula: SetupImGuiDraculaStyle(); break;
    case ogsr_imgui_style_AmberYellow: SetupImGuiAmberYellowStyle(); break;
    case ogsr_imgui_style_Amethyst: SetupImGuiAmethystStyle(); break;
    case ogsr_imgui_style_Sapphire: SetupImGuiSapphireStyle(); break;
    }
}

void CImGuiSettingsWnd::Render()
{
    if (!RenderBegin())
    {
        RenderEnd();
        return;
    }

    ImGui::Separator();

    auto ShowStyleSelector = [](const char* label) {
        string_path fname;
        FS.update_path(fname, fsgame::app_data_root, "imgui.ltx");
        CInifile imgui_custom_ltx{fname, FALSE};

        u32 style_idx = READ_IF_EXISTS(reinterpret_cast<CInifile*>(&imgui_custom_ltx), r_u32, "im_style", "theme_selected", 0);

        bool ret = false;
        if (ImGui::BeginCombo(label, (style_idx >= 0 && style_idx < std::size(ogsr_imgui_style_names)) ? ogsr_imgui_style_names[style_idx] : ""))
        {
            for (u32 n = 0; n < std::size(ogsr_imgui_style_names); n++)
            {
                if (ImGui::Selectable(ogsr_imgui_style_names[n], style_idx == n, ImGuiSelectableFlags_SelectOnNav))
                {
                    style_idx = n;
                    ret = true;

                    SetupStyle(style_idx);
                    imgui_custom_ltx.w_u32("im_style", "theme_selected", style_idx);
                }
                else if (style_idx == n)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        return ret;
    };

    ShowStyleSelector("Theme##Selector");

    ImGui::Separator();

    RenderEnd();
}
