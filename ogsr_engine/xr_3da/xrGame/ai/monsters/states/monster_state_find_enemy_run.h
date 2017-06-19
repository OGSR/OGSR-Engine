#pragma once

#include "../state.h"

template<typename _Object>
class CStateMonsterFindEnemyRun : public CState<_Object> {
	typedef CState<_Object> inherited;

	Fvector				target_point;
	u32					target_vertex;

public:
						CStateMonsterFindEnemyRun	(_Object *obj);
	virtual				~CStateMonsterFindEnemyRun	();

	virtual void		initialize					();
	virtual	void		execute						();
	virtual bool		check_completion			();
};

#include "monster_state_find_enemy_run_inline.h"
