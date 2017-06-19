#pragma once
#include "../monster_state_manager.h"

class CChimera;

class CStateManagerChimera : public CMonsterStateManager<CChimera> {
	
	typedef CMonsterStateManager<CChimera> inherited;

public:
						CStateManagerChimera	(CChimera *obj);
	virtual				~CStateManagerChimera	();

	virtual	void		execute					();
};
