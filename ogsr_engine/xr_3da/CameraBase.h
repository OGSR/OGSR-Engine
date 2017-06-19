// CameraBase.h: interface for the CCameraBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERABASE_H__B11F8AE1_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_CAMERABASE_H__B11F8AE1_1213_11D4_B4E3_4854E82A090D__INCLUDED_
#pragma once

#include "CameraDefs.h"

// refs
class CObject;

class ENGINE_API	CCameraBase
{
protected:
	CObject*		parent;

public:
	BOOL			bClampYaw, bClampPitch, bClampRoll;
	float			yaw,pitch,roll;

	enum{
		flRelativeLink		= (1<<0),
		flPositionRigid		= (1<<1),
		flDirectionRigid	= (1<<2),
	};
	Flags32			m_Flags;

	ECameraStyle	style;
	Fvector2		lim_yaw,lim_pitch,lim_roll;
	Fvector			rot_speed;

	Fvector			vPosition;
	Fvector			vDirection;
	Fvector			vNormal;
	float			f_fov;
	float			f_aspect;

	int				tag;
public:
					CCameraBase		( CObject* p, u32 flags );
	virtual			~CCameraBase	( );
	virtual void	Load			(LPCSTR section);
	void			SetParent		( CObject* p )								{parent=p; VERIFY(p);}
	virtual	void	OnActivate		( CCameraBase* old_cam )					{;}
	virtual	void	OnDeactivate	( )											{;}
	virtual void	Move			( int cmd, float val=0, float factor=1.0f)	{;}
	virtual void	Update			( Fvector& point, Fvector& noise_angle )	{;}
	virtual void	Get				( Fvector& P, Fvector& D, Fvector& N )		{P.set(vPosition);D.set(vDirection);N.set(vNormal);}
	virtual void	Set				( const Fvector& P, const Fvector& D, const Fvector& N ){vPosition.set(P);vDirection.set(D);vNormal.set(N);}
	virtual void	Set				( float Y, float P, float R )				{yaw=Y;pitch=P;roll=R;}
	
	virtual float	GetWorldYaw		( )	{ return 0; };
	virtual float	GetWorldPitch	( )	{ return 0; };

	virtual float	CheckLimYaw		( );
	virtual float	CheckLimPitch	( );
	virtual float	CheckLimRoll	( );

	IC		void	SetLimYaw		(Fvector2 _lim_yaw)		{ lim_yaw = _lim_yaw; };
	IC		void	SetLimPitch		(Fvector2 _lim_pitch)	{ lim_pitch = _lim_pitch; };
	IC		void	SetRotSpeed		(Fvector _rot_speed)	{ rot_speed = _rot_speed; };
	IC		void	SetYaw			(float _yaw)			{ yaw = _yaw; };
	IC		Fvector2	GetLimYaw()		const { return lim_yaw; };
	IC		Fvector2	GetLimPitch()	const { return lim_pitch; };
	IC		Fvector		GetRotSpeed()	const { return rot_speed; };
	IC		float		GetYaw()		const { return yaw; };
};

#include "xrGame\script_export_space.h"

typedef class_exporter<CCameraBase> CCameraScript;
add_to_type_list(CCameraScript)
#undef script_type_list
#define script_type_list save_type_list(CCameraScript)

#endif // !defined(AFX_CAMERABASE_H__B11F8AE1_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
