#include "stdAfx.h"

#include <dinput.h>

#include "imgui.h"
#include "imgui_internal.h"
#include "../../Layers/xrRenderDX10/imgui_impl_dx11.h"

#include "../../XR_3DA/xr_input.h"
#include "../../XR_3DA/IGame_Persistent.h"
#include "xr_level_controller.h"

#include "embedded_editor_main.h"
#include "embedded_editor_helper.h"
#include "embedded_editor_weather.h"
#include "embedded_editor_hud.h"
#include "embedded_editor_pos_informer.h"
#include "embedded_editor_sound_env.h"
#include "embedded_editor_debug_rt.h"
#include "embedded_editor_game.h"
#include "embedded_editor_settings.h"

#include "../Layers/xrRender/Debug/dxPixEvents.h"


CImGuiEditor::CImGuiEditor() { 
    CImGuiGameWnd* game_wnd = xr_new<CImGuiGameWnd>();
    game_wnd->m_bCanHide = false;
    game_wnd->m_Opened = true;
    game_wnd->m_Name = "Game";
    m_Windows.push_back(game_wnd);

    CImGuiMainWnd* main_wnd = xr_new<CImGuiMainWnd>();
    main_wnd->m_Name = "Main";
    main_wnd->m_Opened = true;
    m_Windows.push_back(main_wnd);

    CImGuiDebugRTWnd* rt_debug = xr_new<CImGuiDebugRTWnd>();
    rt_debug->m_Name = "RT debug";
    m_Windows.push_back(rt_debug);

    CImGuiWeatherWnd* weather_wnd = xr_new<CImGuiWeatherWnd>();
    weather_wnd->m_Name = "Weather###Weather";
    m_Windows.push_back(weather_wnd);

    CImGuiPositionInformerWnd* pos_informer_wnd = xr_new<CImGuiPositionInformerWnd>();
    pos_informer_wnd->m_Name = "Position Informer";
    m_Windows.push_back(pos_informer_wnd);

    CImGuiHudEditorWnd* hud_editor_wnd = xr_new<CImGuiHudEditorWnd>();
    hud_editor_wnd->m_Name = "HUD Editor";
    m_Windows.push_back(hud_editor_wnd);

    CImGuiSoundEnvWnd* sound_env_wnd = xr_new<CImGuiSoundEnvWnd>();
    sound_env_wnd->m_Name = "SoundEnv###SoundEnv";
    m_Windows.push_back(sound_env_wnd);

    auto* settings_wnd = xr_new<CImGuiSettingsWnd>();
    m_Windows.push_back(settings_wnd);
}

CImGuiEditor::~CImGuiEditor()
{
    for (auto& wnd : m_Windows)
        xr_delete(wnd);
}

void CImGuiEditor::RenderDockSpace()
{
    const ImGuiID dockspace_id = ImGui::GetID("OGSRDockSpace");
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
    {
        ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
        ImGuiID dock_id_left = 0;
        ImGuiID dock_id_main = dockspace_id;
        ImGui::DockBuilderSplitNode(dock_id_main, ImGuiDir_Right, 0.20f, &dock_id_left, &dock_id_main);
        ImGuiID dock_id_left_top = 0;
        ImGuiID dock_id_left_bottom = 0;
        ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Up, 0.50f, &dock_id_left_top, &dock_id_left_bottom);
        ImGui::DockBuilderDockWindow("Game", dock_id_main);     //-- main window = game rt
        ImGui::DockBuilderDockWindow("Main", dock_id_left_top);
        ImGui::DockBuilderDockWindow("RT debug", dock_id_left_top);
        ImGui::DockBuilderDockWindow("Weather###Weather", dock_id_left_bottom); //-- idk this naming
        ImGui::DockBuilderDockWindow("Position Informer", dock_id_left_bottom);
        ImGui::DockBuilderDockWindow("HUD Editor", dock_id_left_bottom);
        ImGui::DockBuilderDockWindow("SoundEnv###SoundEnv", dock_id_left_bottom);
        ImGui::DockBuilderDockWindow("Demo Window", dock_id_left_bottom);
        ImGui::DockBuilderDockWindow("ImSettings###ImSettings", dock_id_left_bottom);
        ImGui::DockBuilderFinish(dockspace_id);
    }
    ImGui::DockSpaceOverViewport(dockspace_id, viewport, 0);
}

