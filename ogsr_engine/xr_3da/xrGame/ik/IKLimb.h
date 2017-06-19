#pragma once


#include "limb.h"
#include "../../SkeletonAnimated.h"
#include "../ik_anim_state.h"
class	CKinematics	;
class	CDB::TRI	;
struct SCalculateData;
struct SIKCollideData;
class  CGameObject;
class motion_marks;
struct SIKCollideData
{
	//CDB::TRI		*m_tri		;
	Fplane			m_plane		;
	Fvector			m_collide	;
	Fvector			m_anime		;
	Fvector			m_pick_dir	;
	bool			collided	;
	bool			clamp_down	;
	SIKCollideData	(): collided( false ), clamp_down( false ){}
};

struct calculate_state
{
	u32		frame;

	Fmatrix goal;
	Fmatrix anim_pos;
	Fmatrix	obj_pos;
	Fmatrix	collide_pos;
	Fvector pick;
	float	speed_blend_l;
	float	speed_blend_a;
	bool	foot_step;
	bool	blending;
#ifdef DEBUG
	int		count;
#endif
	calculate_state() : frame(0), foot_step(false), blending(false),
						anim_pos(Fidentity), speed_blend_l(0), speed_blend_a(0),
						pick(Fvector().set(0,0,0))
#ifdef DEBUG
						, count(-1)
#endif
	{}
};

class CIKLimb {
public:
							CIKLimb				();
				void		Create				( u16 id, CKinematics* K, const u16 bones[3], const Fvector& toe_pos, bool collide_ );	
				void		Destroy				( );
				void		Calculate			( SCalculateData& cd );
				void		Update				( CGameObject *O, const	CBlend *b, u16 interval );
IC				u16			get_id				()	{ return m_id; }
private:
				void		Invalidate			();
				void		GetFootStepMatrix	( Fmatrix	&m, const Fmatrix &gl_anim, const  SIKCollideData &cld, bool collide );
IC				float		CollideFoot			( float angle, const Fmatrix &gl_anim, Fplane &p, Fvector &ax );
IC				void		make_shift			(Fmatrix &xm, const Fplane &p,const Fvector &pick_dir );
				void		ApplyContext		( SCalculateData& cd );
				void		Solve				( SCalculateData& cd );
				void		Collide				( SIKCollideData &cld, CGameObject *O, const Fmatrix &foot, bool foot_step );
IC				void		AnimGoal			( Fmatrix &gl, CKinematicsAnimated	&K );
				void		SetAnimGoal			( SCalculateData& cd );
				void		SetNewGoal			( const SIKCollideData &cld, SCalculateData& cd );
				void		CalculateBones		(SCalculateData& cd);
				Matrix&		Goal				( Matrix &gl, const Fmatrix &xm, SCalculateData& cd );
				Fmatrix&	GetHipInvert		( Fmatrix &ihip, const SCalculateData& cd );
				float		SwivelAngle			( const Fmatrix &ihip, const SCalculateData& cd );
				void		GetKnee				( Fvector &knee, const SCalculateData& cd ) const;
				void		GetPickDir			(Fvector &v, const Fmatrix &gl_bone ) ;
IC		static	void		get_start			( Fmatrix &start, SCalculateData &D, u16 bone );
private:
		static	void 		BonesCallback0		( CBoneInstance* B );
		static	void 		BonesCallback1		( CBoneInstance* B );
		static	void 		BonesCallback2		( CBoneInstance* B );

private:
	Limb		m_limb;
	Fvector		m_toe_position;
	u16			m_bones[4];	
	u16			m_id;
	bool		m_collide;
	SIKCollideData	collide_data;
	ik_anim_state	anim_state;
	calculate_state	sv_state;
#ifdef DEBUG
	calculate_state	sv_state_DBR;
#endif
};

//#include <boost/noncopyable.hpp>
class	ik_anim_state;
struct SCalculateData /*: private boost::noncopyable */{

	float	const		*m_angles			;
	CKinematicsAnimated	*m_K				;
	CIKLimb				&m_limb				;
	Fmatrix	const		&m_obj				;

	bool				do_collide  		;

	Fmatrix				goal				;
	bool				apply				;
	bool				foot_step			;


//	const BlendSVec		&anim_base			;
//	const motion_vec	&uneffected_motions	;

	SCalculateData(CIKLimb& l,CKinematicsAnimated	*K,const Fmatrix &o):
	m_limb(l), m_obj(o), m_K(K), m_angles(0), apply(false), 
	do_collide(false), foot_step(false) {}
	SCalculateData(const SCalculateData&) = delete;
	void operator=(const SCalculateData&) = delete;
};
