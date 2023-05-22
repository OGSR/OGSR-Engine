#include "stdafx.h"

#include <fstream>

#include "imgui.h"

#include "Actor.h"
#include "ai_object_location.h"

#include "embedded_editor_pos_informer.h"
#include "embedded_editor_helper.h"

string256 section_name = "section";

void SavePosition(string256 sect)
{
    std::string userDir = FS.get_path("$app_data_root$")->m_Path, fileToWrite = "//position_info.txt";
    std::string path = userDir + fileToWrite;
    std::fstream informer;

    informer.open(path, std::fstream::app);

    if (informer.is_open())
    {
        informer << "[" << sect << "]"
                 << "\n";
        informer << "position = " << Actor()->Position().x << ", " << Actor()->Position().y << ", " << Actor()->Position().z << "\n";
        informer << "direction = " << Actor()->Direction().x << ", " << Actor()->Direction().y << ", " << Actor()->Direction().z << "\n";
        informer << "game_vertex_id = " << Actor()->ai_location().game_vertex_id() << "\n";
        informer << "level_vertex_id = " << Actor()->ai_location().level_vertex_id() << "\n\n";
    }
}

void ShowPositionInformer(bool& show)
{
    ImguiWnd wnd("Position Informer", &show);

    if (wnd.Collapsed)
        return;

    Fvector actor_position = Actor()->Position();
    Fvector actor_direction = Actor()->Direction();
    int actor_game_vertex = Actor()->ai_location().game_vertex_id();
    int actor_level_vertex = Actor()->ai_location().level_vertex_id();

    ImGui::InputText("section name", (char*)&section_name, 256);
    ImGui::InputFloat3("position", (float*)&actor_position);
    ImGui::InputFloat3("direction", (float*)&actor_direction);
    ImGui::InputInt("game_vertex_id", (int*)&actor_game_vertex);
    ImGui::InputInt("level_vertex_id", (int*)&actor_level_vertex);

    if (ImGui::Button("Save"))
    {
        SavePosition(section_name);
    }
}