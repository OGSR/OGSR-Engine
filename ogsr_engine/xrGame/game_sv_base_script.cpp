////////////////////////////////////////////////////////////////////////////
//	Module 		: game_sv_base_script.cpp
//	Created 	: 28.06.2004
//  Modified 	: 28.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Base server game script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "game_sv_base.h"
#include "xrMessages.h"
#include "ui/UIInventoryUtilities.h"
#include "xr_time.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "UI/UIGameTutorial.h"
#include "string_table.h"
#include "object_broker.h"
#include "player_hud.h"

using namespace luabind;

CUISequencer* g_tutorial = NULL;
CUISequencer* g_tutorial2 = NULL;

void start_tutorial(LPCSTR name)
{
    if (g_tutorial)
    {
        VERIFY(!g_tutorial2);
        g_tutorial2 = g_tutorial;
    };

    g_tutorial = xr_new<CUISequencer>();
    g_tutorial->Start(name);
    if (g_tutorial2)
        g_tutorial->m_pStoredInputReceiver = g_tutorial2->m_pStoredInputReceiver;
}

void stop_tutorial()
{
    if (g_tutorial)
        g_tutorial->Stop();
}

u32 PlayHudMotion(u8 hand, LPCSTR hud_section, LPCSTR anm_name, bool bMixIn = true, float speed = 1.f, bool bOverride_item = false)
{
    return g_player_hud->script_anim_play(hand, hud_section, anm_name, bMixIn, speed, bOverride_item);
}

void StopHudMotion() { g_player_hud->script_anim_stop(); }

float MotionLength(LPCSTR hud_section, LPCSTR anm_name, float speed) { return g_player_hud->motion_length_script(hud_section, anm_name, speed); }

bool AllowHudMotion() { return g_player_hud->allow_script_anim(); }

float PlayBlendAnm(LPCSTR name, u8 part, float speed, float power, bool bLooped, bool no_restart) { return g_player_hud->PlayBlendAnm(name, part, speed, power, bLooped, no_restart); }

void StopBlendAnm(LPCSTR name, bool bForce) { g_player_hud->StopBlendAnm(name, bForce); }

void StopAllBlendAnms(bool bForce) { g_player_hud->StopAllBlendAnms(bForce); }

float SetBlendAnmTime(LPCSTR name, float time) { return g_player_hud->SetBlendAnmTime(name, time); }

LPCSTR translate_string(LPCSTR str) { return *CStringTable().translate(str); }

bool has_active_tutotial() { return (g_tutorial != NULL); }

LPCSTR generate_id() 
{
    GUID guid;
    CoCreateGuid(&guid);

    // 32 hex chars + 4 hyphens + null terminator
    char guid_string[37];
    snprintf(guid_string, sizeof(guid_string), "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", 
        guid.Data1, guid.Data2, guid.Data3, 
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

    shared_str r = guid_string;
    return r.c_str();
}


void game_sv_GameState::script_register(lua_State* L)
{
    module(L,"game")[
        class_<xrTime>("CTime")
           .enum_("date_format")[
                value("DateToDay", int(InventoryUtilities::edpDateToDay)), 
                value("DateToMonth", int(InventoryUtilities::edpDateToMonth)),
                value("DateToYear", int(InventoryUtilities::edpDateToYear))
           ]
           .enum_("time_format")[
                value("TimeToHours", int(InventoryUtilities::etpTimeToHours)), 
                value("TimeToMinutes", int(InventoryUtilities::etpTimeToMinutes)),
                value("TimeToSeconds", int(InventoryUtilities::etpTimeToSeconds)), 
                value("TimeToMilisecs", int(InventoryUtilities::etpTimeToMilisecs))
           ]
           .def(constructor<>())
           .def(constructor<const xrTime&>())
           .def(const_self < xrTime())
           .def(const_self <= xrTime())
           .def(const_self > xrTime())
           .def(const_self >= xrTime())
           .def(const_self == xrTime())
           .def(self + xrTime())
           .def(self - xrTime())

           .def("diffSec", &xrTime::diffSec_script)
           .def("add", &xrTime::add_script)
           .def("sub", &xrTime::sub_script)

           .def("setHMS", &xrTime::setHMS)
           .def("setHMSms", &xrTime::setHMSms)
           .def("set", &xrTime::set)
           .def("get", &xrTime::get,
                pure_out_value<2>() + pure_out_value<3>() + pure_out_value<4>() + pure_out_value<5>() + pure_out_value<6>() + pure_out_value<7>() + pure_out_value<8>())

           .def("dateToString", &xrTime::dateToString)
           .def("timeToString", &xrTime::timeToString),


       // declarations
       def("time", &get_time), 
       def("get_game_time", &get_time_struct),

       def("start_tutorial", &start_tutorial), 
       def("stop_tutorial", &stop_tutorial),
       def("has_active_tutorial", &has_active_tutotial),

       def("translate_string", &translate_string),

       def("play_hud_motion", PlayHudMotion), 
       def("stop_hud_motion", StopHudMotion), 
       def("get_motion_length", MotionLength),
       def("hud_motion_allowed", AllowHudMotion),
       def("play_hud_anm", PlayBlendAnm), 
       def("stop_hud_anm", StopBlendAnm), 
       def("stop_all_hud_anms", StopAllBlendAnms),
       def("set_hud_anm_time", SetBlendAnmTime),
       //def("set_next_hud_motion_speed", SetNextHudMotionSpeed),

       def("generate_id", &generate_id),

       def("StringHasUTF8", &StringHasUTF8), def("StringToUTF8", &StringToUTF8), def("StringFromUTF8", &StringFromUTF8)
    ];

    module(L)[
            class_<enum_exporter<EGamePlayerFlags>>("game_player_flags")
               .enum_("flags")[value("GAME_PLAYER_FLAG_LOCAL", int(GAME_PLAYER_FLAG_LOCAL))],

            class_<enum_exporter<EGamePhases>>("game_phases")
               .enum_("phases")[value("GAME_PHASE_NONE", int(GAME_PHASE_NONE)), 
                                value("GAME_PHASE_INPROGRESS", int(GAME_PHASE_INPROGRESS)),
                                value("GAME_PHASE_PENDING", int(GAME_PHASE_PENDING))],

           class_<enum_exporter<EGameMessages>>("game_messages")
               .enum_("messages")[value("GAME_EVENT_PLAYER_CONNECTED", int(GAME_EVENT_PLAYER_CONNECTED))]

    ];
}
