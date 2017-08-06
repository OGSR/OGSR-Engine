#pragma once

#include "../../cover_evaluators.h"

class CMonsterCorpseCoverEvaluator: public CCoverEvaluatorBase {
protected:
	typedef CCoverEvaluatorBase inherited;

protected:
	float				m_min_distance;
	float				m_max_distance;

public:

	IC	CMonsterCorpseCoverEvaluator		(CRestrictedObject *object) : inherited(object)
	{
	}

	IC		void		setup				(float min_enemy_distance, float max_enemy_distance){
		inherited::setup		();
		m_min_distance			= min_enemy_distance;
		m_max_distance			= max_enemy_distance;
	}

			void		evaluate			(const CCoverPoint *cover_point, float weight);
};





