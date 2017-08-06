#pragma once
#include "../monster_state_manager.h"

class CSnork;

class CStateManagerSnork : public CMonsterStateManager<CSnork> {
	typedef CMonsterStateManager<CSnork> inherited;

public:
						CStateManagerSnork		(CSnork *obj);
	virtual				~CStateManagerSnork		();

	virtual	void		execute					();
};
