#pragma once


#include "IK/IKLimb.h"

class CKinematicsAnimated;
class CGameObject	;
class CBlend		;
struct SIKCrlCalcData;

class CIKLimbsController {
public:
					CIKLimbsController			( );
			void	Create						( CGameObject *O );
			void	Destroy						( CGameObject *O );
public:
			void	PlayLegs					( CBlend *b );
			void	Update						( );
private:
			void	Calculate					( );
			void	LimbCalculate				(  SCalculateData &cd );
			void	LimbUpdate					( CIKLimb &L, u16 i );
			void	LimbSetup					( const u16 bones[4] );
			//void	UpdateTracks				();
private:
	static	void __stdcall 	IKVisualCallback	( CKinematics* K );

private:
	CBlend				*m_legs_blend;
	CGameObject			*m_object;
	xr_vector<CIKLimb>	_bone_chains;
	enum{
		left_arm	=0,
		right_arm	,
		left_leg	,
		right_leg	,

	};

#ifdef	DEBUG
	LPCSTR			anim_name;
	LPCSTR			anim_set_name;
#endif
/*
	CIKLimb				m_left_leg;
	CIKLimb				m_right_leg;
	CIKLimb				m_left_arm;
	CIKLimb				m_right_arm;
*/
};
