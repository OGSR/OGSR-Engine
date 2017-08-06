#pragma once
#include "../state.h"

template<typename _Object>
class	CStateBurerAttack : public CState<_Object> {
	typedef CState<_Object> inherited;
	typedef CState<_Object> *state_ptr;

	bool				m_force_gravi;
public:
						CStateBurerAttack		(_Object *obj);

	virtual	void		initialize				();
							
	virtual	void		reselect_state			();
	virtual void		setup_substates			();
	virtual void		check_force_state		();
};

#include "burer_state_attack_inline.h"
