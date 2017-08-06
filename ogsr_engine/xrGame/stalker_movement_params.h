////////////////////////////////////////////////////////////////////////////
//	Module 		: stalker_movement_params.h
//	Created 	: 23.12.2005
//  Modified 	: 23.12.2005
//	Author		: Dmitriy Iassenev
//	Description : Stalker movement parameters class
////////////////////////////////////////////////////////////////////////////

#pragma once

namespace MonsterSpace {
	enum EBodyState;
	enum EMovementType;
	enum EMentalState;
	enum EPathType;
};

namespace MovementManager {
	enum EPathType;
};

namespace DetailPathManager {
	enum EDetailPathType;
};

class CStalkerMovementParams {
	friend class CStalkerMovementManager;

private:
	Fvector								m_desired_position;
	Fvector								m_desired_direction;
	MonsterSpace::EBodyState			m_body_state;
	MonsterSpace::EMovementType			m_movement_type;
	MonsterSpace::EMentalState			m_mental_state;
	MovementManager::EPathType			m_path_type;
	DetailPathManager::EDetailPathType	m_detail_path_type;
	bool								m_use_desired_position;
	bool								m_use_desired_direction;

public:
	IC		bool	operator==			(const CStalkerMovementParams &params) const;
	IC		bool	operator!=			(const CStalkerMovementParams &params) const;
};

#include "stalker_movement_params_inline.h"