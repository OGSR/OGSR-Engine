#include "stdafx.h"
#include "chimera.h"
#include "chimera_state_manager.h"
#include "../../../../skeletonanimated.h"
#include "../../../detail_path_manager.h"
#include "../monster_velocity_space.h"
#include "../../../level.h"
#include "../../../PhysicsShell.h"
#include "../../../sound_player.h"
#include "../control_animation_base.h"
#include "../control_movement_base.h"
#include "../control_path_builder_base.h"


CChimera::CChimera()
{
	StateMan = xr_new<CStateManagerChimera>	(this);
	com_man().add_ability(ControlCom::eControlJump);
}

CChimera::~CChimera()
{
	xr_delete		(StateMan);
}

void CChimera::Load(LPCSTR section)
{
	inherited::Load			(section);

	anim().accel_load			(section);
	anim().accel_chain_add		(eAnimWalkFwd,		eAnimRun);
	anim().accel_chain_add		(eAnimWalkDamaged,	eAnimRunDamaged);

	anim().AddReplacedAnim(&m_bDamaged, eAnimRun,		eAnimRunDamaged);
	anim().AddReplacedAnim(&m_bDamaged, eAnimWalkFwd,	eAnimWalkDamaged);

	SVelocityParam &velocity_none		= move().get_velocity(MonsterMovement::eVelocityParameterIdle);	
	SVelocityParam &velocity_turn		= move().get_velocity(MonsterMovement::eVelocityParameterStand);
	SVelocityParam &velocity_walk		= move().get_velocity(MonsterMovement::eVelocityParameterWalkNormal);
	SVelocityParam &velocity_run		= move().get_velocity(MonsterMovement::eVelocityParameterRunNormal);
	SVelocityParam &velocity_walk_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterWalkDamaged);
	SVelocityParam &velocity_run_dmg	= move().get_velocity(MonsterMovement::eVelocityParameterRunDamaged);
	SVelocityParam &velocity_steal		= move().get_velocity(MonsterMovement::eVelocityParameterSteal);
	SVelocityParam &velocity_drag		= move().get_velocity(MonsterMovement::eVelocityParameterDrag);


	anim().AddAnim(eAnimStandIdle,		"stand_idle_",			-1, &velocity_none,				PS_STAND);
	anim().AddAnim(eAnimStandTurnLeft,	"stand_turn_ls_",		-1, &velocity_turn,		PS_STAND);
	anim().AddAnim(eAnimStandTurnRight,	"stand_turn_rs_",		-1, &velocity_turn,		PS_STAND);

	anim().AddAnim(eAnimLieIdle,			"lie_sleep_",			-1, &velocity_none,				PS_LIE);
	anim().AddAnim(eAnimSleep,			"lie_sleep_",			-1, &velocity_none,				PS_LIE);

	anim().AddAnim(eAnimWalkFwd,			"stand_walk_fwd_",		-1, &velocity_walk,	PS_STAND);
	anim().AddAnim(eAnimWalkDamaged,		"stand_walk_fwd_dmg_",	-1, &velocity_walk_dmg,	PS_STAND);
	anim().AddAnim(eAnimRun,				"stand_run_fwd_",		-1,	&velocity_run,		PS_STAND);
	anim().AddAnim(eAnimRunDamaged,		"stand_run_dmg_",		-1,	&velocity_run_dmg,	PS_STAND);
	anim().AddAnim(eAnimCheckCorpse,		"stand_check_corpse_",	-1,	&velocity_none,				PS_STAND);
	anim().AddAnim(eAnimEat,				"stand_eat_",			-1, &velocity_none,				PS_STAND);
	anim().AddAnim(eAnimAttack,			"stand_attack_",		-1, &velocity_turn,		PS_STAND);

	anim().AddAnim(eAnimDragCorpse,		"stand_drag_",			-1, &velocity_drag,				PS_STAND);
	anim().AddAnim(eAnimLookAround,		"stand_idle_",			 2, &velocity_none,				PS_STAND);
	anim().AddAnim(eAnimSteal,			"stand_steal_",			-1, &velocity_steal,			PS_STAND);
	anim().AddAnim(eAnimDie,				"stand_idle_",			-1, &velocity_none,				PS_STAND);
	anim().AddAnim(eAnimThreaten,		"stand_threaten_",		-1, &velocity_none,				PS_STAND);

	anim().AddAnim(eAnimAttackRun,		"stand_run_attack_",	-1, &velocity_run,		PS_STAND);

	//////////////////////////////////////////////////////////////////////////

	anim().AddAnim(eAnimUpperStandIdle,		"stand_up_idle_",		-1, &velocity_none,			PS_STAND_UPPER);
	anim().AddAnim(eAnimUpperStandTurnLeft,	"stand_up_turn_ls_",	-1, &velocity_turn,	PS_STAND_UPPER);
	anim().AddAnim(eAnimUpperStandTurnRight,	"stand_up_turn_rs_",	-1, &velocity_turn,	PS_STAND_UPPER);

	anim().AddAnim(eAnimStandToUpperStand,	"stand_upper_",			-1, &velocity_none,			PS_STAND);
	anim().AddAnim(eAnimUppperStandToStand,	"stand_up_to_down_",	-1, &velocity_none,			PS_STAND_UPPER);

	anim().AddAnim(eAnimUpperWalkFwd,		"stand_up_walk_fwd_",	-1, &m_fsVelocityWalkUpper,							PS_STAND_UPPER);
	anim().AddAnim(eAnimUpperThreaten,		"stand_up_threaten_",	-1, &velocity_none,			PS_STAND_UPPER);
	anim().AddAnim(eAnimUpperAttack,			"stand_up_attack_",		-1, &velocity_turn,	PS_STAND_UPPER);

	//////////////////////////////////////////////////////////////////////////
	// define transitions
	anim().AddTransition(PS_STAND,			PS_STAND_UPPER,		eAnimStandToUpperStand,		false);
	anim().AddTransition(PS_STAND_UPPER,		PS_STAND,			eAnimUppperStandToStand,	false);

	// link action
	anim().LinkAction(ACT_STAND_IDLE,	eAnimStandIdle);
	anim().LinkAction(ACT_SIT_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_LIE_IDLE,		eAnimLieIdle);
	anim().LinkAction(ACT_WALK_FWD,		eAnimWalkFwd);
	anim().LinkAction(ACT_WALK_BKWD,		eAnimDragCorpse);
	anim().LinkAction(ACT_RUN,			eAnimRun);
	anim().LinkAction(ACT_EAT,			eAnimEat);
	anim().LinkAction(ACT_SLEEP,			eAnimSleep);
	anim().LinkAction(ACT_REST,			eAnimLieIdle);
	anim().LinkAction(ACT_DRAG,			eAnimDragCorpse);
	anim().LinkAction(ACT_ATTACK,		eAnimAttack);
	anim().LinkAction(ACT_STEAL,			eAnimSteal);
	anim().LinkAction(ACT_LOOK_AROUND,	eAnimLookAround);

