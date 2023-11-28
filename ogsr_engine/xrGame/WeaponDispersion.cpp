// WeaponDispersion.cpp: разбос при стрельбе
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "weapon.h"
#include "inventoryowner.h"
#include "actor.h"
#include "inventory_item_impl.h"

#include "actoreffector.h"
#include "effectorshot.h"
#include "EffectorShotX.h"

//возвращает 1, если оружие в отличном состоянии и >1 если повреждено
float CWeapon::GetConditionDispersionFactor() const
{
    if (Core.Features.test(xrCore::Feature::npc_simplified_shooting))
    {
        const CActor* actor = smart_cast<const CActor*>(H_Parent());
        if (!actor)
            return 1.f;
    }
    return (1.f + fireDispersionConditionFactor * (1.f - GetCondition()));
}

float CWeapon::GetFireDispersion(bool with_cartridge)
{
    if (!with_cartridge)
        return GetFireDispersion(1.0f);
    if (!m_magazine.empty())
        m_fCurrentCartirdgeDisp = m_magazine.back().m_kDisp;
    return GetFireDispersion(m_fCurrentCartirdgeDisp);
}

//текущая дисперсия (в радианах) оружия с учетом используемого патрона
float CWeapon::GetFireDispersion(float cartridge_k)
{
    //учет базовой дисперсии, состояние оружия и влияение патрона
    float fire_disp = fireDispersionBase * cartridge_k * GetConditionDispersionFactor();

    //вычислить дисперсию, вносимую самим стрелком
    if (auto pOwner = smart_cast<const CInventoryOwner*>(H_Parent()))
    {
        const float parent_disp = pOwner->GetWeaponAccuracy();
        fire_disp += parent_disp;
    }

    return fire_disp;
}

//////////////////////////////////////////////////////////////////////////
// Для эффекта отдачи оружия
void CWeapon::AddShotEffector()
{
    inventory_owner().on_weapon_shot_start(this);
}

void CWeapon::RemoveShotEffector()
{
    CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(H_Parent());
    if (pInventoryOwner)
        pInventoryOwner->on_weapon_shot_stop(this);
}

void CWeapon::ClearShotEffector()
{
    CInventoryOwner* pInventoryOwner = smart_cast<CInventoryOwner*>(H_Parent());
    if (pInventoryOwner)
        pInventoryOwner->on_weapon_hide(this);
};

/**
const Fvector& CWeapon::GetRecoilDeltaAngle()
{
    CActor* pActor		= smart_cast<CActor*>(H_Parent());

    CCameraShotEffector* S = NULL;
    if(pActor)
        S = smart_cast<CCameraShotEffector*>(pActor->EffectorManager().GetEffector(eCEShot));

    if(S)
    {
        S->GetDeltaAngle(m_vRecoilDeltaAngle);
        return m_vRecoilDeltaAngle;
    }
    else
    {
        m_vRecoilDeltaAngle.set(0.f,0.f,0.f);
        return m_vRecoilDeltaAngle;
    }
}
/**/