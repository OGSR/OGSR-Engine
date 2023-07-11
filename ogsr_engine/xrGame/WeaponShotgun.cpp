#include "stdafx.h"
#include "weaponshotgun.h"
#include "entity.h"
#include "ParticlesObject.h"
#include "xr_level_controller.h"
#include "inventory.h"
#include "level.h"
#include "actor.h"

CWeaponShotgun::CWeaponShotgun(void) : CWeaponCustomPistol("TOZ34")
{
    m_eSoundShotBoth = ESoundTypes(SOUND_TYPE_WEAPON_SHOOTING);
    m_eSoundClose = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
    m_eSoundAddCartridge = ESoundTypes(SOUND_TYPE_WEAPON_RECHARGING);
    m_bLockType = true; // Запрещает заряжать в дробовики патроны разного типа
}

CWeaponShotgun::~CWeaponShotgun(void)
{
    // sounds
    HUD_SOUND::DestroySound(sndShotBoth);
    HUD_SOUND::DestroySound(m_sndOpen);
    HUD_SOUND::DestroySound(m_sndAddCartridge);
    HUD_SOUND::DestroySound(m_sndAddCartridgeEmpty);
    HUD_SOUND::DestroySound(m_sndClose);
    HUD_SOUND::DestroySound(m_sndCloseEmpty);
    HUD_SOUND::DestroySound(m_sndBreech);
    HUD_SOUND::DestroySound(m_sndBreechJammed);
}

void CWeaponShotgun::net_Destroy() { inherited::net_Destroy(); }

void CWeaponShotgun::Load(LPCSTR section)
{
    inherited::Load(section);

    // Звук и анимация для выстрела дуплетом
    HUD_SOUND::LoadSound(section, "snd_shoot_duplet", sndShotBoth, m_eSoundShotBoth);

    if (pSettings->line_exist(section, "tri_state_reload"))
    {
        m_bTriStateReload = !!pSettings->r_bool(section, "tri_state_reload");
    }

    if (m_bTriStateReload)
    {
        HUD_SOUND::LoadSound(section, "snd_open_weapon", m_sndOpen, m_eSoundOpen);
        HUD_SOUND::LoadSound(section, "snd_add_cartridge", m_sndAddCartridge, m_eSoundAddCartridge);
        if (pSettings->line_exist(section, "snd_add_cartridge_empty"))
            HUD_SOUND::LoadSound(section, "snd_add_cartridge_empty", m_sndAddCartridgeEmpty, m_eSoundAddCartridge);
        HUD_SOUND::LoadSound(section, "snd_close_weapon", m_sndClose, m_eSoundClose);
        if (pSettings->line_exist(section, "snd_close_weapon_empty"))
            HUD_SOUND::LoadSound(section, "snd_close_weapon_empty", m_sndCloseEmpty, m_eSoundClose);
        if (pSettings->line_exist(section, "snd_breechblock"))
            HUD_SOUND::LoadSound(section, "snd_breechblock", m_sndBreech, m_eSoundClose);
        if (pSettings->line_exist(section, "snd_jam"))
            HUD_SOUND::LoadSound(section, "snd_jam", m_sndBreechJammed, m_eSoundClose);
    }
}

void CWeaponShotgun::OnShot()
{
    inherited::OnShot();

    if (/*!m_sndBreechJammed.sounds.empty() ||*/ !m_sndBreech.sounds.empty())
        PlaySound(/*(IsMisfire() && !m_sndBreechJammed.sounds.empty()) ? m_sndBreechJammed :*/ m_sndBreech, get_LastFP());
}

void CWeaponShotgun::Fire2Start()
{
    if (IsPending())
        return;

    inherited::Fire2Start();

    if (IsValid())
    {
        if (!IsWorking())
        {
            if (GetState() == eReload)
                return;
            if (GetState() == eShowing)
                return;
            if (GetState() == eHiding)
                return;

            CWeapon::FireStart();

            SwitchState((iAmmoElapsed < iMagazineSize) ? eFire : eFire2);
        }
    }
    else
        SwitchState(eMagEmpty);
}

void CWeaponShotgun::Fire2End()
{
    inherited::Fire2End();
    FireEnd();
}

