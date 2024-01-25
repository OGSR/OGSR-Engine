#include "stdafx.h"
#include "imgui.h"
#include "embedded_editor_sound_env.h"

#include <mmeapi.h>
#include <../eax/Include/eax.h>

/*
constexpr const char* env_names[]{"GENERIC",    "PADDEDCELL",      "ROOM",       "BATHROOM",      "LIVINGROOM", "STONEROOM", "AUDITORIUM", "CONCERTHALL", "CAVE",   "ARENA",
                                  "HANGAR",     "CARPETEDHALLWAY", "HALLWAY",    "STONECORRIDOR", "ALLEY",      "FOREST",    "CITY",       "MOUNTAINS",   "QUARRY", "PLAIN",
                                  "PARKINGLOT", "SEWERPIPE",       "UNDERWATER", "DRUGGED",       "DIZZY",      "PSYCHOTIC"};
static_assert(std::size(env_names) == EAX_ENVIRONMENT_COUNT);
*/

void ShowSoundEnvEditor(bool& show)
{
    if (!ImGui::Begin("SoundEnv###SoundEnv", &show))
    {
        ImGui::End();
        return;
    }

    ::Sound->DbgCurrentEnvPaused(true);

    CSound_environment* env = ::Sound->DbgCurrentEnv();

    static string256 env_name;

    strcpy_s(env_name, env->name.c_str());

    ImGui::InputText("Current Zone Name", env_name, std::size(env_name), ImGuiInputTextFlags_ReadOnly);

    ImGui::Separator();

    ImGui::SliderFloat("Room", &env->Room, EAXLISTENER_MINROOM, EAXLISTENER_MAXROOM);
    ImGui::SliderFloat("RoomHF", &env->RoomHF, EAXLISTENER_MINROOMHF, EAXLISTENER_MAXROOMHF);
    ImGui::SliderFloat("RoomRolloffFactor", &env->RoomRolloffFactor, EAXLISTENER_MINROOMROLLOFFFACTOR, EAXLISTENER_MAXROOMROLLOFFFACTOR);
    ImGui::SliderFloat("DecayTime", &env->DecayTime, EAXLISTENER_MINDECAYTIME, EAXLISTENER_MAXDECAYTIME);
    ImGui::SliderFloat("DecayHFRatio", &env->DecayHFRatio, EAXLISTENER_MINDECAYHFRATIO, EAXLISTENER_MAXDECAYHFRATIO);
    ImGui::SliderFloat("Reflections", &env->Reflections, EAXLISTENER_MINREFLECTIONS, EAXLISTENER_MAXREFLECTIONS);
    ImGui::SliderFloat("ReflectionsDelay", &env->ReflectionsDelay, EAXLISTENER_MINREFLECTIONSDELAY, EAXLISTENER_MAXREFLECTIONSDELAY);
    ImGui::SliderFloat("Reverb", &env->Reverb, EAXLISTENER_MINREVERB, EAXLISTENER_MAXREVERB);
    ImGui::SliderFloat("ReverbDelay", &env->ReverbDelay, EAXLISTENER_MINREVERBDELAY, EAXLISTENER_MAXREVERBDELAY);
    ImGui::SliderFloat("EnvironmentSize", &env->EnvironmentSize, EAXLISTENER_MINENVIRONMENTSIZE, EAXLISTENER_MAXENVIRONMENTSIZE);
    ImGui::SliderFloat("EnvironmentDiffusion", &env->EnvironmentDiffusion, EAXLISTENER_MINENVIRONMENTDIFFUSION, EAXLISTENER_MAXENVIRONMENTDIFFUSION);
    ImGui::SliderFloat("AirAbsorptionHF", &env->AirAbsorptionHF, EAXLISTENER_MINAIRABSORPTIONHF, EAXLISTENER_MAXAIRABSORPTIONHF);

/* // Закомментировано, т.к. у нас вот это Environment нигде не используется, его менять смысла нет.
    ImGui::SliderInt("Current: ", reinterpret_cast<int*>(&env->Environment), 0, EAX_ENVIRONMENT_COUNT - 1);
    ImGui::SameLine();
    ImGui::Text(env_names[env->Environment]);
*/

    if (ImGui::Button("Save"))
    {
        ::Sound->DbgCurrentEnvSave();

        Msg("--SoundEnvEditor saved!");
    }

    ImGui::End();
}