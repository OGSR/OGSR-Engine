#pragma once
#include "missile.h"
#include "DamageSource.h"
class CBolt : public CMissile, public IDamageSource
{
    typedef CMissile inherited;
    u16 m_thrower_id;

public:
    CBolt(void);
    virtual ~CBolt(void);

    virtual void OnH_A_Chield();
    virtual void OnEvent(NET_Packet& P, u16 type);

    virtual bool Activate(bool = false);
    virtual void Deactivate(bool = false);

    virtual void SetInitiator(u16 id);
    virtual u16 Initiator();

    virtual void Throw();
    virtual bool Action(s32 cmd, u32 flags);
    virtual bool Useful() const;
    virtual void Destroy();
    virtual void activate_physic_shell();
    virtual void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);

    virtual BOOL UsedAI_Locations() { return FALSE; }
    virtual IDamageSource* cast_IDamageSource() { return this; }

    virtual bool StopSprintOnFire() { return false; }

protected:
    virtual size_t GetWeaponTypeForCollision() const override { return Bolt; }
};
