#include "stdafx.h"
#include "HangingLamp.h"
#include "../xr_3da/LightAnimLibrary.h"
#include "../xr_3da/xr_collide_form.h"
#include "PhysicsShell.h"
#include "Physics.h"
#include "xrserver_objects_alife.h"
#include "PHElement.h"
#include "..\Include/xrRender/Kinematics.h"
#include "..\Include/xrRender/KinematicsAnimated.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHangingLamp::CHangingLamp() { Init(); }

CHangingLamp::~CHangingLamp() {}

void CHangingLamp::Init()
{
    fHealth = 100.f;
    light_bone = BI_NONE;
    ambient_bone = BI_NONE;
    lanim = 0;
    ambient_power = 0.f;
    light_render = 0;
    light_ambient = 0;
    glow_render = 0;
}

void CHangingLamp::RespawnInit()
{
    Init();
    if (Visual())
    {
        IKinematics* K = smart_cast<IKinematics*>(Visual());

        // K->LL_SetBonesVisibleAll();
        VisMask new_mask;
        new_mask.set_all();
        K->LL_SetBonesVisible(new_mask);

        K->CalculateBones_Invalidate();
        K->CalculateBones();
    }
}

void CHangingLamp::Center(Fvector& C) const
{
    if (renderable.visual)
        renderable.xform.transform_tiny(C, renderable.visual->getVisData().sphere.P);
    else
        C.set(XFORM().c);
}

float CHangingLamp::Radius() const { return (renderable.visual) ? renderable.visual->getVisData().sphere.R : EPS; }

void CHangingLamp::Load(LPCSTR section) { inherited::Load(section); }

void CHangingLamp::net_Destroy()
{
    light_render.destroy();
    light_ambient.destroy();
    glow_render.destroy();
    RespawnInit();
    if (Visual())
        CPHSkeleton::RespawnInit();
    inherited::net_Destroy();
}

