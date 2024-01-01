#pragma once
#include "weaponcustompistol.h"

class CWeaponPistol : public CWeaponCustomPistol
{
    typedef CWeaponCustomPistol inherited;

public:
    CWeaponPistol(LPCSTR name);
    virtual ~CWeaponPistol(void);

    virtual void Load(LPCSTR section);

    virtual void net_Destroy();
    virtual void OnH_B_Chield();

    virtual void OnAnimationEnd(u32 state);

    // анимации
    virtual void PlayAnimShow() override;
    virtual void PlayAnimIdleSprint() override;
    virtual void PlayAnimIdleMoving() override;
    virtual void PlayAnimIdleMovingSlow() override;
    virtual void PlayAnimIdleMovingCrouch() override;
    virtual void PlayAnimIdleMovingCrouchSlow() override;
    virtual void PlayAnimIdle() override;
    virtual void PlayAnimAim() override;
    virtual void PlayAnimHide() override;
    virtual void PlayAnimReload() override;
    virtual void PlayAnimShoot() override;

    virtual void UpdateSounds();

protected:
    virtual bool AllowFireWhileWorking() { return true; }

    virtual size_t GetWeaponTypeForCollision() const override { return Pistol; }

    HUD_SOUND sndClose;
    ESoundTypes m_eSoundClose;

    bool m_opened;
};
