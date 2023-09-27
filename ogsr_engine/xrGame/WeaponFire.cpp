// WeaponFire.cpp: implementation of the CWeapon class.
// function responsible for firing with CWeapon
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Weapon.h"
#include "ParticlesObject.h"
#include "HUDManager.h"
#include "entity.h"
#include "actor.h"

#include "actoreffector.h"
#include "effectorshot.h"

#include "level_bullet_manager.h"
#include "../xr_3da/IGame_Persistent.h"

float CWeapon::GetWeaponDeterioration() { return conditionDecreasePerShot; };

void CWeapon::FireTrace(const Fvector& P, const Fvector& D)
{
    VERIFY(m_magazine.size());

    CCartridge& l_cartridge = m_magazine.back();
    //	Msg("ammo - %s", l_cartridge.m_ammoSect.c_str());
    VERIFY(u16(-1) != l_cartridge.bullet_material_idx);
    //-------------------------------------------------------------
#pragma todo("KRodin: мне кажется, или здесь должно быть && вместо & ? Надо б посмотреть, работает ли оно вообще.")
    l_cartridge.m_flags.set(CCartridge::cfTracer, (m_bHasTracers & !!l_cartridge.m_flags.test(CCartridge::cfTracer)));
    if (m_u8TracerColorID != u8(-1))
        l_cartridge.m_u8ColorID = m_u8TracerColorID;
    //-------------------------------------------------------------
    //повысить изношенность оружия с учетом влияния конкретного патрона
    //	float Deterioration = GetWeaponDeterioration();
    //	Msg("Deterioration = %f", Deterioration);
    if (Core.Features.test(xrCore::Feature::npc_simplified_shooting))
    {
        CActor* actor = smart_cast<CActor*>(H_Parent());
        if (actor)
            ChangeCondition(-GetWeaponDeterioration() * l_cartridge.m_impair);
    }
    else
        ChangeCondition(-GetWeaponDeterioration() * l_cartridge.m_impair);

    float fire_disp = GetFireDispersion(true);

    bool SendHit = SendHitAllowed(H_Parent());
    //выстерлить пулю (с учетом возможной стрельбы дробью)
    for (int i = 0; i < l_cartridge.m_buckShot; ++i)
    {
        FireBullet(P, D, fire_disp, l_cartridge, H_Parent()->ID(), ID(), SendHit);
    }

    StartShotParticles();

    if (m_bLightShotEnabled)
        Light_Start();

    const Fvector ShotPos = Fvector().mad(P, D, 1.5f);
    g_pGamePersistent->GrassBendersAddShot(cast_game_object()->ID(), ShotPos, D, 3.0f, 20.0f, ps_ssfx_int_grass_params_2.z, ps_ssfx_int_grass_params_2.w);

    // Ammo
    m_magazine.pop_back();
    --iAmmoElapsed;

    //проверить не произошла ли осечка
    // CheckForMisfire();

    VERIFY((u32)iAmmoElapsed == m_magazine.size());
}

void CWeapon::Fire2Start() { bWorking2 = true; }
void CWeapon::Fire2End()
{
    //принудительно останавливать зацикленные партиклы
    if (m_pFlameParticles2 && m_pFlameParticles2->IsLooped())
        StopFlameParticles2();

    bWorking2 = false;
}

void CWeapon::StopShooting()
{
    // m_bPending = true;

    //принудительно останавливать зацикленные партиклы
    if (m_pFlameParticles && m_pFlameParticles->IsLooped())
        StopFlameParticles();

    if (!dont_interrupt_shot_anm)
        SwitchState(eIdle);

    bWorking = false;
    // if(IsWorking()) FireEnd();
}

void CWeapon::FireEnd()
{
    CShootingObject::FireEnd();
    ClearShotEffector();
}

void CWeapon::StartFlameParticles2() { CShootingObject::StartParticles(m_pFlameParticles2, *m_sFlameParticles2, get_LastFP2()); }
void CWeapon::StopFlameParticles2() { CShootingObject::StopParticles(m_pFlameParticles2); }
void CWeapon::UpdateFlameParticles2()
{
    if (m_pFlameParticles2)
        CShootingObject::UpdateParticles(m_pFlameParticles2, get_LastFP2());
}
