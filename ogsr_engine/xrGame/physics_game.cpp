#include "stdafx.h"
#include "ParticlesObject.h"
#include "gamemtllib.h"
#include "level.h"
#include "gamepersistent.h"
#include "Extendedgeom.h"
#include "PhysicsGamePars.h"
#include "PhysicsCommon.h"
#include "PhSoundPlayer.h"
#include "PhysicsShellHolder.h"
#include "PHCommander.h"
#include "PHReqComparer.h"
#include "MathUtils.h"
#include "PHWorld.h"

/////////////////////////////////////////////////////////////
bool ContactShotMarkGetEffectPars(dContactGeom* c, dxGeomUserData* &data, float &vel_cret, bool &b_invert_normal);

static constexpr float PARTICLE_EFFECT_DIST = 70.f;
static constexpr float SOUND_EFFECT_DIST = 70.f;
static constexpr float SQUARE_PARTICLE_EFFECT_DIST = PARTICLE_EFFECT_DIST * PARTICLE_EFFECT_DIST;
static constexpr float SQUARE_SOUND_EFFECT_DIST = SOUND_EFFECT_DIST * SOUND_EFFECT_DIST;
static constexpr float minimal_plane_distance_between_liquid_particles = 0.2f;
/////////////////////////////////////////////////////////////
class CPHParticlesPlayCall : public CPHAction
{
	const char* ps_name;
protected:
	dContactGeom c;
public:
	CPHParticlesPlayCall(const dContactGeom &contact, bool invert_n, const char* psn)
	{
		ps_name = psn;
		c = contact;
		if (invert_n)
		{
			c.normal[0] = -c.normal[0]; 
			c.normal[1] = -c.normal[1]; 
			c.normal[2] = -c.normal[2];
		}
	}

	virtual void run()
	{
		CParticlesObject* ps = CParticlesObject::Create(ps_name, TRUE);

		Fmatrix pos;
		Fvector zero_vel = { 0.f,0.f,0.f };
		pos.k.set(*((Fvector*)c.normal));
		Fvector::generate_orthonormal_basis(pos.k, pos.j, pos.i);
		pos.c.set(*((Fvector*)c.pos));

		ps->UpdateParent(pos, zero_vel);
		GamePersistent().ps_needtoplay.push_back(ps);
	}

	virtual bool obsolete() const noexcept
	{ 
		return false; 
	}
};

class CPHLiquidParticlesPlayCall : public CPHParticlesPlayCall, public CPHReqComparerV
{
	u32 remove_time;
	bool b_called;
public:
	CPHLiquidParticlesPlayCall(const dContactGeom &contact, bool invert_n, const char* psn) :
								CPHParticlesPlayCall(contact, invert_n, psn), b_called(false)
	{
		static constexpr u32 time_to_call_remove = 3000;
		remove_time = Device.dwTimeGlobal + time_to_call_remove;
	}

	const Fvector &position() const noexcept
	{
		return cast_fv(c.pos);
	}

private:

	virtual bool compare(const	CPHReqComparerV* v) const
	{ 
		return v->compare(this); 
	}

	virtual void run()
	{
		if (b_called)
			return;

		b_called = true;
		CPHParticlesPlayCall::run();
	}

	virtual bool obsolete() const noexcept
	{ 
		return Device.dwTimeGlobal > remove_time;
	}
};


class CPHLiquidParticlesCondition : public CPHCondition, public CPHReqComparerV
{
private:
	virtual bool compare(const	CPHReqComparerV* v) const noexcept
	{ 
		return v->compare(this); 
	}

	virtual bool is_true() noexcept override
	{
		return true; 
	}

	virtual bool obsolete() const noexcept 
	{ 
		return false; 
	}
};

class CPHFindLiquidParticlesComparer : public CPHReqComparerV
{
	Fvector m_position;
public:
	CPHFindLiquidParticlesComparer(const Fvector &position) : m_position(position) {}
private:
	virtual bool			compare(const	CPHReqComparerV* v)					const { return v->compare(this); }
	virtual bool			compare(const	CPHLiquidParticlesCondition* v)		const { return true; }
	virtual bool			compare(const	CPHLiquidParticlesPlayCall* v)		const
	{
		VERIFY(v);

		Fvector	disp = Fvector().sub(m_position, v->position());
		return disp.x*disp.x + disp.z*disp.z < (minimal_plane_distance_between_liquid_particles * minimal_plane_distance_between_liquid_particles);
	}
};

class CPHWallMarksCall : public CPHAction
{
	ref_shader pWallmarkShader;
	Fvector pos;
	CDB::TRI* T;
public:
	CPHWallMarksCall(const Fvector &p, CDB::TRI* Tri, const ref_shader &s) : pWallmarkShader(s), T(Tri)
	{
		pos.set(p);
	}

