#pragma once
class CEntityAlive;

#include "../SkeletonAnimated.h"
class character_hit_animation_controller
{
public:
	void								SetupHitMotions					( CKinematicsAnimated &ca );
	void								PlayHitMotion					( const Fvector &dir, const Fvector &bone_pos, u16 bi, CEntityAlive &ea )const;
	void								GetBaseMatrix					( Fmatrix &m, CEntityAlive &ea)const;
private:
	bool								IsEffected						( u16	bi, CKinematics &ca  )const;

protected:
private:
	//
	u16									base_bone;			
	MotionID							bkhit_motion;
	MotionID							fvhit_motion;
	MotionID							rthit_motion;
	MotionID							lthit_motion;
	MotionID							turn_right;
	MotionID							turn_left;
	MotionID							all_shift_down;
	MotionID							hit_downl;
	MotionID							hit_downr;
	static	const	u16					num_anims = 7;
	mutable u32							block_times[num_anims];
	//
};