BOOL CHangingLamp::net_Spawn(CSE_Abstract* DC)
{
    CSE_Abstract* e = (CSE_Abstract*)(DC);
    CSE_ALifeObjectHangingLamp* lamp = smart_cast<CSE_ALifeObjectHangingLamp*>(e);
    R_ASSERT(lamp);
    inherited::net_Spawn(DC);
    Fcolor clr;

    // set bone id
    //	CInifile* pUserData		= K->LL_UserData();
    //	R_ASSERT3				(pUserData,"Empty HangingLamp user data!",lamp->get_visual());
    xr_delete(collidable.model);
    if (Visual())
    {
        IKinematics* K = smart_cast<IKinematics*>(Visual());
        R_ASSERT(Visual() && smart_cast<IKinematics*>(Visual()));
        light_bone = K->LL_BoneID(*lamp->light_main_bone);
        VERIFY(light_bone != BI_NONE);
        ambient_bone = K->LL_BoneID(*lamp->light_ambient_bone);
        VERIFY(ambient_bone != BI_NONE);
        collidable.model = xr_new<CCF_Skeleton>(this);

        m_animated = !lamp->startup_animation.empty();
    }
    fBrightness = lamp->brightness;
    clr.set(lamp->color);
    clr.a = 1.f;
    clr.mul_rgb(fBrightness);

    light_render = ::Render->light_create();
    light_render->set_shadow(!!lamp->flags.is(CSE_ALifeObjectHangingLamp::flCastShadow));
    light_render->set_type(lamp->flags.is(CSE_ALifeObjectHangingLamp::flTypeSpot) ? IRender_Light::SPOT : IRender_Light::POINT);
    light_render->set_range(lamp->range);
    light_render->set_color(clr);
    light_render->set_cone(lamp->spot_cone_angle);
    light_render->set_texture(*lamp->light_texture);
    light_render->set_virtual_size(lamp->m_virtual_size);

// todo("адаптировать под новый рендер!")
    // light_render->set_flare(!!lamp->flags.is(CSE_ALifeObjectHangingLamp::flUseFlare));
    // light_render->set_lsf_params(lamp->m_speed, lamp->m_amount, lamp->m_smap_jitter);

    //Simp: для поинта не вижу смысла по дефолту включать волюметрик, ибо один поинт - это шесть отдельных источников света направленных в разные стороны. Слишком накладно по 6 волюметриков на светильник.
    //Да и вообще лучше придумать настройки чтоб каждую лампу индивидуально настроить можно было. Пока только настройку через скрипты добавил.
    light_render->set_volumetric(lamp->flags.is(CSE_ALifeObjectHangingLamp::flVolumetricLight) /*|| light_render->get_type() == IRender_Light::SPOT*/);
    light_render->set_volumetric_quality(1.f);
    light_render->set_volumetric_intensity(0.1f);
    light_render->set_volumetric_distance(1.f);

    if (lamp->glow_texture.size())
    {
        glow_render = ::Render->glow_create();
        glow_render->set_texture(*lamp->glow_texture);
        glow_render->set_color(clr);
        glow_render->set_radius(lamp->glow_radius);
    }

    if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPointAmbient))
    {
        ambient_power = lamp->m_ambient_power;
        light_ambient = ::Render->light_create();
        light_ambient->set_type(IRender_Light::POINT);
        light_ambient->set_shadow(false);
        clr.mul_rgb(ambient_power);
        light_ambient->set_range(lamp->m_ambient_radius);
        light_ambient->set_color(clr);
        light_ambient->set_texture(*lamp->m_ambient_texture);
        light_ambient->set_virtual_size(lamp->m_virtual_size);
    }

    fHealth = lamp->m_health;

    lanim = LALib.FindItem(*lamp->color_animator);

    CPHSkeleton::Spawn(e);
    if (smart_cast<IKinematicsAnimated*>(Visual()))
        smart_cast<IKinematicsAnimated*>(Visual())->PlayCycle("idle");
    if (smart_cast<IKinematics*>(Visual()))
    {
        smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate();
        smart_cast<IKinematics*>(Visual())->CalculateBones();
        //.intepolate_pos
    }
    if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic) && !Visual())
        Msg("! WARNING: lamp, obj name [%s],flag physics set, but has no visual", *cName());
    //.	if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic)&&Visual()&&!guid_physic_bone)	fHealth=0.f;
    if (Alive())
        TurnOn();
    else
    {
        processing_activate(); // temporal enable
        TurnOff(); // -> and here is disable :)
    }

    setVisible((BOOL) !!Visual());
    setEnabled((BOOL) !!collidable.model);

    return (TRUE);
}

void CHangingLamp::SpawnInitPhysics(CSE_Abstract* D)
{
    CSE_ALifeObjectHangingLamp* lamp = smart_cast<CSE_ALifeObjectHangingLamp*>(D);
    if (lamp->flags.is(CSE_ALifeObjectHangingLamp::flPhysic))
        CreateBody(lamp);
    if (smart_cast<IKinematics*>(Visual()))
    {
        smart_cast<IKinematics*>(Visual())->CalculateBones_Invalidate();
        smart_cast<IKinematics*>(Visual())->CalculateBones();
        //.intepolate_pos
    }
}

void CHangingLamp::CopySpawnInit()
{
    CPHSkeleton::CopySpawnInit();
    IKinematics* K = smart_cast<IKinematics*>(Visual());
    if (!K->LL_GetBoneVisible(light_bone))
        TurnOff();
}
void CHangingLamp::net_Save(NET_Packet& P)
{
    inherited::net_Save(P);
    CPHSkeleton::SaveNetState(P);
}

BOOL CHangingLamp::net_SaveRelevant() { return (inherited::net_SaveRelevant() || BOOL(PPhysicsShell() != NULL)); }

void CHangingLamp::shedule_Update(u32 dt)
{
    CPHSkeleton::Update(dt);

    inherited::shedule_Update(dt);
}

