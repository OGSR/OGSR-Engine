#pragma once

struct SHit;
struct MotionID;

class  CInifile;
class  CKinematicsAnimated;
class  CEntityAlive;

template<typename T>
void vec_clear(T* &p)
{
	xr_delete(p);
}

class rnd_motion
{
private:
	xr_vector<MotionID> motions;

public:
	rnd_motion();

	rnd_motion* setup(CKinematicsAnimated* k, LPCSTR s);
	MotionID motion() const;
};

class type_motion
{
public:
	enum edirection
	{
		front = 0,
		back,
		left,
		right,
		not_definite
	};
	static const u16 dirs_number = 4;

public:
	type_motion() {}
	virtual				~type_motion();
	type_motion	*setup(CKinematicsAnimated* k, CInifile * ini, LPCSTR section, LPCSTR type);
	MotionID	motion(edirection dr)	const;
	virtual	bool		predicate(CEntityAlive& ea, const SHit& H, MotionID &m, float &angle) const = 0;
private:
	void set_motion(CKinematicsAnimated* k, u16 motion_id, LPCSTR dir_anim);
	void clear();
public:
	static	edirection	dir(CEntityAlive& ea, const SHit& H, float &angle);
private:
	xr_vector<rnd_motion*>		anims;
};

class death_anims
{
public:
	death_anims();
	~death_anims();
	void setup(CKinematicsAnimated* k, LPCSTR section, CInifile* ini);
	void clear();
	MotionID motion(CEntityAlive& ea, const SHit& H, float& angle)	const;

private:
	static const u16		types_number = 7;
	xr_vector<type_motion*>	anims;
	rnd_motion				rnd_anims;
};