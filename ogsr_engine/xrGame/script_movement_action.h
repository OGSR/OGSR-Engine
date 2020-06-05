////////////////////////////////////////////////////////////////////////////
//	Module 		: script_movement_action.h
//	Created 	: 30.09.2003
//  Modified 	: 29.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Script movement action class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_abstract_action.h"
#include "script_export_space.h"

namespace DetailPathManager {
	enum EDetailPathType;
};

namespace MonsterSpace {
	enum EBodyState;
	enum EMovementType;
	enum EScriptMonsterMoveAction;
	enum EScriptMonsterSpeedParam;
};

namespace PatrolPathManager {
	enum EPatrolStartType;
	enum EPatrolRouteType;
};

class CPatrolPath;
class CScriptGameObject;
class CPatrolPathParams;

class CScriptMovementAction : public CScriptAbstractAction {
public:
	enum EGoalType {
		eGoalTypeObject = u32(0),
		eGoalTypePatrolPath,
		eGoalTypePathPosition,
		eGoalTypeNoPathPosition,
		eGoalTypePathNodePosition,
		eGoalTypeInput,
		eGoalTypeJumpToPosition,
		eGoalTypeFollowLeader,
		eGoalTypeDummy = u32(-1),
	};

	enum EInputKeys {
		eInputKeyNone		= u32(1) << 0,
		eInputKeyForward	= u32(1) << 1,
		eInputKeyBack		= u32(1) << 2,
		eInputKeyLeft		= u32(1) << 3,
		eInputKeyRight		= u32(1) << 4,
		eInputKeyShiftUp	= u32(1) << 5,
		eInputKeyShiftDown	= u32(1) << 6,
		eInputKeyBreaks		= u32(1) << 7,
		eInputKeyEngineOn	= u32(1) << 8,
		eInputKeyEngineOff	= u32(1) << 9,
		eInputKeyDummy		= u32(1) << 10,
	};

public:
	shared_str									m_path_name;
	MonsterSpace::EBodyState					m_tBodyState;
	MonsterSpace::EMovementType					m_tMovementType;
	DetailPathManager::EDetailPathType			m_tPathType;
	CObject										*m_tpObjectToGo;
	const CPatrolPath							*m_path;
	PatrolPathManager::EPatrolStartType			m_tPatrolPathStart;
	PatrolPathManager::EPatrolRouteType			m_tPatrolPathStop;
	Fvector										m_tDestinationPosition;
	u32											m_tNodeID;
	EGoalType									m_tGoalType;
	float										m_fSpeed;
	bool										m_bRandom;
	EInputKeys									m_tInputKeys;
	MonsterSpace::EScriptMonsterMoveAction		m_tMoveAction;
	MonsterSpace::EScriptMonsterSpeedParam		m_tSpeedParam;
	u32											m_previous_patrol_point;
	float										m_fDistToEnd;


public:
					CScriptMovementAction	();
	IC				CScriptMovementAction	(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, CScriptGameObject *tpObjectToGo, float fSpeed = 0.f);
					CScriptMovementAction	(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const CPatrolPathParams &tPatrolPathParams, float fSpeed = 0.f);
	IC				CScriptMovementAction	(MonsterSpace::EBodyState tBodyState, MonsterSpace::EMovementType tMovementType, DetailPathManager::EDetailPathType tPathType, const Fvector &tPosition, float fSpeed = 0.f);
					CScriptMovementAction	(const Fvector &tPosition, float fSpeed);
	IC				CScriptMovementAction	(const EInputKeys tInputKeys, float fSpeed = 0.f);
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------						
	// Monsters
	// --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector &tPosition, float	dist_to_end						= -1.f);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, float dist_to_end	= -1.f);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject *tpObjectToGo, float dist_to_end		= -1.f);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, u32 node_id, Fvector &tPosition, float dist_to_end		= -1.f);
	IC				CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, Fvector &tPosition, float	dist_to_end,							MonsterSpace::EScriptMonsterSpeedParam speed_param);
					CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CPatrolPathParams &tPatrolPathParams, float dist_to_end,			MonsterSpace::EScriptMonsterSpeedParam speed_param);
	IC				CScriptMovementAction	(MonsterSpace::EScriptMonsterMoveAction tAct, CScriptGameObject *tpObjectToGo, float dist_to_end,				MonsterSpace::EScriptMonsterSpeedParam speed_param);
	virtual			~CScriptMovementAction	();
	IC		void	SetBodyState			(const MonsterSpace::EBodyState tBodyState);
	IC		void	SetMovementType			(const MonsterSpace::EMovementType tMovementType);
	IC		void	SetPathType				(const DetailPathManager::EDetailPathType tPathType);
			void	SetObjectToGo			(CScriptGameObject *tpObjectToGo);
	IC		void	SetPatrolPath			(const CPatrolPath *path, shared_str path_name);
	IC		void	SetPosition				(const Fvector &tPosition);
	IC		void	SetSpeed				(float fSpeed);
	IC		void	SetPatrolStart			(PatrolPathManager::EPatrolStartType tPatrolPathStart);
	IC		void	SetPatrolStop			(PatrolPathManager::EPatrolRouteType tPatrolPathStop);
	IC		void	SetPatrolRandom			(bool bRandom);
	IC		void	SetInputKeys			(const EInputKeys tInputKeys);
	IC		void	initialize				();

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptMovementAction)
#undef script_type_list
#define script_type_list save_type_list(CScriptMovementAction)

#include "script_movement_action_inline.h"