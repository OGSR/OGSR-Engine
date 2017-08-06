#include "stdafx.h"

#include "interactive_motion.h"

#include "physicsshell.h"
#include "PhysicsShellHolder.h"
#include "MathUtils.h"


interactive_motion::interactive_motion()
{
	init();
}

void interactive_motion::init()
{
	flags.assign(0);
}

void interactive_motion::setup(LPCSTR m,CPhysicsShell *s)
{
	
	VERIFY(s);
	motion = smart_cast<CKinematicsAnimated*>(s->PKinematics())->LL_MotionID(m);
	if(motion.valid())
		flags.set(fl_use_death_motion,TRUE);

}

void interactive_motion::anim_callback(CBlend *B)
{
	VERIFY(B->CallbackParam);
	((interactive_motion*)(B->CallbackParam))->flags.set(fl_switch_dm_toragdoll,TRUE);
}

void interactive_motion::play(CPhysicsShell *s)
{
	VERIFY( s );
	smart_cast<CKinematicsAnimated*>( s->PKinematics( ) )->PlayCycle( motion, TRUE, anim_callback, this );
	state_start( s );
}

float depth = 0;
void  get_depth(bool& do_colide,bool bo1,dContact& c,SGameMtl * /*material_1*/,SGameMtl * /*material_2*/)
{
	save_max(depth, c.geom.depth);
}
void interactive_motion::state_start(CPhysicsShell *s)
{
	s->add_ObjectContactCallback(get_depth);
	collide(s);

	if(flags.test(fl_switch_dm_toragdoll))
	{
		flags.assign(0);
		s->remove_ObjectContactCallback(get_depth);
		return;
	}
}

void	interactive_motion::state_end(CPhysicsShell *s)
{
	flags.set( fl_switch_dm_toragdoll, FALSE );
	flags.set( fl_use_death_motion, FALSE );
	s->Enable( );
	s->remove_ObjectContactCallback( get_depth );
////set and velocities
	s->AnimToVelocityState( Device.fTimeDelta, default_l_limit * 10, default_w_limit * 10 );
}

void interactive_motion::update(CPhysicsShell *s)
{
	CKinematics *K  = s->PKinematics();
	VERIFY(K);
	K -> CalculateBones();
	collide(s);
	if(flags.test(fl_switch_dm_toragdoll))
	{
		switch_to_free(s);
	} else 
		move_update(s);
}

void	interactive_motion::switch_to_free(CPhysicsShell *s)
{
//set to normal state
	state_end(s);
///set all matrises valide
	CPhysicsShellHolder *obj = s->get_ElementByStoreOrder(0)->PhysicsRefObject();
	VERIFY(obj);
	s->InterpolateGlobalTransform(&obj->XFORM());
	CKinematics *K  = s->PKinematics();
	VERIFY(K);
	K->CalculateBones_Invalidate();
	K->CalculateBones(TRUE);
}

///////////////////////////////////////////////////////////////////////////////////////
void imotion_position::state_start(CPhysicsShell *s)
{
	inherited::state_start(s);
	if(!is_enabled())
				return;
	s->Disable();
	s->EnabledCallbacks(FALSE);
}

void	imotion_position::state_end(CPhysicsShell *s)
{
	inherited::state_end( s );
	s->ToAnimBonesPositions();
	s->EnabledCallbacks(TRUE);
}

void imotion_position::move_update(CPhysicsShell *s)
{
	s->Disable();
	s->ToAnimBonesPositions();
}

void imotion_position::collide(CPhysicsShell *s)
{
	depth = 0;
	s->CollideAll();
	if(depth > 0.05)
		flags.set(fl_switch_dm_toragdoll,TRUE);
}
////////////////////////////////////////////////////////////////////////////////////

void imotion_velocity::state_start(CPhysicsShell *s)
{
	inherited::state_start(s);
	if(!is_enabled())
				return;
	s->set_ApplyByGravity(false);
	//s->set_DynamicLimits(default_l_limit,default_w_limit * 5.f);
	//s->set_DynamicScales(1,1);
	//s->SetAirResistance(0,0);
}

void	imotion_velocity::state_end(CPhysicsShell *s)
{
	inherited::state_end( s );
	s->set_ApplyByGravity(true);
}

void imotion_velocity::collide(CPhysicsShell *s)
{

}

void imotion_velocity::move_update(CPhysicsShell *s)
{
		if(!s->AnimToVelocityState( Device.fTimeDelta, 2 * default_l_limit, 10.f * default_w_limit ))
			flags.set(fl_switch_dm_toragdoll,TRUE);
		Fmatrix sv;sv.set(s->mXFORM);
		s->InterpolateGlobalTransform(&s->mXFORM);
		s->mXFORM.set(sv);
}
