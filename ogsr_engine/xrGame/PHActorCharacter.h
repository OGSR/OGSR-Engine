#pragma once

#include "phsimplecharacter.h"
#include "ExtendedGeom.h"

struct SPHCharacterRestrictor
{
							SPHCharacterRestrictor							(CPHCharacter::ERestrictionType Ttype)
							{
								m_type=Ttype;
								m_character=NULL;
								m_restrictor=NULL;
								m_restrictor_transform=NULL;
								m_restrictor_radius=0.1f;
							}
							~SPHCharacterRestrictor				()
							{
								Destroy();
							};
				CPHCharacter*			m_character;
				CPHCharacter::ERestrictionType m_type;

				dGeomID					m_restrictor;
				dGeomID					m_restrictor_transform;
				float					m_restrictor_radius;
				void					SetObjectContactCallback			(ObjectContactCallbackFun* callback);
				void					SetMaterial							(u16 material);
				void					Create								(CPHCharacter* ch,dVector3 sizes);
				void					Destroy								(void);
				void					SetPhysicsRefObject					(CPhysicsShellHolder* ref_object);
				void					SetRadius							(float r);
};

template <CPHCharacter::ERestrictionType Ttype>
struct TPHCharacterRestrictor : public SPHCharacterRestrictor
{
		TPHCharacterRestrictor():SPHCharacterRestrictor(Ttype){}
		void	Create(CPHCharacter* ch,dVector3 sizes)
		{
			dGeomUserDataSetObjectContactCallback(m_restrictor,RestrictorCallBack);
		}
static	void RestrictorCallBack	(bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2)
		{
			do_colide=false;
			dBodyID						b1		=	dGeomGetBody(c.geom.g1);
			dBodyID						b2		=	dGeomGetBody(c.geom.g2);
			if(!(b1&&b2))	return;
			dxGeomUserData				*ud1	=	retrieveGeomUserData(c.geom.g1);
			dxGeomUserData				*ud2	=	retrieveGeomUserData(c.geom.g2);
			if(!(ud1&&ud2))return;

			CPHObject					*o1		=	NULL;if(ud1)o1=ud1->ph_object;
			CPHObject					*o2		=	NULL;if(ud2)o2=ud2->ph_object;
			if(!(o1&&o2))				return;
			if(o1->CastType()!=CPHObject::tpCharacter||o2->CastType()!=CPHObject::tpCharacter) return;

			CPHCharacter* ch1					=	static_cast<CPHCharacter*>(o1);
			CPHCharacter* ch2					=	static_cast<CPHCharacter*>(o2);

			if(bo1)
			{
				ch1			->	ChooseRestrictionType(Ttype,c.geom.depth,ch2);
				do_colide	=	ch2->TouchRestrictor(Ttype);
			}
			else
			{
				ch2			->	ChooseRestrictionType(Ttype,c.geom.depth,ch1);
				do_colide	=	ch1->TouchRestrictor(Ttype);
			}
		}
	};
DEFINE_VECTOR(SPHCharacterRestrictor*,RESRICTORS_V,RESTRICTOR_I);
//typedef SPHCharacterRestrictor*		RESRICTORS_V[2];
//typedef SPHCharacterRestrictor**	RESTRICTOR_I;
IC RESTRICTOR_I begin(RESRICTORS_V& v)
{
	//return v;
	return v.begin();
}

IC RESTRICTOR_I end(RESRICTORS_V& v)
{
	//return v+sizeof(RESRICTORS_V)/sizeof(SPHCharacterRestrictor*);
	return v.end();
}

class CPHActorCharacter :
	public CPHSimpleCharacter
{
	typedef CPHSimpleCharacter	inherited;

	RESRICTORS_V		m_restrictors;
	float				m_speed_goal{};
public:
	typedef TPHCharacterRestrictor<CPHCharacter::rtStalker>			stalker_restrictor;
	typedef TPHCharacterRestrictor<CPHCharacter::rtStalkerSmall>	stalker_small_restrictor;
	typedef TPHCharacterRestrictor<CPHCharacter::rtMonsterMedium>	medium_monster_restrictor;
public:
	virtual CPHActorCharacter	*CastActorCharacter			(){return this;}
	virtual	void		SetObjectContactCallback			(ObjectContactCallbackFun* callback);
	virtual void		SetMaterial							(u16 material);
	virtual void		Create								(dVector3 sizes);
	virtual void		Destroy								(void);
	virtual void		SetPhysicsRefObject					(CPhysicsShellHolder* ref_object);
	virtual void		SetAcceleration						(Fvector accel);
	virtual	void		Disable								();
	virtual	void		Jump								(const Fvector& jump_velocity);
	virtual void		InitContact							(dContact* c,bool &do_collide,u16	material_idx_1 ,u16 material_idx_2);
			void		SetRestrictorRadius					(CPHCharacter::ERestrictionType rtype,float r);
virtual		void		ChooseRestrictionType				(ERestrictionType my_type,float my_depth,CPHCharacter *ch);
						CPHActorCharacter					();
	virtual				~CPHActorCharacter					(void);
private:
		void			ClearRestrictors					();
		RESTRICTOR_I	Restrictor							(CPHCharacter::ERestrictionType rtype);
};
