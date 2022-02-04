#pragma once

#include "../state.h"

template<typename _Object>
class	CStateMonsterHearInterestingSound : public CState<_Object> {
protected:
	typedef CState<_Object>		inherited;
	typedef CState<_Object>*	state_ptr;
	using inherited::object;
	using inherited::prev_substate;
	using inherited::current_substate;
	using inherited::select_state;
	using inherited::get_state_current;
	using inherited::get_state;
	using inherited::add_state;

public:
					CStateMonsterHearInterestingSound	(_Object *obj);
	virtual			~CStateMonsterHearInterestingSound	(){}

	virtual void	reselect_state						();
	virtual void	setup_substates						();
	virtual void	remove_links						(CObject* object) { inherited::remove_links(object);}

private:
			Fvector	get_target_position					();

};

#include "monster_state_hear_int_sound_inline.h"