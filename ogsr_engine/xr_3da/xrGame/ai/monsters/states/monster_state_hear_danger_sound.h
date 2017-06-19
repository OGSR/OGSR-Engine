#pragma once

#include "../state.h"

template<typename _Object>
class	CStateMonsterHearDangerousSound : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;

public:
					CStateMonsterHearDangerousSound		(_Object *obj);
	virtual			~CStateMonsterHearDangerousSound	() {}

	virtual void	reselect_state						();
	virtual void	setup_substates						();

};

#include "monster_state_hear_danger_sound_inline.h"