#ifdef DEBUG	
	anim().accel_chain_test		();
#endif

	//*****************************************************************************

	m_fsVelocityWalkUpper.Load	(section, "Velocity_Walk_Upper");
}

void CChimera::reinit()
{
	inherited::reinit();
	b_upper_state					= false;

	movement().detail().add_velocity(MonsterMovement::eChimeraVelocityParameterUpperWalkFwd,	CDetailPathManager::STravelParams(m_fsVelocityWalkUpper.velocity.linear,	m_fsVelocityWalkUpper.velocity.angular_path, m_fsVelocityWalkUpper.velocity.angular_real));
	move().load_velocity(*cNameSect(), "Velocity_JumpGround",MonsterMovement::eChimeraVelocityParameterJumpGround);

	com_man().load_jump_data("jump_attack_0",0, "jump_attack_1", "jump_attack_2", u32(-1), MonsterMovement::eChimeraVelocityParameterJumpGround,0);
}

void CChimera::SetTurnAnimation(bool turn_left)
{
	if (b_upper_state) 
		(turn_left) ? anim().SetCurAnim(eAnimUpperStandTurnLeft) : anim().SetCurAnim(eAnimUpperStandTurnRight);
	else 
		(turn_left) ? anim().SetCurAnim(eAnimStandTurnLeft)		: anim().SetCurAnim(eAnimStandTurnRight);
}

