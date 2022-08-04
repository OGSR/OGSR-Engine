#pragma once

#include "WeaponCustomPistol.h"
#include "script_export_space.h"

class CWeaponKnife : public CWeapon
{
private:
    typedef CWeapon inherited;

protected:
    HUD_SOUND m_sndShot;
    HUD_SOUND sndItemOn;

    bool m_attackStart;
    bool m_attackMotionMarksAvailable;

    bool HeadLampSwitch{}, NightVisionSwitch{};

protected:
    virtual void switch2_Idle();
    virtual void switch2_Hiding();
    virtual void switch2_Hidden();
    virtual void switch2_Showing();
    void switch2_Attacking(u32 state);

    virtual void OnMotionMark(u32 state, const motion_marks& M);
    virtual void OnAnimationEnd(u32 state);
    virtual void OnStateSwitch(u32 S, u32 oldState);

    virtual void DeviceUpdate() override;
    virtual void UpdateCL() override;
    virtual void PlayAnimDeviceSwitch() override;

    void state_Attacking(float dt);

    virtual void KnifeStrike(u32 state, const Fvector& pos, const Fvector& dir);

    float fWallmarkSize;
    u16 knife_material_idx;

protected:
    ALife::EHitType m_eHitType_1;
    // float				fHitPower_1;
    Fvector4 fvHitPower_1;
    float fHitImpulse_1;

    ALife::EHitType m_eHitType_2;
    // float				fHitPower_2;
    Fvector4 fvHitPower_2;
    float fHitImpulse_2;
    virtual void LoadFireParams(LPCSTR section, LPCSTR prefix);

    virtual size_t GetWeaponTypeForCollision() const override { return Knife_and_other; }
    // TODO: рассчитать здесь позицию для коллизии
    virtual Fvector GetPositionForCollision() override { return Device.vCameraPosition; }
    virtual Fvector GetDirectionForCollision() override { return Device.vCameraDirection; }

public:
    CWeaponKnife();
    virtual ~CWeaponKnife();

    void Load(LPCSTR section);

    virtual void Fire2Start();
    virtual void FireStart();

    virtual bool Action(s32 cmd, u32 flags);

    virtual void GetBriefInfo(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count);

    DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CWeaponKnife)
#undef script_type_list
#define script_type_list save_type_list(CWeaponKnife)
