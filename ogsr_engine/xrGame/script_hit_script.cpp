////////////////////////////////////////////////////////////////////////////
//	Module 		: script_hit_script.cpp
//	Created 	: 06.02.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : XRay Script hit class script export
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_hit.h"
#include "script_game_object.h"

using namespace luabind;

#pragma optimize("s", on)
void CScriptHit::script_register(lua_State* L)
{
    module(L)[class_<CScriptHit>("hit")
                  .enum_("hit_type")[value("burn", int(ALife::eHitTypeBurn)), value("shock", int(ALife::eHitTypeShock)), value("strike", int(ALife::eHitTypeStrike)),
                                     value("wound", int(ALife::eHitTypeWound)), value("radiation", int(ALife::eHitTypeRadiation)),
                                     value("telepatic", int(ALife::eHitTypeTelepatic)), value("chemical_burn", int(ALife::eHitTypeChemicalBurn)),
                                     value("explosion", int(ALife::eHitTypeExplosion)), value("fire_wound", int(ALife::eHitTypeFireWound)), value("dummy", int(ALife::eHitTypeMax))]
                  .def_readwrite("power", &CScriptHit::m_fPower)
                  .def_readwrite("direction", &CScriptHit::m_tDirection)
                  .def_readwrite("draftsman", &CScriptHit::m_tpDraftsman)
                  .def_readwrite("impulse", &CScriptHit::m_fImpulse)
                  .def_readwrite("type", &CScriptHit::m_tHitType)
                  .def(constructor<>())
                  .def(constructor<const CScriptHit*>())
                  .def("bone", &CScriptHit::set_bone_name),
              // KRodin: экспортировал класс SHit в скрипты. Нужно для каллбека entity_alive_before_hit.
              //Сделано по образу и подобию движка X-Ray Extensions.
              class_<SHit>("SHit")
                  .def(constructor<>())
                  .def_readwrite("time", &SHit::Time)
                  .def_readwrite("packet_type", &SHit::PACKET_TYPE)
                  .def_readwrite("dest_id", &SHit::DestID)
                  .def_readwrite("power", &SHit::power)
                  .def_readwrite("dir", &SHit::dir) //Вектор
                  .property("who", &SHit::get_hit_initiator, &SHit::set_hit_initiator) //Сделал так, чтобы тут можно было передавать и получать скриптовые клиентские объекты.
                  .def_readwrite("who_id", &SHit::whoID)
                  .def_readwrite("weapon_id", &SHit::weaponID)
                  .def_readwrite("bone_id", &SHit::boneID)
                  .def_readwrite("p_in_bone_space", &SHit::p_in_bone_space) //Вектор
                  .def_readwrite("impulse", &SHit::impulse)
                  .def_readwrite("hit_type", &SHit::hit_type)
                  .def_readwrite("ap", &SHit::ap)
                  .def_readwrite("aim_bullet", &SHit::aim_bullet)
                  .def_readwrite("bullet_id", &SHit::BulletID)
                  .def_readwrite("sender_id", &SHit::SenderID)
                  .def_readwrite("ignore_hit", &SHit::ignore_flag) //Флаг игнорирования хита. Если скриптово установить его в true, хит нанесён не будет.
                  // Начальное значение хита, до обработок всякими
                  // защитами артефактов и броней.
                  .def_readonly("full_power", &SHit::full_power)];
}
