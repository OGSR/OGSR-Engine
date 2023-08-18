#include "stdafx.h"
#include "ParticlesObject.h"
#include "../xr_3da/gamemtllib.h"
#include "level.h"
#include "gamepersistent.h"
#include "Extendedgeom.h"
#include "PhysicsGamePars.h"
#include "PhysicsCommon.h"
#include "PhSoundPlayer.h"
#include "PhysicsShellHolder.h"
#include "PHCommander.h"
#include "MathUtils.h"
#include "PHReqComparer.h"
#include "PHWorld.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
static const float PARTICLE_EFFECT_DIST = 70.f;
static const float SOUND_EFFECT_DIST = 70.f;
//////////////////////////////////////////////////////////////////////////////////
static const float SQUARE_PARTICLE_EFFECT_DIST = PARTICLE_EFFECT_DIST * PARTICLE_EFFECT_DIST;
static const float SQUARE_SOUND_EFFECT_DIST = SOUND_EFFECT_DIST * SOUND_EFFECT_DIST;

constexpr float minimal_plane_distance_between_liquid_particles = 0.2f;

class CPHParticlesPlayCall : public CPHAction, public CPHReqComparerV
{
    LPCSTR ps_name;
    dContactGeom c;

    u32 remove_time;

    bool b_called{};
    int psLifeTime{};

    const CPhysicsShellHolder* m_object;

public:
    CPHParticlesPlayCall(const dContactGeom& contact, bool invert_n, LPCSTR psn, const CPhysicsShellHolder* object = nullptr)
    {
        ps_name = psn;
        c = contact;
        if (invert_n)
        {
            c.normal[0] = -c.normal[0];
            c.normal[1] = -c.normal[1];
            c.normal[2] = -c.normal[2];
        }
        m_object = object;

        constexpr u32 time_to_call_remove = 3000;
        remove_time = Device.dwTimeGlobal + time_to_call_remove;
    }

    virtual void run()
    {
        if (b_called)
            return;

        b_called = true;

        CParticlesObject* ps = CParticlesObject::Create(ps_name, TRUE);
        psLifeTime = ps->LifeTime();

        Fmatrix pos;
        Fvector zero_vel = {0.f, 0.f, 0.f};
        pos.k.set(*((Fvector*)c.normal));
        Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
        pos.c.set(*((Fvector*)c.pos));

        ps->UpdateParent(pos, zero_vel);
        GamePersistent().ps_needtoplay.push_back(ps);

        if (m_object && psLifeTime > 0)
            remove_time = Device.dwTimeGlobal + iFloor(psLifeTime / 2.f);
    };

    virtual bool obsolete() const noexcept { return Device.dwTimeGlobal > remove_time; }

    const CPhysicsShellHolder* object() const noexcept { return m_object; }
    const Fvector& position() const noexcept { return cast_fv(c.pos); }

    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
};

class CPHParticlesCondition : public CPHCondition, public CPHReqComparerV
{
private:
    virtual bool compare(const CPHReqComparerV* v) const noexcept { return v->compare(this); }

    virtual bool is_true() noexcept override { return true; }

    virtual bool obsolete() const noexcept { return false; }
};


class CPHFindParticlesComparer : public CPHReqComparerV
{
    Fvector m_position;
    const CPhysicsShellHolder* m_object;

public:
    CPHFindParticlesComparer(const Fvector& position, const CPhysicsShellHolder* object = nullptr) : m_position(position), m_object(object) {}

private:
    virtual bool compare(const CPHReqComparerV* v) const { return v->compare(this); }
    virtual bool compare(const CPHParticlesCondition* v) const { return true; }
    virtual bool compare(const CPHParticlesPlayCall* v) const
    {
        VERIFY(v);

        if (m_object)
            return m_object == v->object();

        Fvector disp = Fvector().sub(m_position, v->position());
        return disp.x * disp.x + disp.z * disp.z < (minimal_plane_distance_between_liquid_particles * minimal_plane_distance_between_liquid_particles);
    }
};

