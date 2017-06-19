
#include "stdafx.h"

#include "phmovementcontrol.h"

#include "ExtendedGeom.h"
#include "MathUtils.h"
#include "Physics.h"
#include "Level.h"
#include "GameMtlLib.h"
#include "PhysicsShellHolder.h"

extern	class CPHWorld	*ph_world;
ObjectContactCallbackFun* saved_callback		=	0	;
static float max_depth							=	0.f	;

struct STestCallbackPars
{
	static float calback_friction_factor			;
	static float depth_to_use_force					;
	static float callback_force_factor				;
	static float depth_to_change_softness_pars		;
	static float callback_cfm_factor				;
	static float callback_erp_factor				;
	static float decrement_depth					;
	static float max_real_depth						;
};


float 	STestCallbackPars::calback_friction_factor					=	0.0f	;
float 	STestCallbackPars::depth_to_use_force						=	0.3f	;
float 	STestCallbackPars::callback_force_factor					=	10.f	;
float 	STestCallbackPars::depth_to_change_softness_pars			=	0.00f	;
float 	STestCallbackPars::callback_cfm_factor						=	world_cfm*0.00001f;
float 	STestCallbackPars::callback_erp_factor						=	1.f		;
float	STestCallbackPars::decrement_depth							=	0.f		;
float	STestCallbackPars::max_real_depth							=	0.2f	;
struct STestFootCallbackPars
{
	static float calback_friction_factor			;
	static float depth_to_use_force					;
	static float callback_force_factor				;
	static float depth_to_change_softness_pars		;
	static float callback_cfm_factor				;
	static float callback_erp_factor				;
	static float decrement_depth					;
	static float max_real_depth						;
};