void CHangingLamp::UpdateCL()
{
    inherited::UpdateCL();

    if (m_pPhysicsShell)
        m_pPhysicsShell->InterpolateGlobalTransform(&XFORM());

    if (!Alive())
        return;

    if (light_render->get_active())
    {
        if (Visual())
            PKinematics(Visual())->CalculateBones(m_animated);

        // update T&R from light (main) bone
        Fmatrix xf;
        if (light_bone != BI_NONE)
        {
            Fmatrix& M = smart_cast<IKinematics*>(Visual())->LL_GetTransform(light_bone);
            xf.mul(XFORM(), M);
            VERIFY(!fis_zero(DET(xf)));
        }
        else
        {
            xf.set(XFORM());
        }
        light_render->set_rotation(xf.k, xf.i);
        light_render->set_position(xf.c);
        if (glow_render)
            glow_render->set_position(xf.c);

        // update T&R from ambient bone
        if (light_ambient)
        {
            if (ambient_bone != light_bone)
            {
                if (ambient_bone != BI_NONE)
                {
                    Fmatrix& M = smart_cast<IKinematics*>(Visual())->LL_GetTransform(ambient_bone);
                    xf.mul(XFORM(), M);
                    VERIFY(!fis_zero(DET(xf)));
                }
                else
                {
                    xf.set(XFORM());
                }
            }
            light_ambient->set_rotation(xf.k, xf.i);
            light_ambient->set_position(xf.c);
        }

        if (lanim)
        {
            int frame;
            u32 clr = lanim->CalculateBGR(Device.fTimeGlobal, frame); // возвращает в формате BGR
            Fcolor fclr;
            fclr.set((float)color_get_B(clr), (float)color_get_G(clr), (float)color_get_R(clr), 1.f);
            fclr.mul_rgb(fBrightness / 255.f);
            light_render->set_color(fclr);
            if (glow_render)
                glow_render->set_color(fclr);
            if (light_ambient)
            {
                fclr.mul_rgb(ambient_power);
                light_ambient->set_color(fclr);
            }
        }
    }

    //Эффект мигания при выбросе
    bool light_status{lights_turned_on};
    const float surge_progress = shader_exports.get_pda_params().x;

    if (light_status && !fis_zero(surge_progress))
    {
        const float mig = 1.f - (surge_progress * 2.f);
        const float time[]{Device.fTimeGlobal, Device.fTimeGlobal / 10.f};
        light_status = Device.NoiseRandom(time) <= mig;
    }

    if (light_render)
        light_render->set_active(light_status);

    if (glow_render)
        glow_render->set_active(light_status);

    if (light_ambient)
        light_ambient->set_active(light_status);
}

void CHangingLamp::TurnOn()
{
    lights_turned_on = true;
    light_render->set_active(true);
    if (glow_render)
        glow_render->set_active(true);
    if (light_ambient)
        light_ambient->set_active(true);
    if (Visual())
    {
        IKinematics* K = smart_cast<IKinematics*>(Visual());
        K->LL_SetBoneVisible(light_bone, TRUE, TRUE);
        K->CalculateBones_Invalidate();
        K->CalculateBones();
    }
    processing_activate();
}

void CHangingLamp::TurnOff()
{
    lights_turned_on = false;
    light_render->set_active(false);
    if (glow_render)
        glow_render->set_active(false);
    if (light_ambient)
        light_ambient->set_active(false);
    if (Visual())
        smart_cast<IKinematics*>(Visual())->LL_SetBoneVisible(light_bone, FALSE, TRUE);
    processing_deactivate();
}

// void CHangingLamp::Hit(float P,Fvector &dir, CObject* who,s16 element,
//					   Fvector p_in_object_space, float impulse, ALife::EHitType hit_type)
void CHangingLamp::Hit(SHit* pHDS)
{
    SHit HDS = *pHDS;
    callback(GameObject::eHit)(lua_game_object(), HDS.power, HDS.dir, smart_cast<const CGameObject*>(HDS.who)->lua_game_object(), HDS.bone());
    bool bWasAlive = Alive();

    if (m_pPhysicsShell)
        m_pPhysicsShell->applyHit(pHDS->p_in_bone_space, pHDS->dir, pHDS->impulse, pHDS->boneID, pHDS->hit_type);

    if (pHDS->boneID == light_bone)
        fHealth = 0.f;
    else
        fHealth -= pHDS->damage() * 100.f;

    if (bWasAlive && (!Alive()))
        TurnOff();
}

