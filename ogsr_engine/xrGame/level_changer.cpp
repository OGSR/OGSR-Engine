////////////////////////////////////////////////////////////////////////////
//	Module 		: level_changer.cpp
//	Created 	: 10.07.2003
//  Modified 	: 10.07.2003
//	Author		: Dmitriy Iassenev
//	Description : Level change object
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level_changer.h"
#include "hit.h"
#include "actor.h"
#include "xrserver_objects_alife.h"
#include "level.h"
#include "ai_object_location.h"
#include "ai_space.h"
#include "level_graph.h"
#include "game_level_cross_table.h"

#include "HudManager.h"
#include "UIGameSP.h"

xr_vector<CLevelChanger*> g_lchangers;

CLevelChanger::~CLevelChanger() {}

void CLevelChanger::Center(Fvector& C) const { XFORM().transform_tiny(C, CFORM()->getSphere().P); }

float CLevelChanger::Radius() const { return CFORM()->getRadius(); }

void CLevelChanger::net_Destroy()
{
    inherited ::net_Destroy();
    xr_vector<CLevelChanger*>::iterator it = std::find(g_lchangers.begin(), g_lchangers.end(), this);
    if (it != g_lchangers.end())
        g_lchangers.erase(it);
}

BOOL CLevelChanger::net_Spawn(CSE_Abstract* DC)
{
    m_entrance_time = 0;
    CCF_Shape* l_pShape = xr_new<CCF_Shape>(this);
    collidable.model = l_pShape;

    CSE_Abstract* l_tpAbstract = (CSE_Abstract*)(DC);
    CSE_ALifeLevelChanger* l_tpALifeLevelChanger = smart_cast<CSE_ALifeLevelChanger*>(l_tpAbstract);
    R_ASSERT(l_tpALifeLevelChanger);

    m_game_vertex_id = l_tpALifeLevelChanger->m_tNextGraphID;
    m_level_vertex_id = l_tpALifeLevelChanger->m_dwNextNodeID;
    m_position = l_tpALifeLevelChanger->m_tNextPosition;
    m_angles = l_tpALifeLevelChanger->m_tAngles;

    m_SilentMode = l_tpALifeLevelChanger->m_SilentMode;
    if (ai().get_level_graph())
    {
        //. this information should be computed in xrAI
        ai_location().level_vertex(ai().level_graph().vertex(u32(-1), Position()));
        ai_location().game_vertex(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
    }

    feel_touch.clear();

    for (u32 i = 0; i < l_tpALifeLevelChanger->shapes.size(); ++i)
    {
        CSE_Shape::shape_def& S = l_tpALifeLevelChanger->shapes[i];
        switch (S.type)
        {
        case 0: {
            l_pShape->add_sphere(S.data.sphere);
            break;
        }
        case 1: {
            l_pShape->add_box(S.data.box);
            break;
        }
        }
    }

    BOOL bOk = inherited::net_Spawn(DC);
    if (bOk)
    {
        l_pShape->ComputeBounds();
        Fvector P;
        XFORM().transform_tiny(P, CFORM()->getSphere().P);
        setEnabled(TRUE);
    }
    g_lchangers.push_back(this);
    return (bOk);
}

void CLevelChanger::shedule_Update(u32 dt)
{
    inherited::shedule_Update(dt);

    const Fsphere& s = CFORM()->getSphere();
    Fvector P;
    XFORM().transform_tiny(P, s.P);
    feel_touch_update(P, s.R);

    update_actor_invitation();
}
#include "patrol_path.h"
#include "patrol_path_storage.h"
void CLevelChanger::feel_touch_new(CObject* tpObject)
{
    CActor* l_tpActor = smart_cast<CActor*>(tpObject);
    VERIFY(l_tpActor);
    if (!l_tpActor->g_Alive())
        return;

    if (m_SilentMode)
    {
        if (m_SilentMode == 2)
        {
            Fvector p, r;
            if (get_reject_pos(p, r))
            {
                Actor()->MoveActor(p, r);
                return;
            }
            else
            {
                Msg("! [%s]: [%s] pt_move_if_reject not found: m_SilentMode[%u]", __FUNCTION__, cName().c_str(), m_SilentMode);
            }
        }
        NET_Packet p;
        p.w_begin(M_CHANGE_LEVEL);
        p.w(&m_game_vertex_id, sizeof(m_game_vertex_id));
        p.w(&m_level_vertex_id, sizeof(m_level_vertex_id));
        p.w_vec3(m_position);
        p.w_vec3(m_angles);
        Level().Send(p, net_flags(TRUE));
        return;
    }

    ChangeLevel();

    m_entrance_time = Device.fTimeGlobal;
}

bool CLevelChanger::get_reject_pos(Fvector& p, Fvector& r)
{
    p.set(0, 0, 0);
    r.set(0, 0, 0);
    //--		db.actor:set_actor_position(patrol("t_way"):point(0))
    //--		local dir = patrol("t_look"):point(0):sub(patrol("t_way"):point(0))
    //--		db.actor:set_actor_direction(-dir:getH())

    if (m_ini_file && m_ini_file->section_exist("pt_move_if_reject"))
    {
        LPCSTR p_name = m_ini_file->r_string("pt_move_if_reject", "path");
        const CPatrolPath* patrol_path = ai().patrol_paths().safe_path(p_name, false, true);
        VERIFY(patrol_path);

        const CPatrolPoint* pt;
        pt = &patrol_path->vertex(0)->data();
        p = pt->position();

        Fvector tmp;
        pt = &patrol_path->vertex(1)->data();
        tmp.sub(pt->position(), p);
        tmp.getHP(r.y, r.x);
        return true;
    }
    return false;
}

BOOL CLevelChanger::feel_touch_contact(CObject* object) { return (((CCF_Shape*)CFORM())->Contact(object)) && smart_cast<CActor*>(object); }

void CLevelChanger::update_actor_invitation()
{
    if (m_SilentMode)
        return;

    xr_vector<CObject*>::iterator it = feel_touch.begin();
    xr_vector<CObject*>::iterator it_e = feel_touch.end();

    for (; it != it_e; ++it)
    {
        VERIFY(smart_cast<CActor*>(*it));

        if (m_entrance_time + 5.0f < Device.fTimeGlobal)
        {
            ChangeLevel();
            m_entrance_time = Device.fTimeGlobal;
        }
    }
}

#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"

void CLevelChanger::ChangeLevel()
{
    Fvector p, r;
    bool b = get_reject_pos(p, r);
    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (pGameSP)
    {
        Actor()->callback(GameObject::eLevelChangerAction)(lua_game_object(), (CUIWindow*)pGameSP->UIChangeLevelWnd);
        pGameSP->ChangeLevel(m_game_vertex_id, m_level_vertex_id, m_position, m_angles, p, r, b);
    }
}

#include "Debug_Renderer.h"

void CLevelChanger::OnRender()
{
    DRender->OnFrameEnd();
    Fvector l_half;
    l_half.set(.5f, .5f, .5f);
    Fmatrix l_ball, l_box;
    xr_vector<CCF_Shape::shape_def>& l_shapes = ((CCF_Shape*)CFORM())->Shapes();
    xr_vector<CCF_Shape::shape_def>::iterator l_pShape;

    constexpr u32 Color = D3DCOLOR_XRGB(255, 0, 255);

    for (l_pShape = l_shapes.begin(); l_shapes.end() != l_pShape; ++l_pShape)
    {
        switch (l_pShape->type)
        {
        case 0: {
            Fsphere& l_sphere = l_pShape->data.sphere;
            l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
            // l_ball.scale(1.f, 1.f, 1.f);
            Fvector l_p;
            XFORM().transform(l_p, l_sphere.P);
            l_ball.translate_add(l_p);
            // l_ball.mul(XFORM(), l_ball);
            // l_ball.mul(l_ball, XFORM());
            Level().debug_renderer().draw_ellipse(l_ball, Color);
        }
        break;
        case 1: {
            l_box.mul(XFORM(), l_pShape->data.box);
            Level().debug_renderer().draw_obb(l_box, l_half, Color);
        }
        break;
        }
    }
    if (Device.vCameraPosition.distance_to(XFORM().c) < 100.0f)
    {
        // DRAW name

        Fmatrix res;
        res.mul(Device.mFullTransform, XFORM());

        Fvector4 v_res;

        float delta_height = 0.f;

        // get up on 2 meters
        Fvector shift;
        static float gx = 0.0f;
        static float gy = 2.0f;
        static float gz = 0.0f;
        shift.set(gx, gy, gz);
        res.transform(v_res, shift);

        // check if the object in sight
        if (v_res.z < 0 || v_res.w < 0)
            return;
        if (v_res.x < -1.f || v_res.x > 1.f || v_res.y < -1.f || v_res.y > 1.f)
            return;

        // get real (x,y)
        float x = (1.f + v_res.x) / 2.f * (Device.dwWidth);
        float y = (1.f - v_res.y) / 2.f * (Device.dwHeight) - delta_height;

        HUD().Font().pFontMedium->SetColor(Color);
        HUD().Font().pFontMedium->OutSet(x, y -= delta_height);
        HUD().Font().pFontMedium->OutNext(Name());
    }
}
