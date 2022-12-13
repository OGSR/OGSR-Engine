#include "stdafx.h"
#include "alife_space.h"
#include "hit.h"
#include "ode_include.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "xrMessages.h"
#include "Level.h"

#include "script_game_object.h"

SHit::SHit(float aPower, Fvector& adir, CObject* awho, u16 aelement, Fvector ap_in_bone_space, float aimpulse, ALife::EHitType ahit_type, float aAP, bool AimBullet)
{
    power = aPower;
    dir.set(adir);
    who = awho;
    if (awho)
        whoID = awho->ID();
    else
        whoID = 0;
    boneID = aelement;
    p_in_bone_space.set(ap_in_bone_space);
    impulse = aimpulse;
    hit_type = ahit_type;
    ap = aAP;
    PACKET_TYPE = 0;
    BulletID = 0;
    SenderID = 0;
    aim_bullet = AimBullet;
    full_power = aPower;
}

SHit::SHit() { invalidate(); }
void SHit::invalidate()
{
    Time = 0;
    PACKET_TYPE = 0;
    DestID = 0;

    power = -dInfinity;
    dir.set(-dInfinity, -dInfinity, -dInfinity);
    who = NULL;
    whoID = 0;
    weaponID = 0;

    boneID = BI_NONE;
    p_in_bone_space.set(-dInfinity, -dInfinity, -dInfinity);

    impulse = -dInfinity;
    hit_type = ALife::eHitTypeMax;

    ap = 0.0f;
    BulletID = 0;
    SenderID = 0;
    aim_bullet = false;
    full_power = -dInfinity;
}

bool SHit::is_valide() const { return hit_type != ALife::eHitTypeMax; }

void SHit::GenHeader(u16 PacketType, u16 ID)
{
    DestID = ID;
    PACKET_TYPE = PacketType;
    Time = Level().timeServer();
};

void SHit::Read_Packet(NET_Packet Packet)
{
    u16 type_dummy;
    Packet.r_begin(type_dummy);
    Packet.r_u32(Time);
    Packet.r_u16(PACKET_TYPE);
    Packet.r_u16(DestID);
    Read_Packet_Cont(Packet);
};

void SHit::Read_Packet_Cont(NET_Packet Packet)
{
    Packet.r_u16(whoID);
    Packet.r_u16(weaponID);
    Packet.r_dir(dir);
    Packet.r_float(power);
    Packet.r_u16(boneID);
    Packet.r_vec3(p_in_bone_space);
    Packet.r_float(impulse);
    aim_bullet = Packet.r_u16() != 0;
    hit_type = (ALife::EHitType)Packet.r_u16(); // hit type
    if (hit_type == ALife::eHitTypeFireWound)
    {
        Packet.r_float(ap);
    }
    full_power = power;
}

void SHit::Write_Packet_Cont(NET_Packet& Packet)
{
    Packet.w_u16(whoID);
    Packet.w_u16(weaponID);
    Packet.w_dir(dir);
    Packet.w_float(power);
    Packet.w_u16(boneID);
    Packet.w_vec3(p_in_bone_space);
    Packet.w_float(impulse);
    Packet.w_u16(aim_bullet != 0);
    Packet.w_u16(u16(hit_type & 0xffff));
    if (hit_type == ALife::eHitTypeFireWound)
    {
        Packet.w_float(ap);
    }
}
void SHit::Write_Packet(NET_Packet& Packet)
{
    Packet.w_begin(M_EVENT);
    Packet.w_u32(Time);
    Packet.w_u16(u16(PACKET_TYPE & 0xffff));
    Packet.w_u16(u16(DestID & 0xffff));

    Write_Packet_Cont(Packet);
};

#ifdef DEBUG
void SHit::_dump()
{
    Msg("SHit::_dump()---begin");
    Log("power=", power);
    Log("impulse=", impulse);
    Log("dir=", dir);
    Log("whoID=", whoID);
    Log("weaponID=", weaponID);
    Log("element=", boneID);
    Log("p_in_bone_space=", p_in_bone_space);
    Log("hit_type=", (int)hit_type);
    Log("ap=", ap);
    Msg("SHit::_dump()---end");
}
#endif

void SHit::set_hit_initiator(CScriptGameObject* script_obj)
{
    auto obj = smart_cast<CObject*>(&(script_obj->object()));
    if (!obj)
    {
        Msg("!![SHit::set_hit_initiator] CObject [%s] not found!", script_obj->Name());
        return;
    }
    who = obj;
}
CScriptGameObject* SHit::get_hit_initiator() const
{
    if (who)
    {
        auto obj = smart_cast<CGameObject*>(who);
        if (obj)
            return obj->lua_game_object();
        else
            Msg("!![SHit::get_hit_initiator] Cast failed! CObject: [%s]", who->cName().c_str());
    }
    return nullptr;
}
