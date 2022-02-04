#pragma once
#include "../state.h"
#include "state_data.h"

template<typename _Object>
class CStateMonsterHideFromPoint : public CState<_Object> {
	typedef CState<_Object> inherited;
	using inherited::object;
	using inherited::time_state_started;

	SStateHideFromPoint data;

public:
						CStateMonsterHideFromPoint	(_Object *obj) : inherited(obj, &data){}
	virtual				~CStateMonsterHideFromPoint	() {}

	virtual void		initialize					();
	virtual	void		execute						();
	virtual void		remove_links				(CObject* object) { inherited::remove_links(object);}

	virtual bool		check_completion			();
						
};

#include "state_hide_from_point_inline.h"