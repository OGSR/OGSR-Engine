#pragma once
#include "../state.h"

template<typename _Object>
class CStateMonsterAttackRunAttack : public CState<_Object> {
	typedef CState<_Object> inherited;

public:
						CStateMonsterAttackRunAttack	(_Object *obj) : inherited(obj) {};

	virtual	void		initialize						();
	virtual	void		execute							();
	virtual	void		finalize						();
	virtual	void		critical_finalize				();

	virtual bool 		check_completion				();
	virtual bool 		check_start_conditions			();
	virtual void		remove_links					(CObject* object) { inherited::remove_links(object);}
};

#include "monster_state_attack_run_attack_inline.h"