class CPHWallMarksCall : public CPHAction
{
    wm_shader pWallmarkShader;
    Fvector pos;
    CDB::TRI* T;

public:
    CPHWallMarksCall(const Fvector& p, CDB::TRI* Tri, const wm_shader& s)
    {
        pWallmarkShader = s;
        pos.set(p);
        T = Tri;
    }
    virtual void run()
    {
        // добавить отметку на материале
        ::Render->add_StaticWallmark(pWallmarkShader, pos, 0.09f, T, Level().ObjectSpace.GetStaticVerts());
    };
    virtual bool obsolete() const { return false; }
};

static CPHSoundPlayer* object_snd_player(dxGeomUserData* data) noexcept { return data->ph_ref_object ? data->ph_ref_object->ph_sound_player() : nullptr; }

template <class Pars>
void TContactShotMark(CDB::TRI* T, dContactGeom* c)
{
    dBodyID b = dGeomGetBody(c->g1);
    dxGeomUserData* data;
    bool b_invert_normal = false;
    if (!b)
    {
        b = dGeomGetBody(c->g2);
        data = dGeomGetUserData(c->g2);
        b_invert_normal = true;
    }
    else
    {
        data = dGeomGetUserData(c->g1);
    }
    if (!b)
        return;
    dVector3 vel;
    dMass m;
    dBodyGetMass(b, &m);
    dBodyGetPointVel(b, c->pos[0], c->pos[1], c->pos[2], vel);
    dReal vel_cret = dFabs(dDOT(vel, c->normal)) * _sqrt(m.mass);
    Fvector to_camera;
    to_camera.sub(cast_fv(c->pos), Device.vCameraPosition);
    float square_cam_dist = to_camera.square_magnitude();
    if (data)
    {
        SGameMtlPair* mtl_pair = GMLib.GetMaterialPair(T->material, data->material);
        if (mtl_pair)
        {
            if (vel_cret > Pars::vel_cret_wallmark && !mtl_pair->m_pCollideMarks->empty())
            {
                wm_shader WallmarkShader = mtl_pair->m_pCollideMarks->GenerateWallmark();
                Level().ph_commander().add_call(xr_new<CPHOnesCondition>(), xr_new<CPHWallMarksCall>(*((Fvector*)c->pos), T, WallmarkShader));
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (square_cam_dist < SQUARE_SOUND_EFFECT_DIST && !mtl_pair->CollideSounds.empty())
            {
                SGameMtl* static_mtl = GMLib.GetMaterialByIdx(T->material);
                VERIFY(static_mtl);
                if (static_mtl->Flags.test(SGameMtl::flPassable))
                {
                    if (auto sp = object_snd_player(data); sp)
                        sp->Play(mtl_pair, (Fvector*)c->pos, true, nullptr);
                }
                else
                {                  
                    float volume = collide_volume_min;

                    if (vel_cret > Pars::vel_cret_sound)
                    {
                        volume =+ vel_cret * (collide_volume_max - collide_volume_min) / (_sqrt(mass_limit) * default_l_limit - Pars::vel_cret_sound);
                    }

                    if (auto sp = object_snd_player(data); sp)
                        sp->PlayNext(mtl_pair, (Fvector*)c->pos, true, &volume);
                    else
                        GET_RANDOM(mtl_pair->CollideSounds).play_no_feedback(nullptr, 0, 0, ((Fvector*)c->pos), &volume);
                }
            }
            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (square_cam_dist < SQUARE_PARTICLE_EFFECT_DIST)
            {
                if (vel_cret > Pars::vel_cret_particles && !mtl_pair->CollideParticles.empty())
                {
                    LPCSTR ps_name = *mtl_pair->CollideParticles[::Random.randI(0, mtl_pair->CollideParticles.size())];

                    // отыграть партиклы столкновения материалов
                    CPHFindParticlesComparer find(cast_fv(c->pos), data->ph_ref_object);
                    if (!Level().ph_commander().has_call(&find, &find))
                    {
                        MsgDbg("! Adding collide particle for obj id=%d", data->ph_ref_object->ID());
                        Level().ph_commander().add_call(xr_new<CPHParticlesCondition>(), xr_new<CPHParticlesPlayCall>(*c, b_invert_normal, ps_name, data->ph_ref_object));
                    }
                    else
                        MsgDbg("~ Skip collide particle...");
                }
            }
        }
    }
}

ContactCallbackFun* ContactShotMark = &TContactShotMark<EffectPars>;
ContactCallbackFun* CharacterContactShotMark = &TContactShotMark<CharacterEffectPars>;