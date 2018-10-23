#pragma once
#include "../state.h"

template<typename _Object>
class CStateBurerAttackGravi : public CState<_Object> {
	typedef CState<_Object> inherited;	
	
	enum {
		ACTION_GRAVI_STARTED,
		ACTION_GRAVI_CONTINUE,
		ACTION_GRAVI_FIRE,
		ACTION_WAIT_TRIPLE_END,
		ACTION_COMPLETED,
	} m_action;

	u32			time_gravi_started;

public:
							CStateBurerAttackGravi	(_Object *obj);

		virtual	void		initialize				();
		virtual	void		execute					();
		virtual void		finalize				();
		virtual void		critical_finalize		();

		virtual bool		check_start_conditions	();
		virtual bool		check_completion		();

private:
				// выполнять состояние
				void		ExecuteGraviStart		();
				void		ExecuteGraviContinue	();
				void		ExecuteGraviFire		();

};

#include "burer_state_attack_gravi_inline.h"

