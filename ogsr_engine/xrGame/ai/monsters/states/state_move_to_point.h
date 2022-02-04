#pragma once
#include "../state.h"
#include "state_data.h"

template<typename _Object>
class CStateMonsterMoveToPoint : public CState<_Object> {
	typedef CState<_Object> inherited;
	using inherited::object;
	using inherited::time_state_started;

	SStateDataMoveToPoint data;

public:
						CStateMonsterMoveToPoint	(_Object *obj) : inherited(obj, &data) {}
	virtual				~CStateMonsterMoveToPoint	() {}

	virtual void		initialize					();
	virtual	void		execute						();
	virtual void		remove_links				(CObject* object) { inherited::remove_links(object);}

	virtual bool		check_completion			();

};


template<typename _Object>
class CStateMonsterMoveToPointEx : public CState<_Object> {
	typedef CState<_Object> inherited;
	using inherited::object;
	using inherited::time_state_started;

protected:
	
	SStateDataMoveToPointEx data;

public:
						CStateMonsterMoveToPointEx	(_Object *obj) : inherited(obj, &data) {}
	virtual				~CStateMonsterMoveToPointEx	() {}
	virtual void		initialize					();
	virtual	void		execute						();
	virtual bool		check_completion			();
	virtual void		remove_links				(CObject* object) { inherited::remove_links(object);}
};

#include "state_move_to_point_inline.h"
