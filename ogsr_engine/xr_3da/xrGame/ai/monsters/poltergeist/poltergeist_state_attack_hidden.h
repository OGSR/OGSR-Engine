#pragma once

#include "../state.h"

template<typename _Object>
class	CStatePoltergeistAttackHidden : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

	struct {
		Fvector point;
		u32		node;
	} m_target;


public:
					CStatePoltergeistAttackHidden	(_Object *obj) : inherited(obj) {}
	virtual			~CStatePoltergeistAttackHidden	() {}


	virtual void	initialize				();
	virtual void	execute					();

private:

			void	select_target_point		();
};

#include "poltergeist_state_attack_hidden_inline.h"
