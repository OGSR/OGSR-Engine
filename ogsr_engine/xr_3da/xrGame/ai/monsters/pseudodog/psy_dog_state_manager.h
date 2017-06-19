#pragma once
#include "pseudodog_state_manager.h"

class CStateManagerPsyDog : public CStateManagerPseudodog {
	typedef CStateManagerPseudodog inherited;
public:
					CStateManagerPsyDog	(CAI_PseudoDog *monster); 
	virtual void	execute				();
};
