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
    m_real_dist = 0.f;

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
    m_real_dist = -1.f;

    collide::ray_defs RD(p1, dir, RQ.range, CDB::OPT_CULL, collide::rqtBoth);
    RQR.r_clear();
    VERIFY(!fis_zero(RD.dir.square_magnitude()));
    if (Level().ObjectSpace.RayQuery(RQR, RD, pick_trace_callback, &RQ, nullptr, Level().CurrentEntity()))
    {
        m_real_dist = RQ.range;
        clamp(RQ.range, NEAR_LIM, RQ.range);
    }
}

extern ENGINE_API BOOL g_bRendering;

#include <ai/monsters/poltergeist/poltergeist.h>

void CHUDTarget::Render()
{
    VERIFY(g_bRendering);

    CActor* Actor = smart_cast<CActor*>(Level().CurrentEntity());
    if (!Actor)
        return;

    bool need_to_show_cursor = true;

    if (Actor->get_state() & mcSprint)
    {
        need_to_show_cursor = false;
    }

    CInventoryItem* active_item = Actor->inventory().ActiveItem();
    if (smart_cast<CPda*>(active_item))
    {
        need_to_show_cursor = false;
    }

    CInventoryOwner* our_inv_owner = smart_cast<CInventoryOwner*>(Level().CurrentEntity());

    if (const auto Wpn = smart_cast<CWeapon*>(active_item); Wpn && (Wpn->IsLaserOn() || Wpn->GetState() == CHUDState::EHudStates::eReload))
    {
        need_to_show_cursor = false;
    }

    if (!need_to_show_cursor)
    {
        fuzzyShowInfo = 0.f;

        return;
    }

    Fvector p1 = Device.vCameraPosition;
    Fvector dir = Device.vCameraDirection;

    if (const auto Wpn = smart_cast<CHudItem*>(active_item))
        Actor->g_fireParams(Wpn, p1, dir, true);

    // Render cursor
    u32 C = C_DEFAULT;

    Fvector p2;
    p2.mad(p1, dir, RQ.range);

    Fvector4 pt;
    Device.mFullTransform.transform(pt, p2);
    pt.y = -pt.y;

    CGameFont* F = HUD().Font().pFontGraffiti19Russian;

    F->SetAligment(CGameFont::alCenter);
    F->SetColor(C);

    F->OutSetI(0.f, 0.05f); // for legacy mode

    if (psHUD_Flags.test(HUD_CROSSHAIR_DIST))
    {
        F->OutNext("%4.1f", RQ.range);
    }

    if (psHUD_Flags.test(HUD_INFO))
    {
        if (RQ.O && (RQ.O->getVisible() || !!smart_cast<CPoltergeist*>(RQ.O)) && RQ.range * our_inv_owner->inventory().GetTakeDist())
        {
            CEntityAlive* E = smart_cast<CEntityAlive*>(RQ.O);
            const PIItem I = smart_cast<PIItem>(RQ.O);

            bool needToRender = true;

            float x = 0.f;
            float y = 0.f;

            if (E && E->g_Alive() && E->cast_base_monster())
            {
                auto* pCurEnt = smart_cast<CEntityAlive*>(Level().CurrentEntity());
                const int relation = MONSTER_COMMUNITY::relation(pCurEnt->monster_community->index(), E->monster_community->index());

                C = relation > 0 ? C_ON_FRIEND : (relation == 0 ? C_ON_NEUTRAL : C_ON_ENEMY);
            }
            else if (psHUD_Flags.test(HUD_INFO_OVERHEAD))
            {
                Fvector4 v_res;

                if (E && E->Visual())
                {
                    const auto k = smart_cast<IKinematics*>(E->Visual());
                    const u16 bone_id = k->LL_BoneID(READ_IF_EXISTS(pSettings, r_string, E->cNameSect(), "bone_head", "bip01_head"));

                    k->CalculateBones();

                    Fmatrix matrix;
                    matrix.mul(E->XFORM(), k->LL_GetBoneInstance(bone_id).mTransform);

                    float m_hudInfoBigHeadOffsetY = 0.25f;
                    float m_hudInfoSmallHeadOffsetY = 0.5f;

                    // Move up head point
                    float f = (RQ.range / (2.0f * our_inv_owner->inventory().GetTakeDist()));
                    float head_offset = (1.f - f) * m_hudInfoBigHeadOffsetY + f * m_hudInfoSmallHeadOffsetY;
                    matrix.c.y += head_offset;

                    Device.mFullTransform.transform(v_res, matrix.c);
                }
                else
                {
                    Device.mFullTransform.transform(v_res, RQ.O->XFORM().c);
                }

                if (v_res.z < 0 || v_res.w < 0)
                    needToRender = false;

                // if (v_res.x < -1.f || v_res.x > 1.f || v_res.y < -1.f || v_res.y > 1.f)
                //     needToRender = false;

                if (needToRender)
                {
                    x = (1.f + v_res.x) / 2.f * (Device.dwWidth);
                    y = (1.f - v_res.y) / 2.f * (Device.dwHeight);
                }
            }

            if (needToRender)
            {
                if (E && E->g_Alive() && !E->cast_base_monster())
                {
                    CInventoryOwner* others_inv_owner = smart_cast<CInventoryOwner*>(E);

                    if (our_inv_owner && others_inv_owner)
                    {
                        switch (RELATION_REGISTRY().GetRelationType(others_inv_owner, our_inv_owner))
                        {
                        case ALife::eRelationTypeWorstEnemy:
                        case ALife::eRelationTypeEnemy: C = C_ON_ENEMY; break;
                        case ALife::eRelationTypeNeutral: C = C_ON_NEUTRAL; break;
                        case ALife::eRelationTypeFriend: C = C_ON_FRIEND; break;
                        default:;
                        }

                        if (fuzzyShowInfo > 0.5f)
                        {
                            F->SetColor(subst_alpha(C, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));

                            if (psHUD_Flags.test(HUD_INFO_OVERHEAD))
                            {
                                F->Out(x, y, "%s", *CStringTable().translate(others_inv_owner->Name()));
                                F->Out(x, y + F->CurrentHeight_(), "%s", *CStringTable().translate(others_inv_owner->CharacterInfo().Community().id()));
                            }
                            else
                            {
                                F->OutNext("%s", *CStringTable().translate(others_inv_owner->Name()));
                                F->OutNext("%s", *CStringTable().translate(others_inv_owner->CharacterInfo().Community().id()));
                            }
                        }
                    }

                    fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
                }
                else if (I && our_inv_owner)
                {
                    if (fuzzyShowInfo > 0.5f)
                    {
                        F->SetColor(subst_alpha(C, u8(iFloor(255.f * (fuzzyShowInfo - 0.5f) * 2.f))));

                        const LPCSTR draw_str = I->Name /*Complex*/ ();

                        if (psHUD_Flags.test(HUD_INFO_OVERHEAD))
                        {
                            if (I->GetInvShowCondition())
                            {
                                F->Out(x, y, "%s (%.0f%%)", draw_str, I->GetConditionToShow() * 100.f);
                            }
                            else
                                F->Out(x, y, "%s", draw_str);
                        }
                        else
                        {
                            if (I->GetInvShowCondition())
                            {
                                F->OutNext("%s (%.0f%%)", draw_str, I->GetConditionToShow() * 100.f);
                            }
                            else
                                F->OutNext("%s", draw_str);
                        }
                    }

                    fuzzyShowInfo += SHOW_INFO_SPEED * Device.fTimeDelta;
                }
            }
        }
        else
        {
            fuzzyShowInfo = 0.f;
        }

        clamp(fuzzyShowInfo, 0.f, 1.f);
    }

    Fvector2 scr_size;
    scr_size.set(float(Device.dwWidth), float(Device.dwHeight));

    const float w_2 = scr_size.x / 2.0f;
    const float h_2 = scr_size.y / 2.0f;

    // Convert to screen coords
    const float cx = (pt.x + 1) * w_2;
    const float cy = (pt.y + 1) * h_2;

    // отрендерить кружочек или крестик
    if (!m_bShowCrosshair)
    {
        const float di_size = C_SIZE / powf(pt.w, .2f);

        // Msg("di_size = [%f] range = [%f]", di_size, RQ.range);

        const float size_x = scr_size.x * di_size;
        const float size_y = size_x; // scr_size.y * di_size;

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
        // отрендерить прицел
        HUDCrosshair.cross_color = C;
        HUDCrosshair.OnRender(Fvector2{cx, cy}, scr_size);
    }

}

float CHUDTarget::GetRealDist() { return m_real_dist; }