	virtual void run() noexcept
	{
		//добавить отметку на материале
		::Render->add_StaticWallmark(pWallmarkShader, pos, 0.09f, T, Level().ObjectSpace.GetStaticVerts());
	};

	virtual bool obsolete() const noexcept
	{ 
		return false; 
	}
};

static void play_object(dxGeomUserData* data, SGameMtlPair* mtl_pair, const dContactGeom* c) noexcept
{
	if (data->ph_ref_object)
	{
		CPHSoundPlayer* sp = data->ph_ref_object->ObjectPhSoundPlayer();

		if (sp)
			sp->Play(mtl_pair, *(Fvector*)c->pos);
	}
}

template<class Pars>
inline bool play_liquid_particle_criteria(dxGeomUserData &data, float vel_cret) noexcept
{
	if (vel_cret > Pars::vel_cret_particles)
		return true;

	bool controller = !!data.ph_object && data.ph_object->CastType() == CPHObject::tpCharacter;
	return  !controller && vel_cret > Pars::vel_cret_particles / 4.f;
}


template<class Pars>
void play_particles(float vel_cret, dxGeomUserData* data, const dContactGeom* c, bool b_invert_normal, const SGameMtl* static_mtl, const char* ps_name)
{
	VERIFY(c);
	VERIFY(static_mtl);
	bool liquid = !!static_mtl->Flags.test(SGameMtl::flLiquid);

	const bool play_liquid = liquid && play_liquid_particle_criteria<Pars>(*data, vel_cret);
	const bool play_not_liquid = !liquid && vel_cret > Pars::vel_cret_particles;

	if (play_not_liquid)
	{
		Level().ph_commander().add_call(xr_new<CPHOnesCondition>(), xr_new<CPHParticlesPlayCall>(*c, b_invert_normal, ps_name));
	}
	else if (play_liquid)
	{
		CPHFindLiquidParticlesComparer find(cast_fv(c->pos));
		if (!Level().ph_commander().has_call(&find, &find))
			Level().ph_commander().add_call(xr_new<CPHLiquidParticlesCondition>(), xr_new<CPHLiquidParticlesPlayCall>(*c, b_invert_normal, ps_name));
	}
}

template<class Pars>
void TContactShotMark(CDB::TRI* T, dContactGeom* c)
{
	dxGeomUserData* data = nullptr;
	float vel_cret = 0;
	bool b_invert_normal = false;

	if (!ContactShotMarkGetEffectPars(c, data, vel_cret, b_invert_normal))
		return;

	Fvector to_camera; to_camera.sub(cast_fv(c->pos), Device.vCameraPosition);
	float square_cam_dist = to_camera.square_magnitude();
	if (data)
	{
		SGameMtlPair* mtl_pair = GMLib.GetMaterialPair(T->material, data->material);
		if (mtl_pair)
		{
			if (vel_cret > Pars::vel_cret_wallmark && !mtl_pair->CollideMarks.empty())
			{
				ref_shader WallmarkShader = mtl_pair->CollideMarks[::Random.randI(0,mtl_pair->CollideMarks.size())];
				Level().ph_commander().add_call(xr_new<CPHOnesCondition>(), xr_new<CPHWallMarksCall>(*((Fvector*)c->pos), T, WallmarkShader));
			}
			
			if (square_cam_dist < SQUARE_SOUND_EFFECT_DIST)
			{
				SGameMtl* static_mtl = GMLib.GetMaterialByIdx(T->material);
				VERIFY(static_mtl);
				if (!static_mtl->Flags.test(SGameMtl::flPassable) && vel_cret > Pars::vel_cret_sound)
				{
					if (!mtl_pair->CollideSounds.empty())
					{
						float volume = collide_volume_min + vel_cret * (collide_volume_max - collide_volume_min) / (_sqrt(mass_limit)*default_l_limit - Pars::vel_cret_sound);
						GET_RANDOM(mtl_pair->CollideSounds).play_no_feedback(nullptr, 0, 0, ((Fvector*)c->pos), &volume);
					}
				}
				else if (data->ph_ref_object && !mtl_pair->CollideSounds.empty())
				{
					play_object(data, mtl_pair, c);
				}
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (square_cam_dist < SQUARE_PARTICLE_EFFECT_DIST && !mtl_pair->CollideParticles.empty())
			{
				SGameMtl* static_mtl = GMLib.GetMaterialByIdx(T->material);
				VERIFY(static_mtl);
				const char* ps_name = *mtl_pair->CollideParticles[::Random.randI(0, mtl_pair->CollideParticles.size())];
				play_particles<Pars>(vel_cret, data, c, b_invert_normal, static_mtl, ps_name);
			}
		}
	}
}

ContactCallbackFun *ContactShotMark = &TContactShotMark<EffectPars>;
ContactCallbackFun *CharacterContactShotMark = &TContactShotMark<CharacterEffectPars>;
