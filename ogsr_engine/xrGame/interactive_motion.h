#pragma once
#include "..\xr_3da\skeletonanimated.h"
class CPhysicsShell;
class interactive_motion
{
	MotionID motion;

protected:

	Flags8			flags;
	CPhysicsShell	*shell;
	float			angle;
	enum Flag
	{
		fl_use_death_motion			=1<<4,
		fl_switch_dm_toragdoll		=1<<5,
		
		fl_started					=1<<6, 
		fl_not_played				=1<<7
	};
public:
			interactive_motion	( );
	virtual	~interactive_motion	( );
	void	init				( );
	void	destroy				( );
	void	setup				( LPCSTR m, CPhysicsShell *s, float angle );
	void	setup				( const MotionID &m, CPhysicsShell *s, float angle );
	
	void	update				( );
inline bool	is_enabled			( )	{ return !!flags.test( fl_use_death_motion ); }

	void	play( );

private:
virtual	void	move_update	( )=0;
virtual	void	collide		( )=0;

protected:
virtual	void	state_end	( );
virtual	void	state_start ( );

private:
//biped hack
		void	shell_setup				(  );

		//BoneCallbackFun bone_instance.set_callback(bctPhysics,
//
protected:
		void	switch_to_free	( );
protected:
static	void	anim_callback		( CBlend *B );


};

void interactive_motion_diagnostic( LPCSTR message, const MotionID &m, CPhysicsShell *s );
