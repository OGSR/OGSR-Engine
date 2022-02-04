#pragma once

#include "../state.h"

template<typename _Object>
class CStateZombieAttackRun : public CState<_Object> {
	typedef CState<_Object> inherited;
	using inherited::object;

	TTime				m_time_action_change{};
	EAction				action;

public:
						CStateZombieAttackRun	(_Object *obj);
	virtual				~CStateZombieAttackRun	();

	virtual void		initialize				();
	virtual	void		execute					();

	virtual bool 		check_completion		();
	virtual bool 		check_start_conditions	();
	virtual void		remove_links			(CObject* object) { inherited::remove_links(object);}

private:
			void		choose_action			();

};

#include "zombie_state_attack_run_inline.h"
