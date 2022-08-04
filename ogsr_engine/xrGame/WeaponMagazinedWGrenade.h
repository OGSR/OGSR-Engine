#pragma once
#include "weaponmagazined.h"
#include "rocketlauncher.h"

class CWeaponFakeGrenade;

class CWeaponMagazinedWGrenade : public CWeaponMagazined, public CRocketLauncher
{
    typedef CWeaponMagazined inherited;

public:
    CWeaponMagazinedWGrenade(LPCSTR name = "AK74", ESoundTypes eSoundType = SOUND_TYPE_WEAPON_SUBMACHINEGUN);
    virtual ~CWeaponMagazinedWGrenade();

    virtual void Load(LPCSTR section);

    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void net_Export(CSE_Abstract* E);

    virtual void OnDrawUI();
    virtual void net_Relcase(CObject* object);

    virtual void OnH_B_Independent(bool just_before_destroy);

    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);

    virtual bool Attach(PIItem pIItem, bool b_send_event);
    virtual bool Detach(const char* item_section_name, bool b_spawn_item);
    virtual bool CanAttach(PIItem pIItem);
    virtual bool CanDetach(const char* item_section_name);
    virtual void InitAddons();
    virtual bool UseScopeTexture();
    virtual float CurrentZoomFactor();

    virtual void OnStateSwitch(u32 S, u32 oldState);

    virtual void switch2_Idle();
    virtual void switch2_Reload();
    virtual void state_Fire(float dt);
    virtual void OnShot();
    virtual void SwitchState(u32 S);
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual void ReloadMagazine();

    virtual bool Action(s32 cmd, u32 flags);

    virtual void UpdateSounds();
    virtual void StopHUDSounds();

    //переключение в режим подствольника
    virtual bool SwitchMode();
    void PerformSwitchGL();
    void OnAnimationEnd(u32 state);

    virtual bool IsNecessaryItem(const shared_str& item_sect);

    //виртуальные функции для проигрывания анимации HUD
    virtual void PlayAnimShow();
    virtual void PlayAnimHide();
    virtual void PlayAnimReload();
    virtual void PlayAnimIdle();
    virtual void PlayAnimShoot();
    virtual void PlayAnimModeSwitch();

    HUD_SOUND sndShotG;
    HUD_SOUND sndReloadG;
    HUD_SOUND sndSwitch;

    //дополнительные параметры патронов
    //для подствольника
    CWeaponAmmo* m_pAmmo2{};
    shared_str m_ammoSect2;
    xr_vector<shared_str> m_ammoTypes2;
    u32 m_ammoType2;
    int iMagazineSize2{};
    xr_vector<CCartridge> m_magazine2;
    bool m_bGrenadeMode{};

    CCartridge m_DefaultCartridge2;

    int iAmmoElapsed2{};

    virtual void UpdateGrenadeVisibility(bool visibility);

    //название косточки для гранаты подствольника в HUD
    shared_str grenade_bone_name;

    int GetAmmoElapsed2() const override { return iAmmoElapsed2; }
    bool IsGrenadeMode() const override { return m_bGrenadeMode; }
    virtual float Weight() const;

    bool IsPartlyReloading() const override
    {
        if (m_bGrenadeMode)
            return m_set_next_ammoType_on_reload == u32(-1) && iAmmoElapsed2 > 0 && !IsMisfire();
        else
            return inherited::IsPartlyReloading();
    }
};
