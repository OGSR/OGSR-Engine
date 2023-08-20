#include "stdafx.h"

#include "DiscordRPC.hpp"
#include "../xr_3da/x_ray.h"

constexpr const char* DISCORD_LIBRARY_DLL{"discord-rpc.dll"};

ENGINE_API DiscordRPC Discord;

void DiscordRPC::Init()
{
    m_hDiscordDLL = LoadLibrary(DISCORD_LIBRARY_DLL);
    if (!m_hDiscordDLL)
    {
        Msg("!![%s]Failed to load [%s], error: %s", __FUNCTION__, DISCORD_LIBRARY_DLL, Debug.error2string(GetLastError()));
        return;
    }

    Discord_Initialize = (pDiscord_Initialize)GetProcAddress(m_hDiscordDLL, "Discord_Initialize");
    Discord_Shutdown = (pDiscord_Shutdown)GetProcAddress(m_hDiscordDLL, "Discord_Shutdown");
    Discord_ClearPresence = (pDiscord_ClearPresence)GetProcAddress(m_hDiscordDLL, "Discord_ClearPresence");
    Discord_RunCallbacks = (pDiscord_RunCallbacks)GetProcAddress(m_hDiscordDLL, "Discord_RunCallbacks");
    Discord_UpdatePresence = (pDiscord_UpdatePresence)GetProcAddress(m_hDiscordDLL, "Discord_UpdatePresence");

    if (!Discord_Initialize || !Discord_Shutdown || !Discord_ClearPresence || !Discord_RunCallbacks || !Discord_UpdatePresence)
    {
        Msg("!![%s] Initialization failed!", __FUNCTION__);
        FreeLibrary(m_hDiscordDLL);
        m_hDiscordDLL = nullptr;
        return;
    }

    DiscordEventHandlers nullHandlers{};
    if (IS_OGSR_GA)
        Discord_Initialize("934890865237839992", &nullHandlers, TRUE, nullptr);
    else
        Discord_Initialize("777186147456778272", &nullHandlers, TRUE, nullptr);

    start_time = time(nullptr);
}

DiscordRPC::~DiscordRPC()
{
    if (!m_hDiscordDLL)
        return;

    Discord_ClearPresence();
    Discord_Shutdown();

    FreeLibrary(m_hDiscordDLL);
}

void DiscordRPC::Update(const char* level_name_translated, const char* level_name)
{
    if (!m_hDiscordDLL)
        return;

    DiscordRichPresence presenseInfo{};

    presenseInfo.startTimestamp = start_time; //время с момента запуска

    if (level_name)
        current_level_name = level_name;

    if (IS_OGSR_GA && current_level_name && (level_name || !level_name_translated))
        presenseInfo.largeImageKey = current_level_name; // большая картинка
    else
        presenseInfo.largeImageKey = "main_image"; //большая картинка
    presenseInfo.smallImageKey = "main_image_small"; //маленькая картинка
    presenseInfo.smallImageText = Core.GetEngineVersion(); //версия движка на маленькой картинке

    std::string task_txt, lname, lname_and_task;

    if (active_task_text)
    {
        task_txt = StringHasUTF8(active_task_text) ? active_task_text : StringToUTF8(active_task_text);
        presenseInfo.state = task_txt.c_str(); //Активное задание
    }

    if (level_name_translated)
        current_level_name_translated = level_name_translated;

    if (current_level_name_translated)
    {
        lname = StringHasUTF8(current_level_name_translated) ? current_level_name_translated : StringToUTF8(current_level_name_translated);
        presenseInfo.details = lname.c_str(); //название уровня
    }

    if (!lname.empty())
    {
        lname_and_task = lname;
        if (!task_txt.empty())
        {
            lname_and_task += " | ";
            lname_and_task += task_txt;
        }
    }
    else if (!task_txt.empty())
    {
        lname_and_task = task_txt;
    }

    if (!lname_and_task.empty())
        presenseInfo.largeImageText = lname_and_task.c_str(); //название уровня + активное задание на большой картинке

    Discord_UpdatePresence(&presenseInfo);
}