void CImGuiEditor::RenderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        float pos_tmp = ImGui::GetCursorPosX();
        shared_str logo = "OGSR Editor (SHIFT + F10)";
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize(logo.c_str()).x) * 0.5f);
        ImGui::Text(logo.c_str());

        shared_str close = "X";
        ImGui::SetCursorPosX(ImGui::GetWindowSize().x - ImGui::CalcTextSize(close.c_str()).x - 10.0f);
        if (ImGui::Button(close.c_str()))
            target_stage = EditorStage::None;

        ImGui::SetCursorPosX(pos_tmp);
        bool full = target_stage == EditorStage::Full;
        if(ImGui::Checkbox("##Active", &full))
            target_stage = full ? EditorStage::Full : EditorStage::Light;

        if (ImGui::BeginMenu("Editor"))
        {
            if (ImGui::MenuItem("Active", nullptr, &full))
                target_stage = full ? EditorStage::Full : EditorStage::Light;
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Window"))
        {
            for (auto& wnd : m_Windows)
            {
                if (wnd->m_bCanHide)
                    if (ImGui::MenuItem(wnd->m_Name.c_str(), nullptr, wnd->m_Opened))
                        wnd->m_Opened = !wnd->m_Opened;
            }

            if(ImGui::MenuItem("Demo Window", nullptr, m_bShowDemoWindow))
                m_bShowDemoWindow = !m_bShowDemoWindow;

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void CImGuiEditor::ShowEditor()
{
    ::Sound->DbgCurrentEnvPaused(false);

    if (!IsEditor())
        return;
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = IsEditorActive();

    //-- always first
    RenderDockSpace();
    RenderMenuBar();
    RenderDemoWindow();

    for (auto& wnd : m_Windows)
        if (wnd->m_Opened)
            wnd->Render();    
}

void CImGuiEditor::RenderDemoWindow() {
    if (m_bShowDemoWindow)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&m_bShowDemoWindow);
    }
}


void CImGuiMainWnd::Render()
{
    if (!RenderBegin())
    {
        RenderEnd();
        return;
    }

    ImGui::Text("OGSR Editor");

    CImGuiEditor& editor = CImGuiEditor::Get();

    for (auto& wnd : editor.Windows())
    {
        if (wnd->m_bCanHide)
            if (ImGui::Button(wnd->m_Name.c_str()))
                wnd->m_Opened = !wnd->m_Opened;
    }

    if (ImGui::Button("Demo Window"))
        editor.m_bShowDemoWindow = !editor.m_bShowDemoWindow;

    bool full = editor.target_stage == EditorStage::Full;
    if (ImGui::Checkbox("Active", &full))
        editor.target_stage = full ? EditorStage::Full : EditorStage::Light;

    const float framerate = ImGui::GetIO().Framerate;

    static xr_vector<float> frames;

    // Get frames
    const size_t max_frames = static_cast<size_t>(ImGui::GetContentRegionAvail().x / 2.f); // Max frames to show
    if (frames.size() > max_frames)
    {
        frames.resize(max_frames);
        for (size_t i = 1; i < frames.size(); i++)
        {
            frames[i - 1] = frames[i];
        }
        frames[frames.size() - 1] = framerate;
    }
    else
    {
        frames.push_back(framerate);
    }

    ImGui::Text("TPS %.3f ms/frame (%.1f FPS)", 1000.0f / framerate, framerate);

    ImGui::PlotHistogram("##FPS", frames.data(), frames.size(), 0, nullptr, 0.0f, 300.0f, ImVec2{ImGui::GetContentRegionAvail().x, 100});

#ifdef TRACY_ENABLE
    ImGui::Separator();

    static int stack_levels = 8;
    ImGui::SliderInt("Depth", &stack_levels, 0, 8);

    auto& perf = PIXEventsStatistics();
    for (size_t i = 0; i < perf.count; i++)
    {
        auto& event = perf.events[i];
        if (event.stack < static_cast<u64>(stack_levels))
        {
            u64 time_micros = (event.end - event.begin) / (event.freq / 1000000);
            float time_milliseconds = (float)time_micros * 0.001f;
            ImGui::Text("%*s%s: %.3fms", event.stack * 2, " ", event.name.c_str(), time_milliseconds);
        }
    }
#endif

    RenderEnd();
}


extern bool GetShift();
bool CImGuiEditor::Editor_KeyPress(int key)
{
    if (key == DIK_F10)
    {
        if (GetShift())
        {
            target_stage = static_cast<EditorStage>((static_cast<int>(target_stage) + 1) % static_cast<int>(EditorStage::Count));
            return true;
        }
    }

    return IsEditorActive();
}

bool CImGuiEditor::Editor_KeyRelease(int key)
{
    // т.к.нажатия клавиш имгуй сейчас ловит через winmain, надо выключать exclusive input,
    // а то события нажатий клавиш не приходят если он включен
    if (target_stage == EditorStage::Full)
    {
        if (pInput->exclusive_mode())
            pInput->exclusive_mode(false);
    }
    else if (bool saved_state{!!psDeviceFlags.test(rsExclusiveMode)}; pInput->exclusive_mode() != saved_state)
        pInput->exclusive_mode(saved_state);

    return IsEditorActive();
}

bool CImGuiEditor::Editor_KeyHold(int key) { return IsEditorActive(); }

bool CImGuiEditor::Editor_MouseMove(int dx, int dy) { return IsEditorActive(); }

bool CImGuiEditor::Editor_MouseWheel(int direction) { return IsEditorActive(); }
