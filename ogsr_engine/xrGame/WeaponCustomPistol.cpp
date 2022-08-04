#include "stdafx.h"

#include "Entity.h"
#include "WeaponCustomPistol.h"
#include "game_object_space.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CWeaponCustomPistol::CWeaponCustomPistol(LPCSTR name) : CWeaponMagazined(name, SOUND_TYPE_WEAPON_PISTOL) {}

CWeaponCustomPistol::~CWeaponCustomPistol() {}
void CWeaponCustomPistol::switch2_Fire()
{
    if (GetCurrentFireMode() == 1)
    {
        m_bFireSingleShot = true;
        bWorking = true;
        m_iShotNum = 0;
        m_bStopedAfterQueueFired = false;
    }
    else
    {
        inherited::switch2_Fire();
    }
}

void CWeaponCustomPistol::FireEnd()
{
    // if (fTime <= 0 && GetCurrentFireMode() == 1)
    //{
    //	m_bPending = false;
    // }

    inherited::FireEnd();
}

void CWeaponCustomPistol::net_Relcase(CObject* object) { inherited::net_Relcase(object); }

void CWeaponCustomPistol::OnDrawUI() { inherited::OnDrawUI(); }

void CWeaponCustomPistol::net_Destroy() { inherited::net_Destroy(); }