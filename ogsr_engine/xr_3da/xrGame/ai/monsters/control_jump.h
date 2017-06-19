#pragma once
#include "control_combase.h"
#include "../../../SkeletonAnimated.h"

struct SControlJumpData : public ControlCom::IComData {
	CObject					*target_object;
 	Fvector					target_position;
	float					force_factor;

	enum EFlags {	
		eEnablePredictPosition		= u32(1) << 0,
		ePrepareSkip				= u32(1) << 1,	// do not use prepare state
		ePrepareInMove				= u32(1) << 2,
		eGlideOnPrepareFailed		= u32(1) << 3,  // if not set then cannot start jump
		eGlidePlayAnimOnce			= u32(1) << 4,
		eGroundSkip					= u32(1) << 5,
	};
	
	flags32					flags;

	struct	_prepare{
		MotionID	motion;
	} state_prepare;

	struct	_prepare_in_move{
		MotionID	motion;
		u32			velocity_mask;
	} state_prepare_in_move;


	struct	_glide{
		MotionID	motion;
	} state_glide;

	struct	_ground{
		MotionID	motion;
		u32			velocity_mask;
	} state_ground;
};

class CControlJump : public CControl_ComCustom<SControlJumpData> {
	typedef CControl_ComCustom<SControlJumpData> inherited;

	enum EStateAnimJump {
		eStatePrepare,
		eStatePrepareInMove,
		eStateGlide,
		eStateGround,
		eStateNone
	};


	// loadable parameters
	u32				m_delay_after_jump;
	float			m_jump_factor;
	float			m_trace_ground_range;
	float			m_hit_trace_range;
	float			m_build_line_distance;
	float			m_min_distance;
	float			m_max_distance;
	float			m_max_angle;
	float			m_max_height;

	// run-time params
	u32				m_time_next_allowed;
	u32				m_time_started;			// time jump started
	float			m_jump_time;			// physical-counted time of jump
	float			m_blend_speed;			// current anim blend speed
	Fvector			m_target_position;		// save target position for internal needs


	// state flags
	bool			m_object_hitted;
	bool			m_velocity_bounced;

	// animation
	EStateAnimJump	m_anim_state_prev;
	EStateAnimJump	m_anim_state_current;

public:
	virtual void	load					(LPCSTR section);
	virtual void	reinit					();
	virtual bool	check_start_conditions	();
	virtual void	activate				();
	virtual void	on_release				();
	virtual void	on_event				(ControlCom::EEventType, ControlCom::IEventData*);


	// process jump
	virtual void	update_frame			();

	// check for distance and angle difference
	virtual	bool	can_jump				(CObject *target);

	// stop/break jump and all of jumping states
	virtual void	stop					();

SControlJumpData	&setup_data				() {return m_data;}

private:	
			// service routines		
			// build path after jump 
			void	grounding			();
			// get target position according to object center point
			Fvector get_target			(CObject *obj);
			// check for hit object
			void	hit_test			();

			// check current jump state		
			bool	is_on_the_ground	();

			// position prediction
			Fvector	predict_position	(CObject *obj, const Fvector &pos);

			void	start_jump			(const Fvector &point);

			// animation control method
			void	select_next_anim_state	();

	IC		bool	is_flag					(SControlJumpData::EFlags flag);
};


IC bool CControlJump::is_flag(SControlJumpData::EFlags flag) 
{
	return (m_data.flags.is(flag) == TRUE);
}
