#pragma once

#include "script_object.h"
#include "script_export_space.h"

class CLAItem;

class CProjector: public CScriptObject {
	typedef	CScriptObject		inherited;

	friend void		BoneCallbackX(CBoneInstance *B);
	friend void		BoneCallbackY(CBoneInstance *B);

	float			fBrightness;
	CLAItem*		lanim;
	Fvector			m_pos;
	ref_light		light_render;
	ref_glow		glow_render;

	u16				guid_bone;

	struct SBoneRot {
		float	velocity;
		u16		id;
	} bone_x, bone_y;

	struct {
		float	yaw;
		float	pitch;
	} _start, _current, _target;

public:
					CProjector		();
	virtual			~CProjector		();

	virtual void	Load			( LPCSTR section);
	virtual BOOL	net_Spawn		( CSE_Abstract* DC);
	virtual void	shedule_Update	( u32 dt);							// Called by sheduler
	virtual void	UpdateCL		( );								// Called each frame, so no need for dt
	virtual void	renderable_Render( );

	virtual BOOL	UsedAI_Locations();

	virtual	bool	bfAssignWatch(CScriptEntityAction	*tpEntityAction);
	virtual	bool	bfAssignObject(CScriptEntityAction *tpEntityAction);

	Fvector GetCurrentDirection	();

	void	TurnOn			();
	void	TurnOff			();

	virtual	void			Hit(SHit* pHDS);

	IC		void	SetCurrentYaw		(float _yaw)		{ _current.yaw = _yaw; };
	IC		void	SetTargetYaw		(float _yaw)		{ _target.yaw = _yaw; };
	IC		void	SetCurrentPitch		(float _pitch)		{ _current.pitch = _pitch; };
	IC		void	SetTargetPitch		(float _pitch)		{ _target.pitch = _pitch; };
	IC		float	GetCurrentYaw		()	const			{ return _current.yaw; };
	IC		float	GetTargetYaw		()	const			{ return _target.yaw; };
	IC		float	GetCurrentPitch		()	const			{ return _current.pitch; };
	IC		float	GetTargetPitch		()	const			{ return _target.pitch; };

private:
	// Rotation routines
	static void		BoneCallbackX(CBoneInstance *B);
	static void		BoneCallbackY(CBoneInstance *B);

	void			SetTarget		(const Fvector &target_pos);
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CProjector)
#undef script_type_list
#define script_type_list save_type_list(CProjector)