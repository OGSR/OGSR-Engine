#pragma once

#include "../state.h"

template<typename _Object>
class	CStatePsyDogPsyAttack : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

public:
						CStatePsyDogPsyAttack	(_Object *obj);
	virtual				~CStatePsyDogPsyAttack	() {}

	virtual void		reselect_state			();
};

#include "psy_dog_state_psy_attack_inline.h"
