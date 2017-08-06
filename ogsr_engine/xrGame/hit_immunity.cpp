// hit_immunity.cpp:	класс для тех объектов, которые поддерживают
//						коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_immunity.h"


CHitImmunity::CHitImmunity()
{
	m_HitTypeK.resize(ALife::eHitTypeMax);
	for(int i=0; i<ALife::eHitTypeMax; i++)
		m_HitTypeK[i] = 1.0f;
}


CHitImmunity::~CHitImmunity()
{
}
void CHitImmunity::LoadImmunities(LPCSTR imm_sect,CInifile* ini)
{
	R_ASSERT2	(ini->section_exist(imm_sect), imm_sect);

	m_HitTypeK[ALife::eHitTypeBurn]			= ini->r_float(imm_sect,"burn_immunity");
	m_HitTypeK[ALife::eHitTypeStrike]		= ini->r_float(imm_sect,"strike_immunity");
	m_HitTypeK[ALife::eHitTypeShock]		= ini->r_float(imm_sect,"shock_immunity");
	m_HitTypeK[ALife::eHitTypeWound]		= ini->r_float(imm_sect,"wound_immunity");
	m_HitTypeK[ALife::eHitTypeRadiation]	= ini->r_float(imm_sect,"radiation_immunity");
	m_HitTypeK[ALife::eHitTypeTelepatic]	= ini->r_float(imm_sect,"telepatic_immunity");
	m_HitTypeK[ALife::eHitTypeChemicalBurn] = ini->r_float(imm_sect,"chemical_burn_immunity");
	m_HitTypeK[ALife::eHitTypeExplosion]	= ini->r_float(imm_sect,"explosion_immunity");
	m_HitTypeK[ALife::eHitTypeFireWound]	= ini->r_float(imm_sect,"fire_wound_immunity");
	m_HitTypeK[ALife::eHitTypePhysicStrike]	= READ_IF_EXISTS(ini, r_float, imm_sect,"physic_strike_wound_immunity", 1.0f);
}

float CHitImmunity::AffectHit (float power, ALife::EHitType hit_type)
{
	return power*m_HitTypeK[hit_type];
}