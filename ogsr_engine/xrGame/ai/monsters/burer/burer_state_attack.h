#pragma once
#include "../state.h"

template<typename Object>
class	CStateBurerAttack : public CState<Object> 
{
	typedef CState<Object> inherited;
	typedef CState<Object> *state_ptr;
	using inherited::object;
	using inherited::prev_substate;
	using inherited::current_substate;
	using inherited::select_state;
	using inherited::get_state_current;
	using inherited::get_state;
	using inherited::add_state;

public:
						CStateBurerAttack			(Object *obj);

	virtual	void		initialize					();
	virtual	void		execute						();
	virtual void		remove_links				(CObject* object) { inherited::remove_links(object); }

	virtual void		finalize					();
	virtual void		critical_finalize			();
	virtual bool		check_control_start_conditions	(ControlCom::EControlType type);

private:
	bool				m_wait_state_end;
	bool				m_lost_delta_health;
	bool				m_allow_anti_aim;
	float				m_last_health{};
	TTime				m_next_runaway_allowed_tick{};
};

#include "burer_state_attack_inline.h"