void CWeaponShotgun::OnShotBoth()
{
    //если патронов меньше, чем 2
    if (iAmmoElapsed < iMagazineSize)
    {
        OnShot();
        return;
    }

    //звук выстрела дуплетом
    PlaySound(sndShotBoth, get_LastFP());

    // Camera
    AddShotEffector();

    // анимация дуплета
    PlayHUDMotion({"anim_shoot_both", "anm_shots_both"}, false, GetState());

    // Shell Drop
    Fvector vel;
    PHGetLinearVell(vel);
    OnShellDrop(get_LastSP(), vel);

    //огонь из 2х стволов
    StartFlameParticles();
    StartFlameParticles2();

    //дым из 2х стволов
    if (ParentIsActor())
    {
        CParticlesObject* pSmokeParticles = NULL;
        CShootingObject::StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, get_LastFP(), zero_vel, true);
        pSmokeParticles = NULL;
        CShootingObject::StartParticles(pSmokeParticles, *m_sSmokeParticlesCurrent, get_LastFP2(), zero_vel, true);
    }
}

void CWeaponShotgun::UpdateCL()
{
    float dt = Device.fTimeDelta;

    //когда происходит апдейт состояния оружия
    //ничего другого не делать
    if (GetNextState() == GetState())
    {
        switch (GetState())
        {
        case eFire2:
            // if (iAmmoElapsed > 0)
            //	state_Fire(dt);

            if (fTime <= 0)
            {
                if (iAmmoElapsed == 0)
                    OnMagazineEmpty();
                StopShooting();
            }
            else
            {
                fTime -= dt;
            }

            break;
        }
    }

    inherited::UpdateCL();
}

void CWeaponShotgun::switch2_Fire()
{
    SetPending(TRUE);
    inherited::switch2_Fire();
}

void CWeaponShotgun::switch2_Fire2()
{
    VERIFY(fTimeToFire > 0.f);

    if (fTime <= 0)
    {
        SetPending(TRUE);

        // Fire
        Fvector p1, d;
        p1.set(get_LastFP());
        d.set(get_LastFD());

        CEntity* E = smart_cast<CEntity*>(H_Parent());
        if (E)
        {
#ifdef DEBUG
            CInventoryOwner* io = smart_cast<CInventoryOwner*>(H_Parent());
            if (NULL == io->inventory().ActiveItem())
            {
                Log("current_state", GetState());
                Log("next_state", GetNextState());
                Log("state_time", m_dwStateTime);
                Log("item_sect", cNameSect().c_str());
                Log("H_Parent", H_Parent()->cNameSect().c_str());
            }
#endif
            E->g_fireParams(this, p1, d);
        }

        OnShotBoth();

        //выстрел из обоих стволов
        FireTrace(p1, d);
        FireTrace(p1, d);
        fTime += fTimeToFire * 2.f;

        // Patch for "previous frame position" :)))
        dwFP_Frame = 0xffffffff;
        dwXF_Frame = 0xffffffff;
    }
}

void CWeaponShotgun::UpdateSounds()
{
    inherited::UpdateSounds();
    if (sndShotBoth.playing())
        sndShotBoth.set_position(get_LastFP());
    if (m_sndOpen.playing())
        m_sndOpen.set_position(get_LastFP());
    if (m_sndAddCartridge.playing())
        m_sndAddCartridge.set_position(get_LastFP());
    if (m_sndAddCartridgeEmpty.playing())
        m_sndAddCartridgeEmpty.set_position(get_LastFP());
    if (m_sndClose.playing())
        m_sndClose.set_position(get_LastFP());
    if (m_sndCloseEmpty.playing())
        m_sndCloseEmpty.set_position(get_LastFP());
    if (m_sndBreech.playing())
        m_sndBreech.set_position(get_LastFP());
    if (m_sndBreechJammed.playing())
        m_sndBreechJammed.set_position(get_LastFP());
}

#ifdef DUPLET_STATE_SWITCH
void CWeaponShotgun::SwitchDuplet() { is_duplet_enabled = !is_duplet_enabled; }
#endif

bool CWeaponShotgun::Action(s32 cmd, u32 flags)
{
#ifdef DUPLET_STATE_SWITCH

    if (is_duplet_enabled)
    {
        switch (cmd)
        {
        case kWPN_FIRE: {
            if (flags & CMD_START)
            {
                if (IsPending())
                    return false;
                Fire2Start();
            }
            else
                Fire2End();

            return true;
        }
        }
    }

#endif // !DUPLET_STATE_SWITCH

    if (inherited::Action(cmd, flags))
        return true;

    if (m_bTriStateReload && GetState() == eReload && !IsMisfire() && (flags & CMD_START) && (m_sub_state == eSubstateReloadInProcess || m_sub_state == eSubstateReloadBegin))
    {
        switch (cmd)
        {
        case kWPN_FIRE:
        case kWPN_NEXT:
        //case kWPN_RELOAD:
        case kWPN_ZOOM:
            // остановить перезарядку
            m_stop_triStateReload = true;
            return true;
        }
    }

#ifndef DUPLET_STATE_SWITCH

    //если оружие чем-то занято, то ничего не делать
    if (IsPending())
        return false;

    switch (cmd)
    {
    case kWPN_ZOOM: {
        if (flags & CMD_START)
            Fire2Start();
        else
            Fire2End();
    }
        return true;
    }

#endif // !DUPLET_STATE_SWITCH

    return false;
}

