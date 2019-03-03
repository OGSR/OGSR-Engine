#include "StdAfx.h"

#include "../xr_3da/IGame_Level.h"

#include "death_anims.h"
#include "CharacterPhysicsSupport.h"
#include "Weapon.h"
#include "WeaponShotgun.h"
#include "Explosive.h"

rnd_motion::rnd_motion()
{
}

rnd_motion*	rnd_motion::setup(CKinematicsAnimated* k, const char* s)
{
	VERIFY(k);
	VERIFY(s);

	const u16 nb = (u16)_GetItemCount(s);
	for (u16 i = 0; nb > i; ++i)
	{
		string64 n;
		_GetItem(s, i, n);
		MotionID m = k->LL_MotionID(n);
		if (m.valid())
		{
			motions.push_back(m);
		}
#ifdef	DEBUG
		else
			Msg("death_anims: motion: %s not found!", n);
#endif
	}
	return this;
}

MotionID rnd_motion::motion()	const
{
	if (motions.empty())
		return MotionID();
	return motions[::Random.randI(0, motions.size())];
}

void type_motion::clear()
{
	std::for_each(anims.begin(), anims.end(), vec_clear<rnd_motion>);
	anims.clear();
}

void type_motion::set_motion(CKinematicsAnimated* k, u16 id_motion, const char* dir_anim)
{
	anims[id_motion] = xr_new<rnd_motion>()->setup(k, dir_anim);
}

type_motion* type_motion::setup(CKinematicsAnimated* k, CInifile * ini, const char* section, const char* type)
{
	anims.resize(dirs_number, 0);
	if (ini->line_exist(section, type))
	{
		const char* line = ini->r_string(section, type);
		if (!line)
			return this;

		R_ASSERT(xr_strlen(line) < 1023);
		const int num = _GetItemCount(line, '/');
#ifdef	DEBUG
		Msg("death anims: load: no setings in section %s for %s", section, type);
#endif
		for (int i = 0; num > i; ++i)
		{
			string1024 sdir_anim;
			set_motion(k, u16(i), _GetItem(line, i, sdir_anim, '/'));
#ifdef	DEBUG
			Msg("death anims: load: loaded %s from section %s for %s", sdir_anim, section, type);
#endif
		}
	}
#ifdef	DEBUG
	else
		Msg("death anims: load: no setings in section %s for %s", section, type);

#endif
	return this;
}

type_motion::~type_motion()
{
	clear();
	VERIFY(anims.empty());
}

MotionID type_motion::motion(type_motion::edirection dr)const
{
	VERIFY(dr < not_definite);
	rnd_motion *rm = anims[dr];
	MotionID m;
	if (rm)
		m = rm->motion();
	return m;
}

type_motion::edirection	type_motion::dir(CEntityAlive& ea, const SHit& H, float& angle)
{
	Fvector dir = H.direction();
	dir.y = 0;
	float m = dir.magnitude();
	if (fis_zero(m))
	{
		edirection dr;
		dr = (edirection) ::Random.randI(0, (s32)not_definite);
		VERIFY(dr < not_definite);
		return	dr;
	}
	dir.mul(1.f / m);

	Fvector z_dir = { ea.XFORM().k.x, 0.f, ea.XFORM().k.z };
	Fvector x_dir = { ea.XFORM().i.x, 0.f, ea.XFORM().i.z };
	z_dir.normalize_safe(); x_dir.normalize_safe();

	float front_factor = dir.dotproduct(z_dir);
	float sidefactor = dir.dotproduct(x_dir);

	if (_abs(front_factor) > M_SQRT1_2)
	{
		float sign = front_factor < 0.f ? -1.f : 1.f;

		angle = atan2(-sign * sidefactor, sign * front_factor);

		return sign < 0.f ? front : back;
	}
	else
	{
		float sign = sidefactor > 0.f ? 1.f : -1.f;

		angle = atan2(sign * front_factor, sign * sidefactor);

		return sign > 0.f ? left : right;
	}
}

death_anims::death_anims()
{
}

death_anims::~death_anims()
{
	clear();
	VERIFY(anims.empty());
}

void death_anims::clear()
{
	std::for_each(anims.begin(), anims.end(), vec_clear<type_motion>);
	anims.clear();
}

