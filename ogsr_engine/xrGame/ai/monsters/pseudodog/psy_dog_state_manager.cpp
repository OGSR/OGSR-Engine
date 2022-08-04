#include "stdafx.h"
#include "pseudodog.h"
#include "psy_dog_state_manager.h"

CStateManagerPsyDog::CStateManagerPsyDog(CAI_PseudoDog* monster) : inherited(monster) {}

void CStateManagerPsyDog::execute() { inherited::execute(); }
