////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects.cpp
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "xrServer_Objects.h"
#include "game_base_space.h"

////////////////////////////////////////////////////////////////////////////
// CSE_Shape
////////////////////////////////////////////////////////////////////////////
CSE_Shape::CSE_Shape() {}

CSE_Shape::~CSE_Shape() {}

void CSE_Shape::cform_read(NET_Packet& tNetPacket)
{
    shapes.clear();
    u8 count;
    tNetPacket.r_u8(count);

    while (count)
    {
        shape_def S;
        tNetPacket.r_u8(S.type);
        switch (S.type)
        {
        case 0: tNetPacket.r(&S.data.sphere, sizeof(S.data.sphere)); break;
        case 1: tNetPacket.r_matrix(S.data.box); break;
        }
        shapes.push_back(S);
        --count;
    }
}

void CSE_Shape::cform_write(NET_Packet& tNetPacket)
{
    tNetPacket.w_u8(u8(shapes.size()));
    for (u32 i = 0; i < shapes.size(); ++i)
    {
        shape_def& S = shapes[i];
        tNetPacket.w_u8(S.type);
        switch (S.type)
        {
        case 0: tNetPacket.w(&S.data.sphere, sizeof(S.data.sphere)); break;
        case 1: tNetPacket.w_matrix(S.data.box); break;
        }
    }
}

void CSE_Shape::assign_shapes(CShapeData::shape_def* _shapes, u32 _cnt)
{
    shapes.resize(_cnt);
    for (u32 k = 0; k < _cnt; k++)
        shapes[k] = _shapes[k];
}

////////////////////////////////////////////////////////////////////////////
// CSE_Temporary
////////////////////////////////////////////////////////////////////////////
CSE_Temporary::CSE_Temporary(LPCSTR caSection) : CSE_Abstract(caSection) { m_tNodeID = u32(-1); }

CSE_Temporary::~CSE_Temporary() {}

void CSE_Temporary::STATE_Read(NET_Packet& tNetPacket, u16 size) { tNetPacket.r_u32(m_tNodeID); };

void CSE_Temporary::STATE_Write(NET_Packet& tNetPacket) { tNetPacket.w_u32(m_tNodeID); };

void CSE_Temporary::UPDATE_Read(NET_Packet& tNetPacket){};

void CSE_Temporary::UPDATE_Write(NET_Packet& tNetPacket){};

////////////////////////////////////////////////////////////////////////////
// CSE_PHSkeleton
////////////////////////////////////////////////////////////////////////////
CSE_PHSkeleton::CSE_PHSkeleton(LPCSTR caSection)
{
    source_id = u16(-1);
    _flags.zero();
}

CSE_PHSkeleton::~CSE_PHSkeleton() {}

void CSE_PHSkeleton::STATE_Read(NET_Packet& tNetPacket, u16 size)
{
    CSE_Visual* visual = smart_cast<CSE_Visual*>(this);
    R_ASSERT(visual);
    tNetPacket.r_stringZ(visual->startup_animation);
    tNetPacket.r_u8(_flags.flags);
    tNetPacket.r_u16(source_id);
    if (_flags.test(flSavedData))
    {
        data_load(tNetPacket);
    }
}

void CSE_PHSkeleton::STATE_Write(NET_Packet& tNetPacket)
{
    CSE_Visual* visual = smart_cast<CSE_Visual*>(this);
    R_ASSERT(visual);
    tNetPacket.w_stringZ(visual->startup_animation);
    tNetPacket.w_u8(_flags.flags);
    tNetPacket.w_u16(source_id);
    ////////////////////////saving///////////////////////////////////////
    if (_flags.test(flSavedData))
    {
        data_save(tNetPacket);
    }
}

void CSE_PHSkeleton::data_load(NET_Packet& tNetPacket)
{
    saved_bones.net_Load(tNetPacket);
    _flags.set(flSavedData, TRUE);
}

void CSE_PHSkeleton::data_save(NET_Packet& tNetPacket)
{
    saved_bones.net_Save(tNetPacket);
    //	this comment is added by Dima (correct me if this is wrong)
    //  if we call 2 times in a row StateWrite then we get different results
    //	WHY???
    //	_flags.set(flSavedData,FALSE);
}

void CSE_PHSkeleton::load(NET_Packet& tNetPacket)
{
    /* это больше не используется. Раньше сюда попадало то, что записывал
       в нетпакет CPHSkeleton::SaveNetState(), который вызывался при
       сейве.

        _flags.assign				(tNetPacket.r_u8());
        data_load					(tNetPacket);
    */
    source_id = u16(-1); //.
}
void CSE_PHSkeleton::UPDATE_Write(NET_Packet& tNetPacket) {}

void CSE_PHSkeleton::UPDATE_Read(NET_Packet& tNetPacket) {}

CSE_AbstractVisual::CSE_AbstractVisual(LPCSTR section) : inherited1(section), inherited2(section) {}

CSE_AbstractVisual::~CSE_AbstractVisual() {}

void CSE_AbstractVisual::STATE_Read(NET_Packet& tNetPacket, u16 size)
{
    visual_read(tNetPacket, m_wVersion);
    tNetPacket.r_stringZ(startup_animation);
}

void CSE_AbstractVisual::STATE_Write(NET_Packet& tNetPacket)
{
    visual_write(tNetPacket);
    tNetPacket.w_stringZ(startup_animation);
}

void CSE_AbstractVisual::UPDATE_Read(NET_Packet& tNetPacket) {}

void CSE_AbstractVisual::UPDATE_Write(NET_Packet& tNetPacket) {}
LPCSTR CSE_AbstractVisual::getStartupAnimation() { return *startup_animation; }

CSE_Visual* CSE_AbstractVisual::visual() { return this; }
