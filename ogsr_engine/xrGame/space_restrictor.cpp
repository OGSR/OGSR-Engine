////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.cpp
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restrictor.h"
#include "xrServer_Objects_ALife.h"
#include "level.h"
#include "space_restriction_manager.h"
#include "restriction_space.h"
#include "ai_space.h"
#include "CustomZone.h"
#include "game_object_space.h"
#include "script_game_object.h"
#include "entity_alive.h"

#ifdef DEBUG
#include "debug_renderer.h"
#endif

CSpaceRestrictor::~CSpaceRestrictor() {}

void CSpaceRestrictor::Center(Fvector& C) const { XFORM().transform_tiny(C, CFORM()->getSphere().P); }

float CSpaceRestrictor::Radius() const
{
    auto cf = CFORM();
    ASSERT_FMT(cf, "!![%s]: [%s] has no CFORM()", __FUNCTION__, cName().c_str());
    return cf->getRadius();
}

BOOL CSpaceRestrictor::net_Spawn(CSE_Abstract* data)
{
    actual(false);

    CSE_Abstract* abstract = (CSE_Abstract*)data;
    CSE_ALifeSpaceRestrictor* se_shape = smart_cast<CSE_ALifeSpaceRestrictor*>(abstract);
    R_ASSERT(se_shape);

    m_space_restrictor_type = se_shape->m_space_restrictor_type;

    CCF_Shape* shape = xr_new<CCF_Shape>(this);
    collidable.model = shape;

    for (u32 i = 0; i < se_shape->shapes.size(); ++i)
    {
        CShapeData::shape_def& S = se_shape->shapes[i];
        switch (S.type)
        {
        case 0: {
            shape->add_sphere(S.data.sphere);
            break;
        }
        case 1: {
            shape->add_box(S.data.box);
            break;
        }
        }
    }

    if (se_shape->shapes.empty())
    {
        Msg("! [%s]: %s has no shapes", __FUNCTION__, cName().c_str());
        CShapeData::shape_def _shape;
        _shape.data.sphere.P.set(0.0f, 0.0f, 0.0f);
        _shape.data.sphere.R = 1.0f;
        shape->add_sphere(_shape.data.sphere);
    }

    shape->ComputeBounds();

    BOOL result = inherited::net_Spawn(data);

    if (!result)
        return (FALSE);

    setEnabled(FALSE);
    setVisible(FALSE);

    if (!ai().get_level_graph() || RestrictionSpace::ERestrictorTypes(se_shape->m_space_restrictor_type) == RestrictionSpace::eRestrictorTypeNone)
        return (TRUE);

    Level().space_restriction_manager().register_restrictor(this, RestrictionSpace::ERestrictorTypes(se_shape->m_space_restrictor_type));

    return (TRUE);
}

void CSpaceRestrictor::net_Destroy()
{
    inherited::net_Destroy();
    ScheduleUnregister();

    if (!ai().get_level_graph())
        return;

    if (RestrictionSpace::ERestrictorTypes(m_space_restrictor_type) == RestrictionSpace::eRestrictorTypeNone)
        return;

    Level().space_restriction_manager().unregister_restrictor(this);
}

bool CSpaceRestrictor::inside(const Fsphere& sphere)
{
    if (getDestroy())
        return false;

    if (!actual())
    {
        try
        {
            prepare();
        }
        catch (...)
        {
            Msg("!![%s] FATAL ERROR IN RESTRICTOR ID:[%u]!", __FUNCTION__, ID());
            return false;
        }
    }

    if (!m_selfbounds.intersect(sphere))
        return (false);

    return (prepared_inside(sphere));
}

BOOL CSpaceRestrictor::UsedAI_Locations() { return (FALSE); }

void CSpaceRestrictor::spatial_move()
{
    inherited::spatial_move();
    actual(false);
}

void CSpaceRestrictor::prepare()
{
    Center(m_selfbounds.P);
    m_selfbounds.R = Radius();

    m_spheres.resize(0);
    m_boxes.resize(0);

    const CCF_Shape* shape = (const CCF_Shape*)collidable.model;

    typedef xr_vector<CCF_Shape::shape_def> SHAPES;

    SHAPES::const_iterator I = shape->shapes.begin();
    SHAPES::const_iterator E = shape->shapes.end();
    for (; I != E; ++I)
    {
        switch ((*I).type)
        {
        case 0: { // sphere
            Fsphere temp;
            const Fsphere& sphere = (*I).data.sphere;
            XFORM().transform_tiny(temp.P, sphere.P);
            temp.R = sphere.R;
            m_spheres.push_back(temp);
            break;
        }
        case 1: { // box
            Fmatrix sphere;
            const Fmatrix& box = (*I).data.box;
            sphere.mul_43(XFORM(), box);

            // Build points
            Fvector A, B[8];
            CPlanes temp;
            A.set(-.5f, -.5f, -.5f);
            sphere.transform_tiny(B[0], A);
            A.set(-.5f, -.5f, +.5f);
            sphere.transform_tiny(B[1], A);
            A.set(-.5f, +.5f, +.5f);
            sphere.transform_tiny(B[2], A);
            A.set(-.5f, +.5f, -.5f);
            sphere.transform_tiny(B[3], A);
            A.set(+.5f, +.5f, +.5f);
            sphere.transform_tiny(B[4], A);
            A.set(+.5f, +.5f, -.5f);
            sphere.transform_tiny(B[5], A);
            A.set(+.5f, -.5f, +.5f);
            sphere.transform_tiny(B[6], A);
            A.set(+.5f, -.5f, -.5f);
            sphere.transform_tiny(B[7], A);

            temp.m_planes[0].build(B[0], B[3], B[5]);
            temp.m_planes[1].build(B[1], B[2], B[3]);
            temp.m_planes[2].build(B[6], B[5], B[4]);
            temp.m_planes[3].build(B[4], B[2], B[1]);
            temp.m_planes[4].build(B[3], B[2], B[4]);
            temp.m_planes[5].build(B[1], B[0], B[6]);

            m_boxes.push_back(temp);

            break;
        }
        default: NODEFAULT;
        }
    }

    actual(true);
}

