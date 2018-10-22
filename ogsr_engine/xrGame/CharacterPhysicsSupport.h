
#ifndef CHARACTER_PHYSICS_SUPPORT
#define CHARACTER_PHYSICS_SUPPORT

#include "alife_space.h"
#include "PHSkeleton.h"
#include "Entity_Alive.h"
#include "PHSoundPlayer.h"
#include "Phdestroyable.h"
#include "character_hit_animations.h"


class CPhysicsShell;
class CPHMovementControl;
class CIKLimbsController;
class interactive_motion;



class CCharacterPhysicsSupport :
public CPHSkeleton,
public CPHDestroyable
{
public:
enum EType
{
	etActor,
	etStalker,
	etBitting
};

enum EState
{
	esDead,
	esAlive,
	esRemoved
};

private:
	EType								m_eType;
	EState								m_eState;
	Flags8								m_flags;
	enum Fags 
	{
		fl_death_anim_on			=1<<0,
		fl_skeleton_in_shell		=1<<1,
		fl_specific_bonce_demager	=1<<2,
		fl_block_hit				=1<<3,
	};

	struct	animation_movement_state{ 
		bool		active;
		bool		character_exist;
		void		init( ){ active	=  false ; character_exist =  false ; }
		animation_movement_state( )		{ init( ); }
	}									anim_mov_state;

	CEntityAlive						&m_EntityAlife																																		;
	Fmatrix								&mXFORM																																				;
	CPhysicsShell						*&m_pPhysicsShell																																	;
	CPhysicsShell						*m_physics_skeleton																																	;
	CPHMovementControl					*m_PhysicMovementControl																															;
	CPHSoundPlayer						m_ph_sound_player																																	;
	CIKLimbsController					*m_ik_controller																																	;
	SCollisionHitCallback				*m_collision_hit_callback;
	character_hit_animation_controller	m_hit_animations;

	interactive_motion					*m_interactive_motion;
//skeleton modell(!share?)
	float								skel_airr_lin_factor																																;
	float								skel_airr_ang_factor																																;
	float								hinge_force_factor1																																	;
	float								skel_fatal_impulse_factor																															;
	float								skel_ddelay																																			;
	float								skel_remain_time																																	;	
/////////////////////////////////////////////////
	//bool								b_death_anim_on																																		;
	//bool								b_skeleton_in_shell																																	;
///////////////////////////////////////////////////////////////////////////
	float								m_shot_up_factor																																	;
	float								m_after_death_velocity_factor																														;
	float								m_BonceDamageFactor																																	;
	//gray_wolf>Переменные для поддержки изменяющегося трения у персонажей во время смерти
	float								skeleton_skin_ddelay;
	float								skeleton_skin_remain_time;
	float								skeleton_skin_friction_start;
	float								skeleton_skin_friction_end;
	float								skeleton_skin_ddelay_after_wound;
	float								skeleton_skin_remain_time_after_wound;
	bool								m_was_wounded;
	float								m_Pred_Time;//Для вычисления дельта времени между пересчётами сопротивления в джоинтах и коэффициента NPC
	float								m_time_delta;
	float								pelvis_factor_low_pose_detect;
	BOOL								character_have_wounded_state;
	//gray_wolf<
public:
	//gray_wolf>
	float								m_curr_skin_friction_in_death;
	//gray_wolf<
EType Type()
	{
		return m_eType;
	}
EState STate()
	{
		return m_eState;
	}
void	SetState(EState astate)
	{
		m_eState=astate;
	}
IC	bool isDead()
	{
		return m_eState==esDead;
	}
IC	bool isAlive()
	{
		return !m_pPhysicsShell;
	}
protected:
virtual void							SpawnInitPhysics				(CSE_Abstract	*D)																									;
virtual CPhysicsShellHolder*			PPhysicsShellHolder				()	{return m_EntityAlife.PhysicsShellHolder();}	

virtual bool							CanRemoveObject					();
public:
IC		CPHMovementControl				*movement						()	{return m_PhysicMovementControl;}
IC		CPHSoundPlayer					*ph_sound_player				()	{return &m_ph_sound_player;}
IC		CIKLimbsController				*ik_controller					()	{return	m_ik_controller;}
		void							SetRemoved						();
		bool							IsRemoved						(){return m_eState==esRemoved;}
		bool							IsSpecificDamager				()																{return !!m_flags.test(fl_specific_bonce_demager)	;}
		float							BonceDamageFactor				(){return m_BonceDamageFactor;}
		void							set_movement_position			( const Fvector &pos );
//////////////////base hierarchi methods///////////////////////////////////////////////////
		void							CreateCharacter					();
		void 							in_UpdateCL()																																		;
		void 							in_shedule_Update				( u32 DT )																											;
		void 							in_NetSpawn						(CSE_Abstract* e)																									;
		void 							in_NetDestroy					()																													;
		void							in_NetRelcase					(CObject* O)																										;
		void 							in_Init							()																													;
		void 							in_Load							(LPCSTR section)																									;
		void 							in_Hit							(float P,Fvector &dir, CObject *who, s16 element,Fvector p_in_object_space, float impulse,ALife::EHitType hit_type ,bool is_killing=false);
		void							in_NetSave						(NET_Packet& P)																										;
		void							in_ChangeVisual					();
		void							on_create_anim_mov_ctrl			();
		void							on_destroy_anim_mov_ctrl		();
		void							PHGetLinearVell					(Fvector& velocity);
		SCollisionHitCallback*			get_collision_hit_callback		();
		bool							set_collision_hit_callback		(SCollisionHitCallback* cc);
/////////////////////////////////////////////////////////////////
		CCharacterPhysicsSupport& operator = (CCharacterPhysicsSupport& /**asup/**/){R_ASSERT2(false,"Can not assign it");}
								CCharacterPhysicsSupport				(EType atype,CEntityAlive* aentity)																					;
virtual							~CCharacterPhysicsSupport				()																													;
private:
		void 							CreateSkeleton					(CPhysicsShell* &pShell)																							;
		void 							CreateSkeleton					();
		void 							ActivateShell					(CObject* who)																										;
		void							KillHit							(CObject* who, ALife::EHitType hit_type, float &impulse)																										;
static	void							DeathAnimCallback				(CBlend *B)																											;
		void							CreateIKController				()																													;
		void							DestroyIKController				()																													;
		void							CollisionCorrectObjPos			(const Fvector& start_from,bool character_create=false);
		void							FlyTo							(const	Fvector &disp);
		void							TestForWounded					();
IC		void							UpdateFrictionAndJointResistanse();
IC		void							UpdateDeathAnims				();
IC		void							CalculateTimeDelta				();
IC		bool							DoCharacterShellCollide			();
};
#endif  //CHARACTER_PHYSICS_SUPPORT