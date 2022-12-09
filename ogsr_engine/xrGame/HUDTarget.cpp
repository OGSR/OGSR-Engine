// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:13:00 , by user : Oles , from computer : OLES
// HUDCursor.cpp: implementation of the CHUDTarget class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hudtarget.h"
#include "hudmanager.h"
#include "../xr_3da/GameMtlLib.h"

#include "..\xr_3da\Environment.h"
#include "..\xr_3da\CustomHUD.h"
#include "Actor.h"
#include "level.h"
#include "game_cl_base.h"
#include "..\xr_3da\IGame_Persistent.h"

#include "InventoryOwner.h"
#include "relation_registry.h"
#include "character_info.h"

#include "string_table.h"
#include "entity_alive.h"

#include "inventory_item.h"
#include "inventory.h"
#include "monster_community.h"
#include "HudItem.h"
#include "Weapon.h"
#include "PDA.h"

constexpr float C_SIZE = 0.025f, NEAR_LIM = 0.5f, SHOW_INFO_SPEED = 0.5f, HIDE_INFO_SPEED = 10.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUDTarget::CHUDTarget()
{
    fuzzyShowInfo = 0.f;
    RQ.range = 0.f;

    hShader->create("hud\\cursor", "ui\\cursor");

    RQ.set(NULL, 0.f, -1);

    Load();
    m_bShowCrosshair = false;
}

void CHUDTarget::net_Relcase(CObject* O)
{
    if (RQ.O == O)
        RQ.O = NULL;

    RQR.r_clear();
}

void CHUDTarget::Load() { HUDCrosshair.Load(); }

float CHUDTarget::GetDist() { return RQ.range; }

CObject* CHUDTarget::GetObj() { return RQ.O; }

ICF static BOOL pick_trace_callback(collide::rq_result& result, LPVOID params)
{
    collide::rq_result* RQ = (collide::rq_result*)params;
    if (result.O)
    {
        *RQ = result;
        return FALSE;
    }
    else
    {
        //получить треугольник и узнать его материал
        CDB::TRI* T = Level().ObjectSpace.GetStaticTris() + result.element;
        const auto* mtl = GMLib.GetMaterialByIdx(T->material);
        if (mtl->Flags.is(SGameMtl::flPassable))
            return TRUE;
        // возможно это сетка-рабица и через нее можно брать предметы
        else if (fsimilar(mtl->fVisTransparencyFactor, 1.0f, EPS) && fsimilar(mtl->fShootFactor, 1.0f, EPS) && mtl->Flags.is(SGameMtl::flSuppressWallmarks))
            return TRUE;
    }
    *RQ = result;
    return FALSE;
}

void CHUDTarget::CursorOnFrame()
{
    CActor* Actor = smart_cast<CActor*>(Level().CurrentEntity());
    if (!Actor)
        return;

    Fvector p1 = Device.vCameraPosition;
    Fvector dir = Device.vCameraDirection;

    if (auto Wpn = smart_cast<CHudItem*>(Actor->inventory().ActiveItem()))
        Actor->g_fireParams(Wpn, p1, dir, true);

    // Render cursor
    RQ.O = nullptr;
    RQ.range = g_pGamePersistent->Environment().CurrentEnv->far_plane * 0.99f;
    RQ.element = -1;

    collide::ray_defs RD(p1, dir, RQ.range, CDB::OPT_CULL, collide::rqtBoth);
    RQR.r_clear();
    VERIFY(!fis_zero(RD.dir.square_magnitude()));
    if (Level().ObjectSpace.RayQuery(RQR, RD, pick_trace_callback, &RQ, nullptr, Level().CurrentEntity()))
        clamp(RQ.range, NEAR_LIM, RQ.range);
}

extern ENGINE_API BOOL g_bRendering;

#include <ai/monsters/poltergeist/poltergeist.h>

