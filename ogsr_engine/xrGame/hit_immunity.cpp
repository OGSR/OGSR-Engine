// hit_immunity.cpp:	класс для тех объектов, которые поддерживают
//						коэффициенты иммунитета для разных типов хитов
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hit_immunity.h"
#include "GameObject.h"

CHitImmunity::CHitImmunity()
{
    m_HitTypeK.resize(ALife::eHitTypeMax);
    for (int i = 0; i < ALife::eHitTypeMax; i++)
        m_HitTypeK[i] = 1.0f;
}

CHitImmunity::~CHitImmunity() {}
void CHitImmunity::LoadImmunities(LPCSTR imm_sect, CInifile* ini)
{
    R_ASSERT2(ini->section_exist(imm_sect), imm_sect);

    m_HitTypeK[ALife::eHitTypeBurn] = ini->r_float(imm_sect, "burn_immunity");
    m_HitTypeK[ALife::eHitTypeStrike] = ini->r_float(imm_sect, "strike_immunity");
    m_HitTypeK[ALife::eHitTypeShock] = ini->r_float(imm_sect, "shock_immunity");
    m_HitTypeK[ALife::eHitTypeWound] = ini->r_float(imm_sect, "wound_immunity");
    m_HitTypeK[ALife::eHitTypeRadiation] = ini->r_float(imm_sect, "radiation_immunity");
    m_HitTypeK[ALife::eHitTypeTelepatic] = ini->r_float(imm_sect, "telepatic_immunity");
    m_HitTypeK[ALife::eHitTypeChemicalBurn] = ini->r_float(imm_sect, "chemical_burn_immunity");
    m_HitTypeK[ALife::eHitTypeExplosion] = ini->r_float(imm_sect, "explosion_immunity");
    m_HitTypeK[ALife::eHitTypeFireWound] = ini->r_float(imm_sect, "fire_wound_immunity");
    m_HitTypeK[ALife::eHitTypePhysicStrike] = READ_IF_EXISTS(ini, r_float, imm_sect, "physic_strike_wound_immunity", 1.0f);
    m_HitTypeK[ALife::eHitTypeWound_2] = READ_IF_EXISTS(ini, r_float, imm_sect, "wound_2_immunity", 1.0f);
}

float CHitImmunity::AffectHit(float power, ALife::EHitType hit_type) { return power * m_HitTypeK[hit_type]; }

using namespace luabind;

float get_burn_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeBurn]; }
void set_burn_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeBurn] = i; }

float get_strike_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeStrike]; }
void set_strike_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeStrike] = i; }

float get_shock_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeShock]; }
void set_shock_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeShock] = i; }

float get_wound_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeWound]; }
void set_wound_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeWound] = i; }

float get_radiation_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeRadiation]; }
void set_radiation_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeRadiation] = i; }

float get_telepatic_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeTelepatic]; }
void set_telepatic_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeTelepatic] = i; }

float get_chemical_burn_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeChemicalBurn]; }
void set_chemical_burn_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeChemicalBurn] = i; }

float get_explosion_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeExplosion]; }
void set_explosion_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeExplosion] = i; }

float get_fire_wound_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeFireWound]; }
void set_fire_wound_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeFireWound] = i; }

float get_wound_2_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypeWound_2]; }
void set_wound_2_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypeWound_2] = i; }

float get_physic_strike_immunity(CHitImmunity* I) { return I->immunities()[ALife::eHitTypePhysicStrike]; }
void set_physic_strike_immunity(CHitImmunity* I, float i) { I->immunities()[ALife::eHitTypePhysicStrike] = i; }

// extern LPCSTR get_lua_class_name(luabind::object O);

void CHitImmunity::script_register(lua_State* L)
{
    module(L)[class_<CHitImmunity>("CHitImmunity")
                  .property("burn_immunity", &get_burn_immunity, &set_burn_immunity)
                  .property("strike_immunity", &get_strike_immunity, &set_strike_immunity)
                  .property("shock_immunity", &get_shock_immunity, &set_shock_immunity)
                  .property("wound_immunity", &get_wound_immunity, &set_wound_immunity)
                  .property("radiation_immunity", &get_radiation_immunity, &set_radiation_immunity)
                  .property("telepatic_immunity", &get_telepatic_immunity, &set_telepatic_immunity)
                  .property("chemical_burn_immunity", &get_chemical_burn_immunity, &set_chemical_burn_immunity)
                  .property("explosion_immunity", &get_explosion_immunity, &set_explosion_immunity)
                  .property("fire_wound_immunity", &get_fire_wound_immunity, &set_fire_wound_immunity)
                  .property("wound_2_immunity", &get_wound_2_immunity, &set_wound_2_immunity)
                  .property("physic_strike_immunity", &get_physic_strike_immunity, &set_physic_strike_immunity)
              //.property("class_name"				,			&get_lua_class_name)
    ];
}