bool CSpaceRestrictor::prepared_inside(const Fsphere& sphere) const
{
    VERIFY(actual());

    {
        SPHERES::const_iterator I = m_spheres.begin();
        SPHERES::const_iterator E = m_spheres.end();
        for (; I != E; ++I)
            if (sphere.intersect(*I))
                return (true);
    }

    {
        BOXES::const_iterator I = m_boxes.begin();
        BOXES::const_iterator E = m_boxes.end();
        for (; I != E; ++I)
        {
            for (u32 i = 0; i < PLANE_COUNT; ++i)
                if ((*I).m_planes[i].classify(sphere.P) > sphere.R)
                    goto continue_loop;
            return (true);
        continue_loop:
            continue;
        }
    }
    return (false);
}

void CSpaceRestrictor::ScheduleRegister()
{
    if (!IsScheduled())
    {
        shedule_register();
        feel_touch.clear();
        b_scheduled = true;
    }
}

void CSpaceRestrictor::ScheduleUnregister()
{
    if (IsScheduled())
    {
        shedule_unregister();
        feel_touch.clear();
        b_scheduled = false;
    }
}

void CSpaceRestrictor::shedule_Update(u32 dt)
{
    inherited::shedule_Update(dt);
    if (IsScheduled())
    {
        const Fsphere& s = CFORM()->getSphere();
        Fvector P;
        XFORM().transform_tiny(P, s.P);
        feel_touch_update(P, s.R);
    }
}

void CSpaceRestrictor::feel_touch_new(CObject* tpObject)
{
    if (IsScheduled())
    {
        CGameObject* l_tpGameObject = smart_cast<CGameObject*>(tpObject);
        callback(GameObject::eZoneEnter)(lua_game_object(), l_tpGameObject->lua_game_object());
    }
}

void CSpaceRestrictor::feel_touch_delete(CObject* tpObject)
{
    if (IsScheduled())
    {
        CGameObject* l_tpGameObject = smart_cast<CGameObject*>(tpObject);
        callback(GameObject::eZoneExit)(lua_game_object(), l_tpGameObject->lua_game_object());
    }
}

void CSpaceRestrictor::net_Relcase(CObject* O)
{
    if (IsScheduled() && !Level().is_removing_objects())
    {
        feel_touch_relcase2(O);
    }
    inherited::net_Relcase(O);
}

BOOL CSpaceRestrictor::feel_touch_contact(CObject* O)
{
    if (smart_cast<CEntityAlive*>(O))
        return ((CCF_Shape*)CFORM())->Contact(O);
    return FALSE;
}

bool CSpaceRestrictor::active_contact(u16 id) const
{
    for (const auto& I : feel_touch)
        if (I->ID() == id)
            return true;
    return false;
}

float CSpaceRestrictor::distance_to(Fvector& P)
{
    if (!actual())
        prepare();

    if (m_spheres.empty() && m_boxes.empty())
        return P.distance_to(m_selfbounds.P);

    float min_dist = flt_max;
    for (const auto& it : m_spheres)
    {
        float dist = P.distance_to(it.P) - it.R;
        if (dist < 0)
            return dist;
        else if (dist < min_dist)
            min_dist = dist;
    }

    for (const auto& it : m_boxes)
    {
        float max_dist = -flt_max;
        for (u32 i = 0; i < PLANE_COUNT; ++i)
        {
            float dist = it.m_planes[i].classify(P);
            if (dist > max_dist)
                max_dist = dist;
        }
        if (max_dist < 0)
            return max_dist;
        if (max_dist < min_dist)
            min_dist = max_dist;
    }

    return min_dist;
}

#include "hudmanager.h"
#include "Debug_Renderer.h"

void CSpaceRestrictor::OnRender()
{
    DRender->OnFrameEnd();
    Fvector l_half;
    l_half.set(.5f, .5f, .5f);
    Fmatrix l_ball, l_box;
    xr_vector<CCF_Shape::shape_def>& l_shapes = ((CCF_Shape*)CFORM())->Shapes();
    xr_vector<CCF_Shape::shape_def>::iterator l_pShape;

    u32 Color = 0;
    CCustomZone* custom_zone = smart_cast<CCustomZone*>(this);
    if (custom_zone && custom_zone->IsEnabled())
        Color = D3DCOLOR_XRGB(0, 255, 255);
    else
        Color = D3DCOLOR_XRGB(255, 0, 0);

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
        HUD().Font().pFontMedium->OutNext("%s id=%d", Name(), ID());
        CCustomZone* z = smart_cast<CCustomZone*>(this);
        if (z)
        {
            string64 str;
            switch (z->ZoneState())
            {
            case CCustomZone::eZoneStateIdle: strcpy_s(str, "IDLE"); break;
            case CCustomZone::eZoneStateAwaking: strcpy_s(str, "AWAKING"); break;
            case CCustomZone::eZoneStateBlowout: strcpy_s(str, "BLOWOUT"); break;
            case CCustomZone::eZoneStateAccumulate: strcpy_s(str, "ACCUMULATE"); break;
            case CCustomZone::eZoneStateDisabled: strcpy_s(str, "DISABLED"); break;
            };
            HUD().Font().pFontMedium->OutNext(str);
        }
    }
}
