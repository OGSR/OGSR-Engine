////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_Abstract.h
//	Created 	: 19.09.2002
//  Modified 	: 18.06.2004
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrServer_Space.h"
#ifdef XRGAME_EXPORTS
#include "../../xrCDB/xrCDB.h"
#else // XRGAME_EXPORTS
#include "../xrCDB/xrCDB.h"
#endif // XRGAME_EXPORTS
#include "ShapeData.h"

class NET_Packet;
class CDUInterface;

#ifndef XRGAME_EXPORTS
#include "Sound.h"
#endif

#include "..\..\Include\xrRender\DrawUtils.h"

#pragma warning(push)
#pragma warning(disable : 4005)

#pragma pack(push, 4)

class ISE_Shape
{
public:
    virtual ~ISE_Shape() = default;
    virtual void __stdcall assign_shapes(CShapeData::shape_def* shapes, u32 cnt) = 0;
};

SERVER_ENTITY_DECLARE_BEGIN0(CSE_Visual)
public:
shared_str visual_name;
shared_str startup_animation;
enum
{
    flObstacle = (1 << 0)
};
Flags8 flags;

public:
CSE_Visual(LPCSTR name = 0);
virtual ~CSE_Visual();

void visual_read(NET_Packet& P, u16 version);
void visual_write(NET_Packet& P);

void set_visual(LPCSTR name);
LPCSTR get_visual() const { return *visual_name; };

virtual CSE_Visual* __stdcall visual() = 0;
}
;
add_to_type_list(CSE_Visual)
#define script_type_list save_type_list(CSE_Visual)

    SERVER_ENTITY_DECLARE_BEGIN0(CSE_Motion) public : shared_str motion_name;
public:
CSE_Motion(LPCSTR name = 0);
virtual ~CSE_Motion();

void motion_read(NET_Packet& P);
void motion_write(NET_Packet& P);

void set_motion(LPCSTR name);
LPCSTR get_motion() const { return *motion_name; };

virtual CSE_Motion* __stdcall motion() = 0;
}
;
add_to_type_list(CSE_Motion)
#define script_type_list save_type_list(CSE_Motion)

    struct ISE_AbstractLEOwner
{
    virtual ~ISE_AbstractLEOwner() = default;
    virtual void __stdcall get_bone_xform(LPCSTR name, Fmatrix& xform) = 0;
};

struct ISE_Abstract
{
    enum
    {
        flUpdateProperties = u32(1 << 0),
        flVisualChange = u32(1 << 1),
        flVisualAnimationChange = u32(1 << 2),
        flMotionChange = u32(1 << 3),
    };
    Flags32 m_editor_flags;
    IC void set_editor_flag(u32 mask) { m_editor_flags.set(mask, TRUE); }

public:
    virtual void __stdcall Spawn_Write(NET_Packet& tNetPacket, BOOL bLocal) = 0;
    virtual BOOL __stdcall Spawn_Read(NET_Packet& tNetPacket) = 0;
    virtual LPCSTR __stdcall name() const = 0;
    virtual void __stdcall set_name(LPCSTR) = 0;
    virtual LPCSTR __stdcall name_replace() const = 0;
    virtual void __stdcall set_name_replace(LPCSTR) = 0;
    virtual Fvector& __stdcall position() = 0;
    virtual Fvector& __stdcall angle() = 0;
    virtual Flags16& __stdcall flags() = 0;
    virtual ISE_Shape* __stdcall shape() = 0;
    virtual CSE_Visual* __stdcall visual() = 0;
    virtual CSE_Motion* __stdcall motion() = 0;
    virtual bool __stdcall validate() = 0;
    virtual void __stdcall on_render(CDUInterface* du, ISE_AbstractLEOwner* owner, bool bSelected, const Fmatrix& parent, int priority, bool strictB2F) = 0;
};

#pragma warning(pop)

#pragma pack(pop)
