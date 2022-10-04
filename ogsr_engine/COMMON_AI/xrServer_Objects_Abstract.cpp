////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.cpp
//	Created 	: 19.09.2002
//  Modified 	: 14.07.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#pragma pack(push, 4)

#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "xrServer_Objects_Abstract.h"
#include "xrMessages.h"

////////////////////////////////////////////////////////////////////////////
// CSE_Visual
////////////////////////////////////////////////////////////////////////////
CSE_Visual::CSE_Visual(LPCSTR name)
{
    if (name)
    {
        string_path tmp;
        strcpy_s(tmp, name);
        if (strext(tmp))
            *strext(tmp) = 0;
        xr_strlwr(tmp);
        visual_name = tmp;
    }
    else
        visual_name = NULL;

    startup_animation = "$editor";
    flags.zero();
}

CSE_Visual::~CSE_Visual() {}

void CSE_Visual::set_visual(LPCSTR name)
{
    string_path tmp;
    strcpy_s(tmp, name);
    if (strext(tmp))
        *strext(tmp) = 0;
    xr_strlwr(tmp);
    visual_name = tmp;
}

void CSE_Visual::visual_read(NET_Packet& tNetPacket, u16 version)
{
    tNetPacket.r_stringZ(visual_name);
    if (version > 103)
        flags.assign(tNetPacket.r_u8());
}

void CSE_Visual::visual_write(NET_Packet& tNetPacket)
{
    tNetPacket.w_stringZ(visual_name);
    tNetPacket.w_u8(flags.get());
}

////////////////////////////////////////////////////////////////////////////
// CSE_Animated
////////////////////////////////////////////////////////////////////////////
CSE_Motion::CSE_Motion(LPCSTR name) { motion_name = name; }

CSE_Motion::~CSE_Motion() {}

void CSE_Motion::set_motion(LPCSTR name) { motion_name = name; }

void CSE_Motion::motion_read(NET_Packet& tNetPacket) { tNetPacket.r_stringZ(motion_name); }

void CSE_Motion::motion_write(NET_Packet& tNetPacket) { tNetPacket.w_stringZ(motion_name); }

#pragma pack(pop, 4)