float 	STestFootCallbackPars::calback_friction_factor					=	0.3f	;
float 	STestFootCallbackPars::depth_to_use_force						=	0.3f	;
float 	STestFootCallbackPars::callback_force_factor					=	10.f	;
float 	STestFootCallbackPars::depth_to_change_softness_pars			=	0.00f	;
float 	STestFootCallbackPars::callback_cfm_factor						=	world_cfm*0.00001f;
float 	STestFootCallbackPars::callback_erp_factor						=	1.f		;
float	STestFootCallbackPars::decrement_depth							=	0.05f	;
float	STestFootCallbackPars::max_real_depth							=	0.2f	;
template<class Pars>
void TTestDepthCallback (bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
{
	if(saved_callback)saved_callback(do_colide,bo1,c,material_1,material_2);

	if(do_colide&&!material_1->Flags.test(SGameMtl::flPassable) &&!material_2->Flags.test(SGameMtl::flPassable))
	{
		float& depth=c.geom.depth;
		float test_depth=depth-Pars::decrement_depth;
		save_max(max_depth,test_depth);
		c.surface.mu*=Pars::calback_friction_factor;
		if(test_depth>Pars::depth_to_use_force)
		{
			float force = Pars::callback_force_factor*ph_world->Gravity();
			dBodyID b1=dGeomGetBody(c.geom.g1);
			dBodyID b2=dGeomGetBody(c.geom.g2);
			if(b1)dBodyAddForce(b1,c.geom.normal[0]*force,c.geom.normal[1]*force,c.geom.normal[2]*force);
			if(b2)dBodyAddForce(b2,-c.geom.normal[0]*force,-c.geom.normal[1]*force,-c.geom.normal[2]*force);
			dxGeomUserData* ud1=retrieveGeomUserData(c.geom.g1);
			dxGeomUserData* ud2=retrieveGeomUserData(c.geom.g2);

			if(ud1)
			{
				CPhysicsShell* phsl=ud1->ph_ref_object->PPhysicsShell();
				if(phsl) phsl->Enable();
			}

			if(ud2)
			{
				CPhysicsShell* phsl=ud2->ph_ref_object->PPhysicsShell();
				if(phsl) phsl->Enable();
			}


			do_colide=false;
		}
		else if(test_depth>Pars::depth_to_change_softness_pars)
		{
			c.surface.soft_cfm=Pars::callback_cfm_factor;
			c.surface.soft_erp=Pars::callback_erp_factor;
		}
		limit_above(depth,Pars::max_real_depth);
	}

}

ObjectContactCallbackFun* TestDepthCallback		= &TTestDepthCallback<STestCallbackPars>;
ObjectContactCallbackFun* TestFootDepthCallback = &TTestDepthCallback<STestFootCallbackPars>;
///////////////////////////////////////////////////////////////////////////////////////
class CVelocityLimiter :
	public CPHUpdateObject
{
	dBodyID m_body;
public:
	float l_limit;
	float y_limit;
private:
	dVector3 m_safe_velocity;
	dVector3 m_safe_position;
public:
	CVelocityLimiter(dBodyID b,float l,float yl)
	{
		R_ASSERT(b);
		m_body=b;
		dVectorSet(m_safe_velocity,dBodyGetLinearVel(m_body));
		dVectorSet(m_safe_position,dBodyGetPosition(m_body));
		l_limit=l;
		y_limit=yl;
	}
	virtual ~CVelocityLimiter()
	{
		Deactivate();
		m_body =0;
	}


	bool VelocityLimit()
	{
		const float		*linear_velocity		=dBodyGetLinearVel(m_body);
		//limit velocity
		bool ret=false;
		if(dV_valid(linear_velocity))
		{
			dReal mag;
			Fvector vlinear_velocity;vlinear_velocity.set(cast_fv(linear_velocity));
			mag=_sqrt(linear_velocity[0]*linear_velocity[0]+linear_velocity[2]*linear_velocity[2]);//
			if(mag>l_limit)
			{
				dReal f=mag/l_limit;
				//dBodySetLinearVel(m_body,linear_velocity[0]/f,linear_velocity[1],linear_velocity[2]/f);///f
				vlinear_velocity.x/=f;vlinear_velocity.z/=f;
				ret=true;
			}
			mag=_abs(linear_velocity[1]);
			if(mag>y_limit)
			{
				vlinear_velocity.y=linear_velocity[1]/mag*y_limit;
				ret=true;
			}
			dBodySetLinearVel(m_body,vlinear_velocity.x,vlinear_velocity.y,vlinear_velocity.z);
			return ret;
		}
		else
		{
			dBodySetLinearVel(m_body,m_safe_velocity[0],m_safe_velocity[1],m_safe_velocity[2]);
			return true;
		}
	}
	virtual void PhDataUpdate(dReal step)
	{
		const float		*linear_velocity		=dBodyGetLinearVel(m_body);

		if(VelocityLimit())
		{
			dBodySetPosition(m_body,
				m_safe_position[0]+linear_velocity[0]*fixed_step,
				m_safe_position[1]+linear_velocity[1]*fixed_step,
				m_safe_position[2]+linear_velocity[2]*fixed_step);
		}

		if(!dV_valid(dBodyGetPosition(m_body)))
			dBodySetPosition(m_body,m_safe_position[0]-m_safe_velocity[0]*fixed_step,
			m_safe_position[1]-m_safe_velocity[1]*fixed_step,
			m_safe_position[2]-m_safe_velocity[2]*fixed_step);


		dVectorSet(m_safe_position,dBodyGetPosition(m_body));
		dVectorSet(m_safe_velocity,linear_velocity);
	}

	virtual void PhTune(dReal step)
	{

		VelocityLimit();
	}

};
////////////////////////////////////////////////////////////////////////////////////
class CGetContactForces:
	public CPHUpdateObject
{
	dBodyID m_body;
	float	m_max_force_self;
	float	m_max_torque_self;

	float	m_max_force_self_y;
	float	m_max_force_self_sd;

	float	m_max_force_others;
	float	m_max_torque_others;
public:
	CGetContactForces(dBodyID b)
	{
		R_ASSERT(b);
		m_body=b;
		InitValues();
	}
	float mf_slf(){return m_max_force_self;}
	float mf_othrs(){return m_max_force_others;}
	float mt_slf(){return m_max_torque_self;}
	float mt_othrs(){return m_max_torque_others;}

	float mf_slf_y(){return m_max_force_self_y;}
	float mf_slf_sd(){return m_max_force_self_sd;}
protected:
	virtual void PhTune(dReal step)
	{
		InitValues();
		int num=dBodyGetNumJoints(m_body);
		for(int i=0;i<num;++i)
		{
			dJointID joint=dBodyGetJoint(m_body,i);

			if(dJointGetType(joint)==dJointTypeContact)
			{
				dJointSetFeedback(joint,ContactFeedBacks.add());
			}
		}
	}

	virtual void PhDataUpdate(dReal step)
	{
		int num=dBodyGetNumJoints(m_body);
		for(int i=0;i<num;i++)
		{
			dJointID joint=dBodyGetJoint(m_body,i);
			if(dJointGetType(joint)==dJointTypeContact)
			{
				dJointFeedback* feedback=dJointGetFeedback(joint);
				R_ASSERT2(feedback,"Feedback was not set!!!");
				dxJoint* b_joint=(dxJoint*) joint;
				dBodyID other_body=b_joint->node[1].body;
				bool b_body_second=(b_joint->node[1].body==m_body);
				dReal* self_force=feedback->f1;
				dReal* self_torque=feedback->t1;
				dReal* othrers_force=feedback->f2;
				dReal* othrers_torque=feedback->t2;
				if(b_body_second)
				{
					other_body=b_joint->node[0].body;
					self_force=feedback->f2;
					self_torque=feedback->t2;
					othrers_force=feedback->f1;
					othrers_torque=feedback->t1;
				}

				save_max(m_max_force_self,_sqrt(dDOT( self_force,self_force)));
				save_max(m_max_torque_self,_sqrt(dDOT( self_torque,self_torque)));
				save_max(m_max_force_self_y,_abs(self_force[1]));
				save_max(m_max_force_self_sd,_sqrt(self_force[0]*self_force[0]+self_force[2]*self_force[2]));
				if(other_body)
				{
					dVector3 shoulder;dVectorSub(shoulder,dJointGetPositionContact(joint),dBodyGetPosition(other_body));
					dReal shoulder_lenght=_sqrt(dDOT(shoulder,shoulder));

					save_max(m_max_force_others,_sqrt(dDOT( othrers_force,othrers_force)));
					if(!fis_zero(shoulder_lenght)) 
						save_max(m_max_torque_others,_sqrt(dDOT( othrers_torque,othrers_torque))/shoulder_lenght);
				}
			}
		}
	}

private:
	void InitValues()
	{
		m_max_force_self				=			0.f;
		m_max_torque_self				=			0.f;
		m_max_force_others				=			0.f;
		m_max_torque_others				=			0.f;
		m_max_force_self_y				=			0.f;
		m_max_force_self_sd				=			0.f;
	}
};
/////////////////////////////////////////////////////////////////////////////////////

bool CPHMovementControl:: ActivateBoxDynamic(DWORD id,int num_it/*=8*/,int num_steps/*5*/,float resolve_depth/*=0.01f*/)
{
	bool  character_exist=CharacterExist();
	if(character_exist&&trying_times[id]!=u32(-1))
	{
		Fvector dif;dif.sub(trying_poses[id],cast_fv(dBodyGetPosition(m_character->get_body())));
		if(Device.dwTimeGlobal-trying_times[id]<500&&dif.magnitude()<0.05f)
																	return false;
	}
	if(!m_character||m_character->PhysicsRefObject()->PPhysicsShell())return false;
	DWORD old_id=BoxID();

	bool  character_disabled=character_exist && !m_character->IsEnabled();
	if(character_exist&&id==old_id)return true;

	if(!character_exist)
	{
		CreateCharacter();
	}

	//m_PhysicMovementControl->ActivateBox(id);
	m_character->CPHObject::activate();
	ph_world->Freeze();
	UnFreeze();

	saved_callback=ObjectContactCallback();
	SetOjectContactCallback(TestDepthCallback);
	SetFootCallBack(TestFootDepthCallback);
	max_depth=0.f;



	//////////////////////////////////pars///////////////////////////////////////////
//	int		num_it=8;
//	int		num_steps=5;
//	float	resolve_depth=0.01f;

	
	if(!character_exist)
	{
		num_it=20;
		num_steps=1;		
		resolve_depth=0.1f;
	}
	///////////////////////////////////////////////////////////////////////
	float	fnum_it=float(num_it);
	float	fnum_steps=float(num_steps);
	float	fnum_steps_r=1.f/fnum_steps;

	Fvector vel;
	Fvector pos;
	GetCharacterVelocity(vel);
	GetCharacterPosition(pos);
	//const Fbox& box =Box();
	float pass=	character_exist ? _abs(Box().getradius()-boxes[id].getradius()) : boxes[id].getradius();
	float max_vel=pass/2.f/fnum_it/fnum_steps/fixed_step;
	float max_a_vel=M_PI/8.f/fnum_it/fnum_steps/fixed_step;
	dBodySetForce(GetBody(),0.f,0.f,0.f);
	dBodySetLinearVel(GetBody(),0.f,0.f,0.f);
	Calculate(Fvector().set(0,0,0),Fvector().set(1,0,0),0,0,0,0);
	CVelocityLimiter vl(GetBody(),max_vel,max_vel);
	max_vel=1.f/fnum_it/fnum_steps/fixed_step;

	bool	ret=false;
	m_character->SwitchOFFInitContact();
	vl.Activate();
	vl.l_limit*=(fnum_it*fnum_steps/5.f);
	vl.y_limit=vl.l_limit;
////////////////////////////////////
	for(int m=0;30>m;++m)
	{
		Calculate(Fvector().set(0,0,0),Fvector().set(1,0,0),0,0,0,0);
		EnableCharacter();
		m_character->ApplyForce(0,ph_world->Gravity()*m_character->Mass(),0);
		max_depth=0.f;
		ph_world->Step();
		if(max_depth	<	resolve_depth) 
		{
			break;
		}	
		ph_world->CutVelocity(max_vel,max_a_vel);
	}
	vl.l_limit/=(fnum_it*fnum_steps/5.f);
	vl.y_limit=vl.l_limit;
/////////////////////////////////////

	for(int m=0;num_steps>m;++m)
	{
		float param =fnum_steps_r*(1+m);
		InterpolateBox(id,param);
		ret=false;
		for(int i=0;num_it>i;++i){
			max_depth=0.f;
			Calculate(Fvector().set(0,0,0),Fvector().set(1,0,0),0,0,0,0);
			EnableCharacter();
			m_character->ApplyForce(0,ph_world->Gravity()*m_character->Mass(),0);
			ph_world->Step();
			ph_world->CutVelocity(max_vel,max_a_vel);
			if(max_depth	<	resolve_depth) 
			{
				ret=true;
				break;
			}	
		}
		if(!ret) break;
	}
	m_character->SwitchInInitContact();
	vl.Deactivate();

	ph_world->UnFreeze();
	if(!ret)
	{	
		if(!character_exist)DestroyCharacter();
		else if(character_disabled)m_character->Disable();
		ActivateBox(old_id);
		SetVelocity(vel);
		dBodyID b=GetBody();
		if(b)
		{
			dMatrix3 R;
			dRSetIdentity (R);
			dBodySetAngularVel(b,0.f,0.f,0.f);
			dBodySetRotation(b,R);
		}
		SetPosition(pos);
		
		//Msg("can not activate!");
	}
	else
	{
		ActivateBox(id);
		//Msg("activate!");
	}

	SetOjectContactCallback(saved_callback);
	SetVelocity(vel);
	saved_callback=0;
	if(!ret&&character_exist)
	{
		trying_times[id]=Device.dwTimeGlobal;
		trying_poses[id].set(cast_fv(dBodyGetPosition(m_character->get_body())));
	}
	else
	{
		trying_times[id]=u32(-1);
	}
	return ret;
}