static BONE_P_MAP bone_map = BONE_P_MAP();
void CHangingLamp::CreateBody(CSE_ALifeObjectHangingLamp* lamp)
{
    if (!Visual())
        return;
    if (m_pPhysicsShell)
        return;

    IKinematics* pKinematics = smart_cast<IKinematics*>(Visual());

    m_pPhysicsShell = P_create_Shell();

    bone_map.clear();
    LPCSTR fixed_bones = *lamp->fixed_bones;
    if (fixed_bones)
    {
        int count = _GetItemCount(fixed_bones);
        for (int i = 0; i < count; ++i)
        {
            string64 fixed_bone;
            _GetItem(fixed_bones, i, fixed_bone);
            u16 fixed_bone_id = pKinematics->LL_BoneID(fixed_bone);
            R_ASSERT2(BI_NONE != fixed_bone_id, "wrong fixed bone");
            bone_map.insert(mk_pair(fixed_bone_id, physicsBone()));
        }
    }
    else
    {
        bone_map.insert(mk_pair(pKinematics->LL_GetBoneRoot(), physicsBone()));
    }

    m_pPhysicsShell->build_FromKinematics(pKinematics, &bone_map);
    m_pPhysicsShell->set_PhysicsRefObject(this);
    m_pPhysicsShell->mXFORM.set(XFORM());
    m_pPhysicsShell->Activate(true); //,
    // m_pPhysicsShell->SmoothElementsInertia(0.3f);
    m_pPhysicsShell->SetAirResistance(); // 0.0014f,1.5f

    /////////////////////////////////////////////////////////////////////////////
    BONE_P_PAIR_IT i = bone_map.begin(), e = bone_map.end();
    for (; i != e; ++i)
    {
        CPhysicsElement* fixed_element = i->second.element;
        /// R_ASSERT2(fixed_element,"fixed bone has no physics");
        if (fixed_element)
            fixed_element->Fix();
    }

    m_pPhysicsShell->mXFORM.set(XFORM());
    m_pPhysicsShell->SetAirResistance(0.001f, 0.02f);
    SAllDDOParams disable_params;
    disable_params.Load(smart_cast<IKinematics*>(Visual())->LL_UserData());
    m_pPhysicsShell->set_DisableParams(disable_params);
    ApplySpawnIniToPhysicShell(&lamp->spawn_ini(), m_pPhysicsShell, fixed_bones[0] != '\0');
}

void CHangingLamp::net_Export(CSE_Abstract* E) { VERIFY(Local()); }

BOOL CHangingLamp::UsedAI_Locations() { return (FALSE); }

void CHangingLamp::SetLSFParams(float _speed, float _amount, float _jit)
{
// todo("адаптировать под новый рендер!")
    // light_render->set_lsf_params(_speed, _amount, _jit);
}


void CHangingLamp::script_register(lua_State* L)
{
    luabind::module(L)[luabind::class_<CHangingLamp, CGameObject>("hanging_lamp")
                           .def(luabind::constructor<>())
                           .def("turn_on", &CHangingLamp::TurnOn)
                           .def("turn_off", &CHangingLamp::TurnOff)
                           .def("set_lsf_params", &CHangingLamp::SetLSFParams)
                           .def("set_volumetric", [](CHangingLamp* self, const bool val) { self->light_render->set_volumetric(val); })
                           .def("set_volumetric_quality", [](CHangingLamp* self, const float val) { self->light_render->set_volumetric_quality(val); })
                           .def("set_volumetric_intensity", [](CHangingLamp* self, const float val) { self->light_render->set_volumetric_intensity(val); })
                           .def("set_volumetric_distance", [](CHangingLamp* self, const float val) { self->light_render->set_volumetric_distance(val); })
    ];
}