void CWeaponShotgun::OnAnimationEnd(u32 state)
{
    if (!m_bTriStateReload || state != eReload)
        return inherited::OnAnimationEnd(state);

    auto ProcessReloadEnd = [this] {
        if (IsMisfire())
        {
            SwitchMisfire(false);
            if (iAmmoElapsed > 0) //
                SetAmmoElapsed(iAmmoElapsed - 1); //
        }
        m_sub_state = eSubstateReloadBegin;
        SwitchState(eIdle);
    };

    switch (m_sub_state)
    {
    case eSubstateReloadBegin: {
        if (IsMisfire() && has_anm_reload_jammed)
            ProcessReloadEnd();
        else
        {
            m_sub_state = IsMisfire() ? eSubstateReloadEnd : eSubstateReloadInProcess;
            is_reload_empty = iAmmoElapsed == 0;
            SwitchState(eReload);
        }
        break;
    }
    case eSubstateReloadInProcess: {
        AddCartridge(1);
        if (m_stop_triStateReload || !HaveCartridgeInInventory(1) || m_magazine.size() >= iMagazineSize)
            m_sub_state = eSubstateReloadEnd;

        SwitchState(eReload);
        break;
    }
    case eSubstateReloadEnd: {
        ProcessReloadEnd();
        break;
    }
    };
}

void CWeaponShotgun::Reload()
{
    OnZoomOut();
    if (m_bTriStateReload)
    {
        m_stop_triStateReload = false;
        TriStateReload();
    }
    else
        TryReload();
}

void CWeaponShotgun::TriStateReload()
{
    if (HaveCartridgeInInventory(1) || IsMisfire())
    {
        m_sub_state = eSubstateReloadBegin;
        SwitchState(eReload);
    }
}

void CWeaponShotgun::OnStateSwitch(u32 S, u32 oldState)
{
    if (!m_bTriStateReload || S != eReload)
    {
        inherited::OnStateSwitch(S, oldState);
        return;
    }

    CWeapon::OnStateSwitch(S, oldState);

    switch (m_sub_state)
    {
    case eSubstateReloadBegin: {
        if (HaveCartridgeInInventory(1) || IsMisfire())
        {
            if (IsMisfire())
            {
                const char* Anm = iAmmoElapsed == 1 ? "anm_reload_jammed_last" : "anm_reload_jammed";
                has_anm_reload_jammed = AnimationExist(Anm);
                if (has_anm_reload_jammed)
                {
                    if (iAmmoElapsed == 1 && !sndReloadJammedLast.sounds.empty())
                        PlaySound(sndReloadJammedLast, get_LastFP());
                    else if (!sndReloadJammed.sounds.empty())
                        PlaySound(sndReloadJammed, get_LastFP());

                    PlayHUDMotion(Anm, true, GetState());
                    SetPending(TRUE);
                    break;
                }
            }
            PlaySound(m_sndOpen, get_LastFP());
            if (ParentIsActor())
                PlayHUDMotion({"anim_open_weapon", "anm_open"}, true, GetState());
            else //Временно заткнул баг с неперезарядкой винчестера у нпс, надо фиксить анимацию, но это будет сделано позже, потом этот код убрать!
                PlayHUDMotion({"anim_add_cartridge", "anm_add_cartridge"}, true, GetState());
            SetPending(TRUE);
        }
        break;
    }
    case eSubstateReloadInProcess: {
        if (HaveCartridgeInInventory(1))
        {
            PlaySound(iAmmoElapsed == 0 && !m_sndAddCartridgeEmpty.sounds.empty() ? m_sndAddCartridgeEmpty : m_sndAddCartridge, get_LastFP());
            PlayHUDMotion({iAmmoElapsed == 0 ? "anm_add_cartridge_empty" : "nullptr", "anim_add_cartridge", "anm_add_cartridge"}, true, GetState());
            SetPending(TRUE);
        }
        break;
    }
    case eSubstateReloadEnd: {
        PlayHUDMotion({IsMisfire() ? "anm_close_jammed" : (is_reload_empty ? "anm_close_empty" : "nullptr"), "anim_close_weapon", "anm_close"}, true, GetState());
        PlaySound(((IsMisfire() || is_reload_empty) && !m_sndCloseEmpty.sounds.empty()) ? m_sndCloseEmpty : m_sndClose, get_LastFP());
        SetPending(TRUE);
        break;
    }
    };
}