MotionID death_anims::motion(CEntityAlive& ea, const SHit& H, float &angle) const
{
	angle = 0;
	if (anims.empty())
		return rnd_anims.motion();

	MotionID m;
	xr_vector<type_motion*>::const_iterator i = anims.begin(), e = anims.end();
	for (; e != i; ++i)
		if ((*i)->predicate(ea, H, m, angle) && m.valid())
			return m;

	angle = 0;
	//Msg("No specific anim was found, use random");
	return rnd_anims.motion();
}

Fvector& global_hit_position(Fvector &gp, CEntityAlive& ea, const SHit& H)
{
	VERIFY(ea.Visual());
	CKinematics	*K = ea.Visual()->dcast_PKinematics();
	VERIFY(K);
	K->LL_GetTransform(H.bone()).transform_tiny(gp, H.bone_space_position());
	ea.XFORM().transform_tiny(gp);
	return gp;
}

#pragma warning(push)
#pragma warning(disable: 4273)

bool find_in_parents(const u16 bone_to_find, const u16 from_bone, CKinematics &ca)
{
	const u16 root = ca.LL_GetBoneRoot();

	for (u16 bi = from_bone; bi != root && bi != BI_NONE; )
	{
		const CBoneData &bd = ca.LL_GetData(bi);
		if (bi == bone_to_find)
			return true;
		bi = bd.GetParentID();
	}

	return false;
}

inline bool is_bone_head(CKinematics &K, u16 bone)
{
	const u16 head_bone = K.LL_BoneID("bip01_head");
	const u16 neck_bone = K.LL_BoneID("bip01_neck");
	return (bone != BI_NONE) && neck_bone == bone || find_in_parents(head_bone, bone, K);
}

//1.	Инерционное движение вперед от попадания в голову 
class type_motion0 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle)	const override
	{
		m = MotionID();
		if (H.initiator() != g_pGameLevel->CurrentEntity())
			return false;

		VERIFY(pEntity.Visual());
		CKinematics *K = pEntity.Visual()->dcast_PKinematics();
		VERIFY(K);
		if (!is_bone_head(*K, H.bone()))
			return false;

		CCharacterPhysicsSupport* chs = pEntity.character_physics_support();
		if (!chs || chs->Type() == CCharacterPhysicsSupport::etBitting)
			return false;

		const Fvector stalker_velocity = chs->MovementVelocity();
		const float stalker_speed = stalker_velocity.magnitude();
		const float min_stalker_speed = 3.65f;
		if (stalker_speed < min_stalker_speed)
			return false;

		const Fvector stalker_velocity_dir = Fvector().mul(stalker_velocity, 1.f / stalker_speed);
		const Fvector dir_to_actor = Fvector().sub(H.initiator()->Position(), pEntity.Position()).normalize_safe();
		const float front_angle_cos = _cos(deg2rad(20.f));

		if (stalker_velocity_dir.dotproduct(dir_to_actor) < front_angle_cos)
			return false;

		if (type_motion::front != dir(pEntity, H, angle))
			return false;

		Fvector p;
		if (Fvector().sub(H.initiator()->Position(), global_hit_position(p, pEntity, H)).magnitude() > 30.f)
			return false;

		//Msg("type_motion0 death...");
		m = motion(front);
		return true;
	}
};

//2.	Изрешетить пулями (заглушка)
class type_motion1 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle) const override
	{
		m = MotionID();

		return false;
	}
};

//3.	Шотган 
class type_motion2 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle) const override
	{
		m = MotionID();
		if (H.initiator() != g_pGameLevel->CurrentEntity())
			return false;

		CObject* O = Level().Objects.net_Find(H.weaponID);
		if (!O)
			return false;

		CWeaponShotgun* pShotgun = smart_cast<CWeaponShotgun*>(O);
		if (!pShotgun)
			return false;
		Fvector p;

		const	float max_distance = 20.f;
		if (Fvector().sub(H.initiator()->Position(), global_hit_position(p, pEntity, H)).magnitude() > max_distance)
			return false;

		edirection dr = dir(pEntity, H, angle);
		//Msg("type_motion2 death...");
		m = motion(dr);
		return true;
	}
};

