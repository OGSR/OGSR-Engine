#include "stdafx.h"
#include "radioactivezone.h"
#include "hudmanager.h"
#include "level.h"
#include "xrmessages.h"
#include "clsid_game.h"
#include "game_base_space.h"
#include "Hit.h"
#include "../xr_3da/bone.h"

CRadioactiveZone::CRadioactiveZone(void) {}

CRadioactiveZone::~CRadioactiveZone(void) {}

void CRadioactiveZone::Load(LPCSTR section) { inherited::Load(section); }

bool CRadioactiveZone::BlowoutState()
{
    bool result = inherited::BlowoutState();
    if (!result)
        UpdateBlowout();
    return result;
}

void CRadioactiveZone::Affect(SZoneObjectInfo* O)
{
    // вермя срабатывания не чаще, чем заданный период
    if (m_dwDeltaTime < m_dwPeriod)
        return;
    //.	m_dwDeltaTime = 0;

    CGameObject* GO = O->object;

    if (GO)
    {
        Fvector pos;
        XFORM().transform_tiny(pos, CFORM()->getSphere().P);

#ifdef DEBUG
        char pow[255];
        sprintf_s(pow, "zone hit. %.3f", Power(GO->Position().distance_to(pos)));
        if (bDebug)
            Msg("%s %s", *GO->cName(), pow);
#endif

        Fvector dir;
        dir.set(0, 0, 0);

        Fvector position_in_bone_space;
        float power = Power(GO->Position().distance_to(pos));
        float impulse = 0.f;
        if (power > EPS)
        {
            //.			m_dwDeltaTime = 0;
            position_in_bone_space.set(0.f, 0.f, 0.f);

            CreateHit(GO->ID(), ID(), dir, power, BI_NONE, position_in_bone_space, impulse, ALife::eHitTypeRadiation);
        }
    }
}

void CRadioactiveZone::feel_touch_new(CObject* O) { inherited::feel_touch_new(O); }

#include "actor.h"
BOOL CRadioactiveZone::feel_touch_contact(CObject* O)
{
    CActor* A = smart_cast<CActor*>(O);
    if (A)
    {
        if (!((CCF_Shape*)CFORM())->Contact(O))
            return FALSE;
        return A->feel_touch_on_contact(this);
    }
    else
        return FALSE;
}

void CRadioactiveZone::UpdateWorkload(u32 dt) { inherited::UpdateWorkload(dt); }