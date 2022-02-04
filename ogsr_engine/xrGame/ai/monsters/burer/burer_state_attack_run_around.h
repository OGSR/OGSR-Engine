#pragma once
#include "../state.h"

template<typename _Object>
class	CStateBurerAttackRunAround : public CState<_Object> {
	typedef CState<_Object>	inherited;
	using inherited::object;

	Fvector				selected_point;
	u32					time_started{};

	Fvector				dest_direction;

public:
						CStateBurerAttackRunAround	(_Object *obj);
	virtual void		initialize					();
	virtual void		execute						();

	virtual bool		check_start_conditions		();
	virtual bool		check_completion			();
	virtual void		remove_links				(CObject* object) { inherited::remove_links(object);}
};

#include "burer_state_attack_run_around_inline.h"