bool CWeaponShotgun::HaveCartridgeInInventory(u8 cnt)
{
    if (unlimited_ammo())
        return true;
    if (!m_pCurrentInventory)
        return false;

    if (m_set_next_ammoType_on_reload != u32(-1))
    {
        m_ammoType = m_set_next_ammoType_on_reload;
        m_set_next_ammoType_on_reload = u32(-1);
        if (!m_magazine.empty())
            UnloadMagazine();
    }

    u32 ac = GetAmmoCount(m_ammoType, cnt);
    if (ac == 0 && (m_magazine.empty() || !m_bLockType))
    {
        u8 skip_ammo_type = m_ammoType;
        for (u8 i = 0; i < u8(m_ammoTypes.size()); ++i)
        {
            if (i == skip_ammo_type)
                continue;
            ac = GetAmmoCount(i, cnt);
            if (ac >= cnt)
            {
                m_ammoType = i;
                break;
            }
            else if (ac > 0 && !m_ammoType)
                m_ammoType = i;
        }
    }
    m_pAmmo = smart_cast<CWeaponAmmo*>(m_pCurrentInventory->GetAmmoMinCurr(*m_ammoTypes[m_ammoType], ParentIsActor()));

    return (m_pAmmo && ac >= cnt);
}

u8 CWeaponShotgun::AddCartridge(u8 cnt)
{
    if (m_set_next_ammoType_on_reload != u32(-1))
    {
        m_ammoType = m_set_next_ammoType_on_reload;
        m_set_next_ammoType_on_reload = u32(-1);
    }

    if (m_magazine.size() >= (u32)iMagazineSize || !HaveCartridgeInInventory(1))
        return cnt;

    VERIFY((u32)iAmmoElapsed == m_magazine.size());

    if (m_DefaultCartridge.m_LocalAmmoType != m_ammoType)
        m_DefaultCartridge.Load(*m_ammoTypes[m_ammoType], u8(m_ammoType));

    CCartridge l_cartridge = m_DefaultCartridge;
    while (cnt && m_magazine.size() < (u32)iMagazineSize) // && m_pAmmo->Get(l_cartridge))
    {
        if (!unlimited_ammo())
        {
            if (!m_pAmmo->Get(l_cartridge))
                break; //-V595
        }
        --cnt;
        ++iAmmoElapsed;
        l_cartridge.m_LocalAmmoType = u8(m_ammoType);
        m_magazine.push_back(l_cartridge);
    }

    VERIFY((u32)iAmmoElapsed == m_magazine.size());

    //выкинуть коробку патронов, если она пустая
    if (m_pAmmo && !m_pAmmo->m_boxCurr)
        m_pAmmo->SetDropManual(TRUE);

    return cnt;
}

void CWeaponShotgun::net_Export(CSE_Abstract* E)
{
    inherited::net_Export(E);
    CSE_ALifeItemWeaponShotGun* sg = smart_cast<CSE_ALifeItemWeaponShotGun*>(E);
    sg->m_AmmoIDs.clear();
    for (u32 i = 0; i < m_magazine.size(); i++)
    {
        CCartridge& l_cartridge = *(m_magazine.begin() + i);
        sg->m_AmmoIDs.push_back(l_cartridge.m_LocalAmmoType);
    }
}

void CWeaponShotgun::TryReload()
{
    if (m_pCurrentInventory)
    {
        if (HaveCartridgeInInventory(1) || unlimited_ammo() || (IsMisfire() && iAmmoElapsed))
        {
            SetPending(TRUE);
            SwitchState(eReload);
            return;
        }
    }
}

void CWeaponShotgun::ReloadMagazine()
{ //Используется только при отключенном tri_state_reload
    m_dwAmmoCurrentCalcFrame = 0;

    if (IsMisfire())
        SwitchMisfire(false);
    else
    {
        if (!m_pCurrentInventory)
            return;

        u8 cnt = AddCartridge(1);
        while (cnt == 0)
            cnt = AddCartridge(1);
    }
}

void CWeaponShotgun::StopHUDSounds()
{
    HUD_SOUND::StopSound(m_sndOpen);
    HUD_SOUND::StopSound(m_sndAddCartridge);
    HUD_SOUND::StopSound(m_sndAddCartridgeEmpty);
    HUD_SOUND::StopSound(m_sndClose);
    HUD_SOUND::StopSound(m_sndCloseEmpty);
    HUD_SOUND::StopSound(m_sndBreech);
    HUD_SOUND::StopSound(m_sndBreechJammed);

    inherited::StopHUDSounds();
}
