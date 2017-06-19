#pragma once
#include "../monster_state_manager.h"

class CAI_Bloodsucker;

class CStateManagerBloodsucker : public CMonsterStateManager<CAI_Bloodsucker> {
	typedef CMonsterStateManager<CAI_Bloodsucker> inherited;

public:
					CStateManagerBloodsucker	(CAI_Bloodsucker *monster); 
	virtual void	execute						();
};