void CChimera::CheckSpecParams(u32 spec_params)
{
	if ((spec_params & ASP_THREATEN) == ASP_THREATEN) {
		if (b_upper_state)
			anim().SetCurAnim(eAnimUpperThreaten);
		else 
			anim().SetCurAnim(eAnimThreaten);
	}

	if ((spec_params & ASP_ATTACK_RUN) == ASP_ATTACK_RUN) {
		anim().SetCurAnim(eAnimAttackRun);
	}

	if (b_upper_state) {
		switch (anim().GetCurAnim()) {
			case eAnimAttack:			anim().SetCurAnim(eAnimUpperAttack);			break;
			case eAnimRun:
			case eAnimWalkFwd:			anim().SetCurAnim(eAnimUpperWalkFwd);		break;
			case eAnimStandTurnLeft:	anim().SetCurAnim(eAnimUpperStandTurnLeft);	break;
			case eAnimStandTurnRight:	anim().SetCurAnim(eAnimUpperStandTurnRight); break;
			case eAnimThreaten:			anim().SetCurAnim(eAnimUpperThreaten);		break;
			case eAnimStandIdle:		anim().SetCurAnim(eAnimUpperStandIdle);		break;
		}
	}
}

EAction CChimera::CustomVelocityIndex2Action(u32 velocity_index) 
{
	switch (velocity_index) {
		case MonsterMovement::eChimeraVelocityParameterUpperWalkFwd: return ACT_WALK_FWD;
	}
	
	return ACT_STAND_IDLE;
}

void CChimera::TranslateActionToPathParams()
{
	bool bEnablePath = true;
	u32 vel_mask = 0;
	u32 des_mask = 0;

	switch (anim().m_tAction) {
	case ACT_STAND_IDLE: 
	case ACT_SIT_IDLE:	 
	case ACT_LIE_IDLE:
	case ACT_EAT:
	case ACT_SLEEP:
	case ACT_REST:
	case ACT_LOOK_AROUND:
	case ACT_ATTACK:
		bEnablePath = false;
		break;
	case ACT_WALK_FWD:
		if (b_upper_state) {
			vel_mask = MonsterMovement::eChimeraVelocityParamsUpperWalkFwd;
			des_mask = MonsterMovement::eChimeraVelocityParameterUpperWalkFwd;
		} else {
			if (m_bDamaged) {
				vel_mask = MonsterMovement::eVelocityParamsWalkDamaged;
				des_mask = MonsterMovement::eVelocityParameterWalkDamaged;
			} else {
				vel_mask = MonsterMovement::eVelocityParamsWalk;
				des_mask = MonsterMovement::eVelocityParameterWalkNormal;
			}
		}
		break;
	case ACT_WALK_BKWD:
		break;
	case ACT_RUN:
		if (b_upper_state) {
			vel_mask = MonsterMovement::eChimeraVelocityParamsUpperWalkFwd;
			des_mask = MonsterMovement::eChimeraVelocityParameterUpperWalkFwd;
		} else {
			if (m_bDamaged) {
				vel_mask = MonsterMovement::eVelocityParamsRunDamaged;
				des_mask = MonsterMovement::eVelocityParameterRunDamaged;
			} else {
				vel_mask = MonsterMovement::eVelocityParamsRun;
				des_mask = MonsterMovement::eVelocityParameterRunNormal;
			}
		}
		break;
	case ACT_DRAG:
		vel_mask = MonsterMovement::eVelocityParamsDrag;
		des_mask = MonsterMovement::eVelocityParameterDrag;

		anim().SetSpecParams(ASP_MOVE_BKWD);

		break;
	case ACT_STEAL:
		vel_mask = MonsterMovement::eVelocityParamsSteal;
		des_mask = MonsterMovement::eVelocityParameterSteal;
		break;
	}

	if (m_force_real_speed) vel_mask = des_mask;

	if (bEnablePath) {
		path().set_velocity_mask	(vel_mask);
		path().set_desirable_mask	(des_mask);
		path().enable_path			();	
	} else {
		path().disable_path			();
	}
}

void CChimera::HitEntityInJump(const CEntity *pEntity)
{
	SAAParam &params	= anim().AA_GetParams("jump_attack_1");
	HitEntity			(pEntity, params.hit_power, params.impulse, params.impulse_dir);
}

void CChimera::UpdateCL()
{
	inherited::UpdateCL				();
}