//4.	Хедшот (по вероятности), кроме 5 (4) 
class type_motion3 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle) const override
	{
		m = MotionID();
		if (H.initiator() != g_pGameLevel->CurrentEntity())
			return false;

		VERIFY(pEntity.Visual());
		CKinematics *K = pEntity.Visual()->dcast_PKinematics();
		VERIFY(K);

		if (is_bone_head(*K, H.bone()))
		{
			edirection dr = dir(pEntity, H, angle);
			//Msg("type_motion3 death...");
			m = motion(dr);
			return true;
		}
		return false;
	}
};

bool is_snipper(u16 weaponID)
{
	CObject* O = Level().Objects.net_Find(weaponID);
	if (!O)
		return false;

	CWeapon* WM = smart_cast<CWeapon*>(O);
	return WM && WM->IsZoomed() && WM->IsScopeAttached();
}

//5.	Снайперка в голову. 
class type_motion4 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle) const override
	{
		if (H.initiator() != g_pGameLevel->CurrentEntity())
			return false;

		m = MotionID();
		VERIFY(pEntity.Visual());
		CKinematics *K = pEntity.Visual()->dcast_PKinematics();
		VERIFY(K);

		if (!is_bone_head(*K, H.bone()))
			return false;

		if (is_snipper(H.weaponID))
		{
			edirection dr = dir(pEntity, H, angle);
			m = motion(dr);
			//Msg("type_motion4 death...");
			return true;
		}
		return false;
	}
};

//6.	Снайперка в тело. 
class type_motion5 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle) const override
	{
		if (H.initiator() != g_pGameLevel->CurrentEntity())
			return false;

		m = MotionID();
		VERIFY(pEntity.Visual());
		CKinematics *K = pEntity.Visual()->dcast_PKinematics();
		VERIFY(K);

		if (is_snipper(H.weaponID) && !is_bone_head(*K, H.bone()))
		{
			edirection dr = dir(pEntity, H, angle);
			m = motion(dr);
			//Msg("type_motion5 death...");
			return true;
		}
		return false;
	}
};

//7.	Гранта 
class type_motion6 : public type_motion
{
	bool predicate(CEntityAlive& pEntity, const SHit& H, MotionID &m, float &angle) const override
	{
		if (H.initiator() != g_pGameLevel->CurrentEntity())
			return false;

		if (H.type() == ALife::eHitTypeExplosion) // HACK
		{
			edirection dr = dir(pEntity, H, angle);
			m = motion(dr);
			//Msg("type_motion6 death...");
			return true;
		}

		CObject* O = Level().Objects.net_Find(H.weaponID);
		if (!O)
		{
			m = MotionID();
			return false;
		}

		if (smart_cast<CExplosive*>(O) != nullptr)
		{
			edirection dr = dir(pEntity, H, angle);
			m = motion(dr);
			//Msg("type_motion6 death...");
			return true;
		}

		return false;
	}
};

void death_anims::setup(CKinematicsAnimated* k, LPCSTR section, CInifile* ini)
{
	clear();

	VERIFY(k);
	VERIFY(section);
	VERIFY(ini);
	VERIFY(anims.empty());

	anims.resize(types_number);

	//1.	Инерционное движение вперед от попадания в голову
	anims[0] = xr_new<type_motion0>()->setup(k, ini, section, "kill_enertion");
	//2.	Изрешетить пулями (заглушка)
	anims[1] = xr_new<type_motion1>()->setup(k, ini, section, "kill_burst");
	//3.	Шотган
	anims[2] = xr_new<type_motion2>()->setup(k, ini, section, "kill_shortgun");
	//7.	Гранта
	anims[3] = xr_new<type_motion6>()->setup(k, ini, section, "kill_grenade");
	//5.	Снайперка в голову
	anims[4] = xr_new<type_motion4>()->setup(k, ini, section, "kill_sniper_headshot");
	//6.	Снайперка в тело
	anims[5] = xr_new<type_motion5>()->setup(k, ini, section, "kill_sniper_body");
	//4.	Хедшот (по вероятности), кроме 5 (4)
	anims[6] = xr_new<type_motion3>()->setup(k, ini, section, "kill_headshot");

	if (ini->line_exist(section, "random_death_animations"))
		rnd_anims.setup(k, ini->r_string(section, "random_death_animations"));
}

#pragma warning(pop)