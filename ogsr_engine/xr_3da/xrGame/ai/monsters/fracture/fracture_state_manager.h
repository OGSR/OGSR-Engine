#pragma once
#include "../monster_state_manager.h"

class CFracture;

class CStateManagerFracture : public CMonsterStateManager<CFracture> {
	typedef CMonsterStateManager<CFracture> inherited;

public:
						CStateManagerFracture	(CFracture *obj);
	virtual				~CStateManagerFracture	();

	virtual	void		execute					();
};
