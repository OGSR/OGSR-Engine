// ActorCondition.h: класс состояния игрока
//

#pragma once

#include "EntityCondition.h"
#include "actor_defs.h"
#include "..\xr_3da\feel_touch.h"

template <typename _return_type>
class CScriptCallbackEx;



class CActor;

class CActorCondition: public CEntityCondition {
	friend class CScriptActor;
public:
	typedef CEntityCondition inherited;
	enum {	eCriticalPowerReached			=(1<<0),
			eCriticalMaxPowerReached		=(1<<1),
			eCriticalBleedingSpeed			=(1<<2),
			eCriticalSatietyReached			=(1<<3),
			eCriticalRadiationReached		=(1<<4),
			eWeaponJammedReached			=(1<<5),
			ePhyHealthMinReached			=(1<<6),
			eCantWalkWeight					=(1<<7),

			eLimping = (1 << 8),
			eCantWalk = (1 << 9),
			eCantSprint = (1 << 10),
			};
	Flags16											m_condition_flags;
private:
	CActor*											m_object;
	void				UpdateTutorialThresholds	();
	void 				UpdateSatiety				();
public:
						CActorCondition				(CActor *object);
	virtual				~CActorCondition			(void);

	virtual void		LoadCondition				(LPCSTR section);
	virtual void		reinit						();

	virtual CWound*		ConditionHit				(SHit* pHDS);
	virtual void		UpdateCondition				();

	virtual void 		ChangeAlcohol				(float value);
	virtual void 		ChangeSatiety				(float value);

	// хромание при потере сил и здоровья
	virtual	bool		IsLimping					();
	virtual bool		IsCantWalk					();
	virtual bool		IsCantWalkWeight			();
	virtual bool		IsCantSprint				();

	void		PowerHit					(float power, bool apply_outfit);
	virtual void		UpdatePower();

	void		ConditionJump				(float weight);
	void		ConditionWalk				(float weight, bool accel, bool sprint);
	void		ConditionStand				(float weight);
	
	float	xr_stdcall	GetAlcohol			()	{return m_fAlcohol;}
	float	xr_stdcall	GetPsy				()	{return 1.0f-GetPsyHealth();}
	float				GetSatiety			()  {return m_fSatiety;}
	void				SetMaxWalkWeight	(float _weight) { m_MaxWalkWeight = _weight; }

	void		AffectDamage_InjuriousMaterialAndMonstersInfluence();
	float		GetInjuriousMaterialDamage	();

public:
	IC		CActor		&object						() const
	{
		VERIFY			(m_object);
		return			(*m_object);
	}
	virtual void			save					(NET_Packet &output_packet);
	virtual void			load					(IReader &input_packet);
	float	m_MaxWalkWeight;

	bool	DisableSprint							(SHit* pHDS);
	float	HitSlowmo								(SHit* pHDS);

protected:
	float m_fAlcohol;
	float m_fV_Alcohol;
//--
	float m_fSatiety;
	float m_fSatietyLightLimit;
	float m_fSatietyCriticalLimit;
	float m_fV_Satiety;
	float m_fV_SatietyPower;
	float m_fV_SatietyHealth;
//--
	float m_fPowerLeakSpeed;
	float m_fV_Power;

	float m_fJumpPower;
	float m_fStandPower;
	float m_fWalkPower;
	float m_fJumpWeightPower;
	float m_fWalkWeightPower;
	float m_fOverweightWalkK;
	float m_fOverweightJumpK;
	float m_fAccelK;
	float m_fSprintK;

	float	m_f_time_affected;
	
	//порог силы и здоровья меньше которого актер начинает хромать
	float m_fLimpingPowerBegin;
	float m_fLimpingPowerEnd;
	float m_fCantWalkPowerBegin;
	float m_fCantWalkPowerEnd;

	float m_fCantSprintPowerBegin;
	float m_fCantSprintPowerEnd;

	float m_fLimpingHealthBegin;
	float m_fLimpingHealthEnd;

protected:
	Feel::Touch* monsters_feel_touch;
	float        monsters_aura_radius;

public:
	void net_Relcase( CObject* O );
	void set_monsters_aura_radius( float r ) { if ( r > monsters_aura_radius ) monsters_aura_radius = r; };
};
