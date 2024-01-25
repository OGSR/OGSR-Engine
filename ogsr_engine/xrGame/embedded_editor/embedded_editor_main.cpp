#include "stdAfx.h"

#include <dinput.h>

#include "imgui.h"

#include "../../XR_3DA/xr_input.h"
#include "xr_level_controller.h"

#include "embedded_editor_main.h"
#include "embedded_editor_helper.h"
#include "embedded_editor_weather.h"
#include "embedded_editor_hud.h"
#include "embedded_editor_pos_informer.h"
#include "embedded_editor_sound_env.h"

bool bShowWindow = true;

bool show_test_window = false;
bool show_weather_window = false;
bool show_hud_editor = false;
bool show_position_informer = false;
bool show_sound_env_window = false;

/*
bool show_info_window = false;
bool show_prop_window = false;
bool show_restr_window = false;
bool show_shader_window = false;
bool show_occ_window = false;
bool show_node_editor = false;
*/

static bool isAlt = false;

enum class EditorStage
{
    None,
    Light,
    Full,

    Count,
};

EditorStage stage = EditorStage::None;

bool IsEditorActive() { return stage == EditorStage::Full || (stage == EditorStage::Light && isAlt); }

bool IsEditor() { return stage != EditorStage::None; }

void ShowMain()
{
    ImguiWnd wnd("Main");
    if (wnd.Collapsed)
        return;

    ImGui::Text("OGSR Editor");

    if (ImGui::Button("Weather"))
        show_weather_window = !show_weather_window;
    if (ImGui::Button("Position Informer"))
        show_position_informer = !show_position_informer;
    if (ImGui::Button("HUD Editor"))
        show_hud_editor = !show_hud_editor;
    if (ImGui::Button("Sound Env Editor"))
        show_sound_env_window = !show_sound_env_window;

    if (ImGui::Button("Test Window"))
        show_test_window = !show_test_window;

    /*
    if (ImGui::Button("Test Node Editor"))
        show_node_editor = !show_node_editor;
    */

    bool full = stage == EditorStage::Full;
    if (ImGui::Checkbox("Active", &full))
        stage = full ? EditorStage::Full : EditorStage::Light;

    float framerate = ImGui::GetIO().Framerate;

    static xr_vector<float> frames;

    	// Get frames
    if (frames.size() > 100) // Max seconds to show
    {
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

    ImGui::PlotHistogram("", &frames[0], frames.size(), 0, NULL, 0.0f, 300.0f, ImVec2(300, 100));
}

void ShowEditor()
{
    ::Sound->DbgCurrentEnvPaused(false);

    if (!IsEditor())
        return;

    ShowMain();

    if (show_test_window)
    {
        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&show_test_window);
    }
    // if (show_node_editor)
    //{
    //     ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
    //     if (ImGui::Begin("Example: Custom Node Graph", &show_node_editor))
    //     {
    //         ImGui::TestNodeGraphEditor();
    //     }
    //     ImGui::End();
    // }
    if (show_weather_window)
        ShowWeatherEditor(show_weather_window);
    if (show_position_informer)
        ShowPositionInformer(show_position_informer);
    if (show_hud_editor)
        ShowHudEditor(show_hud_editor);
    if (show_sound_env_window)
        ShowSoundEnvEditor(show_sound_env_window);
    //if (show_prop_window)
    //    ShowPropEditor(show_prop_window);
    //if (show_lua_binder)
    //    ShowLuaBinder(show_lua_binder);
    //if (show_logic_editor)
    //    ShowLogicEditor(show_logic_editor);
}

bool isRControl = false, isLControl = false, isRShift = false, isLShift = false;
extern bool GetShift();

bool Editor_KeyPress(int key)
{
    if (key == DIK_F10)
    {
        if (GetShift())
            stage = static_cast<EditorStage>((static_cast<int>(stage) + 1) % static_cast<int>(EditorStage::Count));
    }
    else if (key == DIK_RALT || key == DIK_LALT)
        isAlt = true;

    if (!IsEditorActive())
        return false;

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;

    switch (key)
    {
    case DIK_RALT:
    case DIK_LALT:
    case DIK_F10: break;

    case DIK_RCONTROL:
        isRControl = true;
        io.KeyCtrl = true;
        break;
    case DIK_LCONTROL:
        isLControl = true;
        io.KeyCtrl = true;
        break;
    case DIK_RSHIFT:
        isRShift = true;
        io.KeyShift = true;
        break;
    case DIK_LSHIFT:
        isLShift = true;
        io.KeyShift = true;
        break;
    case MOUSE_1: io.MouseDown[0] = true; break;
    case MOUSE_2: io.MouseDown[1] = true; break;
    case MOUSE_3: io.MouseDown[2] = true; break;

    default:
        if (key < 512)
            io.KeysDown[key] = true;

        if (const u16 ch = pInput->DikToChar(key, false))
            io.AddInputCharacter(ch);

            // пример подключения шрифт и нормального ввода в utf
            // запишем что б не забыть, но хз надо ли оно реально

            //io.Fonts->AddFontFromFileTTF("DroidSans.ttf", 13.0f, &font_config, io.Fonts->GetGlyphRangesCyrillic());

            //wchar_t wch;
            //MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)&ch, 1, &wch, 1);
            //io.AddInputCharacter(wch);
    }
    return true;
}

bool Editor_KeyRelease(int key)
{
    if (key == DIK_RALT || key == DIK_LALT)
        isAlt = false;
    bool active = IsEditorActive();

    ImGuiIO& io = ImGui::GetIO();
    if (!active)
        io.MouseDrawCursor = false;

    switch (key)
    {
    case DIK_RCONTROL:
        isRControl = false;
        io.KeyCtrl = isRControl || isLControl;
        break;
    case DIK_LCONTROL:
        isLControl = false;
        io.KeyCtrl = isRControl || isLControl;
        break;
    case DIK_RSHIFT:
        isRShift = false;
        io.KeyShift = isRShift || isLShift;
        break;
    case DIK_LSHIFT:
        isLShift = false;
        io.KeyShift = isRShift || isLShift;
        break;
    case MOUSE_1: io.MouseDown[0] = false; break;
    case MOUSE_2: io.MouseDown[1] = false; break;
    case MOUSE_3: io.MouseDown[2] = false; break;
    default:
        if (key < 512)
            io.KeysDown[key] = false;
    }
    return active;
}

bool Editor_KeyHold(int key)
{
    if (!IsEditorActive())
        return false;
    return true;
}

bool Editor_MouseMove(int dx, int dy)
{
    if (!IsEditorActive())
        return false;

    ImGuiIO& io = ImGui::GetIO();
    POINT p;
    GetCursorPos(&p);
    io.MousePos.x = p.x;
    io.MousePos.y = p.y;
    return true;
}

static int s_direction;

bool Editor_MouseWheel(int direction)
{
    if (!IsEditorActive())
        return false;

    s_direction = direction;

    //ImGuiIO& io = ImGui::GetIO();
    //io.MouseWheel += direction > 0 ? +1.0f : -1.0f;
    return true;
}

void Editor_OnFrame()
{
    if (s_direction != 0)
    {       
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel += s_direction > 0 ? +1.0f : -1.0f;
        s_direction = 0;
    }
}