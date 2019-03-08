#pragma once

#include "GameObject.h"
#include "ParticlesPlayer.h"

class CPHDestroyable;
class CPHCollisionDamageReceiver;
class CPHSoundPlayer;
class IDamageSource;
class CPHSkeleton;
class CCharacterPhysicsSupport;
class ICollisionDamageInfo;
class CIKLimbsController;
class CPHCapture;

class ICollisionHitCallback
{
public:
	virtual	void call(CPhysicsShellHolder* obj, float min_cs, float max_cs, float &cs, float &hl, ICollisionDamageInfo* di) = 0;
	virtual	~ICollisionHitCallback() {}
};
class CPhysicsShellHolder:  public CGameObject,
							public CParticlesPlayer
	
{
	bool				b_sheduled;
public:
	void	SheduleRegister		(){if(!IsSheduled())shedule_register();b_sheduled=true;}
	void	SheduleUnregister	(){if(IsSheduled())shedule_unregister();b_sheduled=false;}
IC	bool	IsSheduled			(){return b_sheduled;}	
public:

	typedef CGameObject inherited;
	

	CPhysicsShell			*m_pPhysicsShell;


	CPhysicsShellHolder							();



	IC CPhysicsShell	*&PPhysicsShell				()		
	{
		return m_pPhysicsShell;
	}

	IC CPhysicsShellHolder*	PhysicsShellHolder	()
	{
		return this;
	}
	virtual CPHDestroyable				*ph_destroyable				()							{return NULL;}
	virtual CPHCollisionDamageReceiver	*PHCollisionDamageReceiver	()							{return NULL;}
	virtual CPHSkeleton					*PHSkeleton					()							{return NULL;}
	virtual CPhysicsShellHolder			*cast_physics_shell_holder	()							{return this;}
	virtual CParticlesPlayer			*cast_particles_player		()							{return this;}
	virtual IDamageSource				*cast_IDamageSource			()							{return NULL;}
	virtual CPHSoundPlayer				*ph_sound_player			()  						{return NULL;}
	virtual	CCharacterPhysicsSupport	*character_physics_support	()							{return NULL;}
	virtual	CCharacterPhysicsSupport	*character_physics_support	() const					{return NULL;}
	virtual	CIKLimbsController			*character_ik_controller	()							{return NULL;}
	virtual ICollisionHitCallback		*get_collision_hit_callback ()							{return NULL;}
	virtual void						set_collision_hit_callback	(ICollisionHitCallback *cc)	{;}
	virtual void						enable_notificate			()							{;}
public:

	virtual void			PHGetLinearVell		(Fvector& velocity);
	virtual void			PHSetLinearVell		(Fvector& velocity);
	virtual void			PHSetMaterial		(LPCSTR m);
	virtual void			PHSetMaterial		(u16 m);
			void			PHSaveState			(NET_Packet &P);
			void			PHLoadState			(IReader &P);
	virtual f32				GetMass				();
	virtual	void			PHHit				(SHit& H);
	virtual	void			Hit					(SHit* pHDS);
///////////////////////////////////////////////////////////////////////
	virtual u16				PHGetSyncItemsNumber();
	virtual CPHSynchronize*	PHGetSyncItem		(u16 item);
	virtual void			PHUnFreeze			();
	virtual void			PHFreeze			();
	virtual float			EffectiveGravity	();
///////////////////////////////////////////////////////////////
	virtual void			create_physic_shell		();
	virtual void			activate_physic_shell	();
	virtual void			setup_physic_shell		();
	virtual void			deactivate_physics_shell ();

	virtual void			net_Destroy			();
	virtual BOOL			net_Spawn			(CSE_Abstract*	DC);
	virtual void			save				(NET_Packet &output_packet);
	virtual void			load				(IReader &input_packet);
			void			init				();

	virtual void			OnChangeVisual		();
	//для наследования CParticlesPlayer
	virtual void			UpdateCL			();
			void			correct_spawn_pos	();

public:
	virtual bool			register_schedule	() const;
	bool ActorCanCapture() const;
	bool hasFixedBones() const;

public://IPhysicsShellHolder
	CPHCapture*				_BCL					PHCapture							()						;

private:
	Fvector					m_overriden_activation_speed;
	bool					m_activation_speed_is_overriden;

public:
	virtual bool			ActivationSpeedOverriden	(Fvector& dest, bool clear_override);
	virtual void			SetActivationSpeedOverride	(Fvector const& speed);
};
