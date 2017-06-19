#pragma once

#include "../BaseMonster/base_monster.h"
#include "../ai_monster_bones.h"
#include "../controlled_actor.h"
#include "../anim_triple.h"
#include "../../../script_export_space.h"
#include "bloodsucker_alien.h"

class CAI_Bloodsucker : public CBaseMonster, 
						public CControlledActor {

	typedef		CBaseMonster	inherited;
	
public:
							CAI_Bloodsucker	();
	virtual					~CAI_Bloodsucker();	

	virtual void			reinit					();
	virtual	void			reload					(LPCSTR section);

	virtual void			UpdateCL				();
	virtual void			shedule_Update			(u32 dt);
	virtual void			Die						(CObject* who);
	virtual BOOL			net_Spawn				(CSE_Abstract* DC);
	virtual	void			Load					(LPCSTR section);

	virtual	void			CheckSpecParams			(u32 spec_params);
	virtual bool			ability_invisibility	() {return true;}
	virtual bool			ability_pitch_correction() {return false;}
	virtual	void			post_fsm_update			();
	
	virtual bool			use_center_to_aim		() const {return true;}
	virtual bool			check_start_conditions	(ControlCom::EControlType);
	virtual void			HitEntity				(const CEntity *pEntity, float fDamage, float impulse, Fvector &dir);
	
	//--------------------------------------------------------------------
	// Utils
	//--------------------------------------------------------------------
			void			move_actor_cam			();

	//--------------------------------------------------------------------
	// Bones
	//--------------------------------------------------------------------
private:
	static	void			BoneCallback			(CBoneInstance *B);
			void			vfAssignBones			();
			void			LookDirection			(Fvector to_dir, float bone_turn_speed);


	bonesManipulation		Bones;

	CBoneInstance			*bone_spine;
	CBoneInstance			*bone_head;

	//--------------------------------------------------------------------
	// Invisibility
	//--------------------------------------------------------------------
private:
	SMotionVel				invisible_vel;
	LPCSTR					invisible_particle_name;

public:
			void			start_invisible_predator	();
			void			stop_invisible_predator		();

	//--------------------------------------------------------------------
	// Vampire
	//--------------------------------------------------------------------
public:

	u32						m_vampire_min_delay;
	SAnimationTripleData	anim_triple_vampire;

	SPPInfo					pp_vampire_effector;

			
			void			ActivateVampireEffector	();
	IC		bool			WantVampire				() {return (fsimilar(m_vampire_want_value,1.f) == TRUE);}
	IC		void			SatisfyVampire			() {m_vampire_want_value = 0.f;}

private:
	float					m_vampire_want_value;
	float					m_vampire_want_speed;		// load from ltx
	float					m_vampire_wound;
	
			void			LoadVampirePPEffector	(LPCSTR section);	

	//--------------------------------------------------------------------
	// Alien
	//--------------------------------------------------------------------
public:
	CBloodsuckerAlien		m_alien_control;
	u32						m_time_lunge;

			void			set_alien_control		(bool val);


	//--------------------------------------------------------------------
	// Predator
	//--------------------------------------------------------------------
public:
	shared_str				m_visual_default;
	LPCSTR					m_visual_predator;
	bool					m_predator;
			
			void			predator_start			();
			void			predator_stop			();
			void			predator_freeze			();
			void			predator_unfreeze		();
	
	//--------------------------------------------------------------------
	// Sounds
	//--------------------------------------------------------------------
public:

	enum EBloodsuckerSounds {
		eAdditionalSounds		= MonsterSound::eMonsterSoundCustom,

		eVampireGrasp			= eAdditionalSounds | 0,
		eVampireSucking			= eAdditionalSounds | 1,
		eVampireHit				= eAdditionalSounds | 2,
		eVampireStartHunt		= eAdditionalSounds | 3,

		eGrowl					= eAdditionalSounds | 5,

		eChangeVisibility		= eAdditionalSounds | 6,
		
		eAlien					= eAdditionalSounds | 7,
	};

	//--------------------------------------------------------------------

public:
			void	set_manual_control	(bool value) {}
			void	manual_activate		();
			void	manual_deactivate	();

#ifdef DEBUG
	virtual CBaseMonster::SDebugInfo show_debug_info();

#ifdef _DEBUG
			void debug_on_key						(int key);
#endif

#endif

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CAI_Bloodsucker)
#undef script_type_list
#define script_type_list save_type_list(CAI_Bloodsucker)