void CHUDTarget::Render()
{
    VERIFY(g_bRendering);

    CActor* Actor = smart_cast<CActor*>(Level().CurrentEntity());
    if (!Actor)
        return;

    if (smart_cast<CPda*>(Actor->inventory().ActiveItem()))
        return;

    Fvector p1 = Device.vCameraPosition;
    Fvector dir = Device.vCameraDirection;

    if (auto Wpn = smart_cast<CHudItem*>(Actor->inventory().ActiveItem()))
        Actor->g_fireParams(Wpn, p1, dir, true);

    // Render cursor
    u32 C = C_DEFAULT;

    Fvector p2;
    p2.mad(p1, dir, RQ.range);
    Fvector4 pt;
    Device.mFullTransform.transform(pt, p2);
    pt.y = -pt.y;
    float di_size = C_SIZE / powf(pt.w, .2f);

    CGameFont* F = HUD().Font().pFontGraffiti19Russian;
    F->SetAligment(CGameFont::alCenter);
    F->OutSetI(0.f, 0.05f);

    if (psHUD_Flags.test(HUD_CROSSHAIR_DIST))
    {
        F->SetColor(C);
        F->OutNext("%4.1f", RQ.range);
    }

    if (psHUD_Flags.test(HUD_INFO))
    {
        const bool is_poltergeist = RQ.O && !!smart_cast<CPoltergeist*>(RQ.O);

        if ((RQ.O && RQ.O->getVisible()) || is_poltergeist)
        {
            CEntityAlive* E = smart_cast<CEntityAlive*>(RQ.O);
            CEntityAlive* pCurEnt = smart_cast<CEntityAlive*>(Level().CurrentEntity());
            PIItem l_pI = smart_cast<PIItem>(RQ.O);

            CInventoryOwner* our_inv_owner = smart_cast<CInventoryOwner*>(pCurEnt);
            if (/*psHUD_Flags.test(HUD_INFO_MONSTER) &&*/ E && E->g_Alive() && E->cast_base_monster())
            {
                int relation = MONSTER_COMMUNITY::relation(pCurEnt->monster_community->index(), E->monster_community->index());

                if (relation > 0)
                    C = C_ON_FRIEND;
                else if (relation == 0)
                    C = C_ON_NEUTRAL;
                else
                    C = C_ON_ENEMY;
            }
            else if (E && E->g_Alive() && !E->cast_base_monster())
            {
                CInventoryOwner* others_inv_owner = smart_cast<CInventoryOwner*>(E);

                if (our_inv_owner && others_inv_owner)
                {
                    switch (RELATION_REGISTRY().GetRelationType(others_inv_owner, our_inv_owner))
                    {
                    case ALife::eRelationTypeEnemy: C = C_ON_ENEMY; break;
                    case ALife::eRelationTypeNeutral: C = C_ON_NEUTRAL; break;
                    case ALife::eRelationTypeFriend: C = C_ON_FRIEND; break;
                    }

                    if (fuzzyShowInfo > 0.5f)
                    {
                        CStringTable strtbl;
                        F->SetColor(subst_alpha(C, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));
                        F->OutNext("%s", *strtbl.translate(others_inv_owner->Name()));
                        F->OutNext("%s", *strtbl.translate(others_inv_owner->CharacterInfo().Community().id()));
                    }
                }

                fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
            }
            else if (l_pI && our_inv_owner && RQ.range < 2.0f * our_inv_owner->inventory().GetTakeDist())
            {
                if (fuzzyShowInfo > 0.5f)
                {
                    F->SetColor(subst_alpha(C, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));
                    F->OutNext("%s", l_pI->Name /*Complex*/ ());
                }
                fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
            }
        }
        else
        {
            fuzzyShowInfo -= HIDE_INFO_SPEED * Device.fTimeDelta;
        }
        clamp(fuzzyShowInfo, 0.f, 1.f);
    }

    if (auto Wpn = smart_cast<CWeapon*>(Actor->inventory().ActiveItem()); Wpn && (Wpn->IsLaserOn() || Wpn->GetState() == CHUDState::EHudStates::eReload))
        return;

    if (const u32 State = Actor->get_state() & mcSprint)
        return;

    Fvector2 scr_size;
    scr_size.set(float(Device.dwWidth), float(Device.dwHeight));
    float size_x = scr_size.x * di_size;
    float size_y = scr_size.y * di_size;

    size_y = size_x;

    float w_2 = scr_size.x / 2.0f;
    float h_2 = scr_size.y / 2.0f;

    // Convert to screen coords
    float cx = (pt.x + 1) * w_2;
    float cy = (pt.y + 1) * h_2;

    //отрендерить кружочек или крестик
    if (!m_bShowCrosshair)
    {
        // actual rendering
        UIRender->StartPrimitive(6, IUIRender::ptTriList, UI()->m_currentPointType);

        //	TODO: return code back to indexed rendering since we use quads
        //	Tri 1
        UIRender->PushPoint(cx - size_x, cy + size_y, 0, C, 0, 1);
        UIRender->PushPoint(cx - size_x, cy - size_y, 0, C, 0, 0);
        UIRender->PushPoint(cx + size_x, cy + size_y, 0, C, 1, 1);
        //	Tri 2
        UIRender->PushPoint(cx + size_x, cy + size_y, 0, C, 1, 1);
        UIRender->PushPoint(cx - size_x, cy - size_y, 0, C, 0, 0);
        UIRender->PushPoint(cx + size_x, cy - size_y, 0, C, 1, 0);

        // unlock VB and Render it as triangle LIST
        UIRender->SetShader(*hShader);
        UIRender->FlushPrimitive();
    }
    else
    {
        //отрендерить прицел
        HUDCrosshair.cross_color = C;
        HUDCrosshair.OnRender(Fvector2{cx, cy}, scr_size);
    }
}
