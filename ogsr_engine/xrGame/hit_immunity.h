// hit_immunity.h: класс для тех объектов, которые поддерживают
//				   коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#pragma once

#include "alife_space.h"
#include "script_export_space.h"
#include "hit_immunity_space.h"

class CHitImmunity
{
public:
	CHitImmunity();
	virtual ~CHitImmunity();

	virtual void LoadImmunities (LPCSTR section,CInifile* ini);

	virtual float AffectHit		(float power, ALife::EHitType hit_type);

protected:
	//коэффициенты на которые домножается хит
	//при соответствующем типе воздействия
	//(для защитных костюмов и специфичных животных)
	HitImmunity::HitTypeSVec m_HitTypeK;
public:
	HitImmunity::HitTypeSVec &immunities() { return m_HitTypeK; }
	static void	script_register (lua_State *L);

	virtual	CHitImmunity*	cast_hit_immunities() { return this; }
};