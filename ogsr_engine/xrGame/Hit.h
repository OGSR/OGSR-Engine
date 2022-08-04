#pragma once

class CScriptGameObject;

struct SHit
{
    SHit(float Power, Fvector& dir, CObject* who, u16 element, Fvector p_in_object_space, float impulse, ALife::EHitType hit_type, float ap = 0.0f, bool AimBullet = false);
    SHit();
    bool is_valide() const;
    void invalidate();
    IC float damage() const
    {
        VERIFY(is_valide());
        return power;
    }
    IC const Fvector& direction() const
    {
        VERIFY(is_valide());
        return dir;
    }
    IC const CObject* initiator() const
    {
        VERIFY(is_valide());
        return who;
    }
    IC u16 bone() const
    {
        VERIFY(is_valide());
        return boneID;
    }
    IC const Fvector& bone_space_position() const
    {
        VERIFY(is_valide());
        return p_in_bone_space;
    }
    IC float phys_impulse() const
    {
        VERIFY(is_valide());
        return impulse;
    }
    IC ALife::EHitType type() const
    {
        VERIFY(is_valide());
        return hit_type;
    }
    void Read_Packet(NET_Packet P);
    void Read_Packet_Cont(NET_Packet P);
    void Write_Packet(NET_Packet& P);
    void Write_Packet_Cont(NET_Packet& P);

    void GenHeader(u16 PacketType, u16 ID);
    // private:
    // GE_HIT
    u32 Time{};
    u16 PACKET_TYPE;
    u16 DestID{};

    float power;
    Fvector dir;
    CObject* who;
    void set_hit_initiator(CScriptGameObject* script_obj);
    CScriptGameObject* get_hit_initiator() const;
    u16 whoID;
    u16 weaponID{};
    u16 boneID;
    Fvector p_in_bone_space;
    float impulse;
    ALife::EHitType hit_type;
    float ap;
    bool aim_bullet;
    //---------------------------------------------------
    // GE_HIT_STATISTIC
    u32 BulletID;
    u32 SenderID;

    bool ignore_flag = false;
    // Начальное значение хита, до обработок всякими защитами
    // артефактов и броней.
    float full_power;

#ifdef DEBUG
    void _dump();
#endif
};
