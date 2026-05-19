#include "stdafx.h"

#include "DiscordRPC.hpp"
#include "../xr_3da/x_ray.h"

#ifndef DISABLE_DISCORD
#include "../DiscordRPC/DiscordRPC/include/discord_rpc.h"
#pragma comment(lib, "DiscordRPC")
#endif

ENGINE_API DiscordRPC Discord;

void DiscordRPC::Init()
{
#ifndef DISABLE_DISCORD
    DiscordEventHandlers nullHandlers{};
    if (IS_OGSR_GA)
        Discord_Initialize("934890865237839992", &nullHandlers, TRUE, nullptr);
    else
        Discord_Initialize("777186147456778272", &nullHandlers, TRUE, nullptr);

    start_time = time(nullptr);
#endif
}

void DiscordRPC::Destroy()
{
#ifndef DISABLE_DISCORD
    Discord_ClearPresence();
    Discord_Shutdown();
#endif
}

void DiscordRPC::Update(const char* level_name_translated, const char* level_name)
{
#ifndef DISABLE_DISCORD
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

    presenseInfo.buttons[0] = {"OGSR Github", "https://github.com/OGSR"};
    presenseInfo.buttons[1] = {"OGSR Discord", "https://discord.com/servers/ogsr-508530704674455562"};

    Discord_UpdatePresence(&presenseInfo);
#endif
}
