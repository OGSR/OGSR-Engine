#pragma once

#include "../state.h"


template<typename _Object>
class	CStateMonsterFindEnemy : public CState<_Object> {
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
						CStateMonsterFindEnemy	(_Object *obj);
	virtual				~CStateMonsterFindEnemy	();
	virtual void		remove_links			(CObject* object) { inherited::remove_links(object);}

	virtual	void		reselect_state			();
};

#include "monster_state_find_enemy_inline.h"

