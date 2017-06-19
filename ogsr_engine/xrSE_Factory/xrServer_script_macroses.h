////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_script_macroses.cpp
//	Created 	: 24.06.2004
//  Modified 	: 24.06.2004
//	Author		: Dmitriy Iassenev
//	Description : Server script macroses
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "script_export_macroses.h"
#include "xrEProps.h"
#include "net_utils.h"
#include "ai_space.h"
#include "script_engine.h"

//#define USE_WRITER_READER
class CSE_Abstract;
class NET_Packet;
class CSE_ALifeMonsterAbstract;
class CALifeSmartTerrainTask;

#define INHERIT_PURE

//#ifndef USE_WRITER_READER
//#	define INHERIT_PURE \
//	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(save,			NET_Packet)\
//	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(load,			NET_Packet)
//#else
//#	define INHERIT_PURE \
//	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(save,			NET_Packet)\
//	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(load,			NET_Packet)\
//	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(save,			IWriter)\
//	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(load,			IReader)
//#endif

#define INHERIT_ABSTRACT \
	INHERIT_PURE\
	DEFINE_LUA_WRAPPER_METHOD_R2P1_V1	(STATE_Write,	NET_Packet)\
	DEFINE_LUA_WRAPPER_METHOD_R2P1_V2	(STATE_Read,	NET_Packet,	u16)\
	DEFINE_LUA_WRAPPER_METHOD_R2P2_V2	(FillProps,		LPCSTR,	PropItemVec)\
	DEFINE_LUA_WRAPPER_METHOD_R2P1_V4	(OnEvent,		NET_Packet, u16, u32, ClientID)\
	DEFINE_LUA_WRAPPER_METHOD_0			(init,			CSE_Abstract*)

#define INHERIT_ALIFE \
	INHERIT_ABSTRACT\
	DEFINE_LUA_WRAPPER_CONST_METHOD_0	(used_ai_locations,	bool)\
	DEFINE_LUA_WRAPPER_CONST_METHOD_0	(can_save,			bool)\
	DEFINE_LUA_WRAPPER_CONST_METHOD_0	(can_switch_online,	bool)\
	DEFINE_LUA_WRAPPER_CONST_METHOD_0	(can_switch_offline,bool)\
	DEFINE_LUA_WRAPPER_CONST_METHOD_0	(interactive,		bool)

#ifdef XRGAME_EXPORTS
#	define INHERIT_DYNAMIC_ALIFE \
	INHERIT_ALIFE\
	DEFINE_LUA_WRAPPER_METHOD_V0		(on_spawn)\
	DEFINE_LUA_WRAPPER_METHOD_V0		(on_before_register)\
	DEFINE_LUA_WRAPPER_METHOD_V0		(on_register)\
	DEFINE_LUA_WRAPPER_METHOD_V0		(on_unregister)
#else
#	define INHERIT_DYNAMIC_ALIFE \
	INHERIT_ALIFE
#endif

#ifdef XRGAME_EXPORTS
#	define INHERIT_ZONE \
	INHERIT_DYNAMIC_ALIFE\
	DEFINE_LUA_WRAPPER_METHOD_V0		(update)\
	DEFINE_LUA_WRAPPER_METHOD_V1		(smart_touch,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_CONST_METHOD_1	(enabled,bool,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_CONST_METHOD_1	(suitable,float,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_METHOD_V1		(register_npc,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_METHOD_V1		(unregister_npc,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_METHOD_1			(task,CALifeSmartTerrainTask*,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_METHOD_0			(detect_probability,float)
#else
#	define INHERIT_ZONE \
	INHERIT_DYNAMIC_ALIFE\
	DEFINE_LUA_WRAPPER_METHOD_V0		(update)\
	DEFINE_LUA_WRAPPER_METHOD_V1		(smart_touch,CSE_ALifeMonsterAbstract*)\
	DEFINE_LUA_WRAPPER_METHOD_0			(detect_probability,float)
#endif

#ifdef XRGAME_EXPORTS
#	define INHERIT_CREATURE \
		INHERIT_DYNAMIC_ALIFE\
		DEFINE_LUA_WRAPPER_METHOD_0			(g_team,	u8)\
		DEFINE_LUA_WRAPPER_METHOD_0			(g_squad,	u8)\
		DEFINE_LUA_WRAPPER_METHOD_0			(g_group,	u8)\
		DEFINE_LUA_WRAPPER_METHOD_V1		(on_death,	CSE_Abstract*)
#else // XRGAME_EXPORTS
#	define INHERIT_CREATURE \
		INHERIT_DYNAMIC_ALIFE\
		DEFINE_LUA_WRAPPER_METHOD_0			(g_team,	u8)\
		DEFINE_LUA_WRAPPER_METHOD_0			(g_squad,	u8)\
		DEFINE_LUA_WRAPPER_METHOD_0			(g_group,	u8)
#endif // XRGAME_EXPORTS

#define INHERIT_MONSTER \
	INHERIT_CREATURE\
	DEFINE_LUA_WRAPPER_METHOD_V0		(update)

#define INHERIT_ITEM \
	INHERIT_DYNAMIC_ALIFE\
	DEFINE_LUA_WRAPPER_METHOD_0			(bfUseful,	bool)

template <typename T>
struct CWrapperPure : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperPure<T>				self_type;
	CWrapperPure						(LPCSTR section) : inherited(section){}
	INHERIT_PURE;
};

template <typename T>
struct CWrapperAbstract : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperAbstract<T>			self_type;

	CWrapperAbstract					(LPCSTR section) : inherited(section){}
	INHERIT_ABSTRACT;
};

template <typename T>
struct CWrapperAbstractALife : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperAbstractALife<T>	self_type;
	CWrapperAbstractALife				(LPCSTR section) : inherited(section){}
	INHERIT_ALIFE;
};

template <typename T>
struct CWrapperAbstractDynamicALife : public T, public luabind::wrap_base {
	typedef T								inherited;
	typedef CWrapperAbstractDynamicALife<T>	self_type;
	CWrapperAbstractDynamicALife		(LPCSTR section) : inherited(section){}
	INHERIT_DYNAMIC_ALIFE;
};

template <typename T>
struct CWrapperAbstractZone : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperAbstractZone<T>		self_type;
	CWrapperAbstractZone				(LPCSTR section) : inherited(section){}
	INHERIT_ZONE;
};

template <typename T>
struct CWrapperAbstractCreature : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperAbstractCreature<T>	self_type;
	CWrapperAbstractCreature			(LPCSTR section) : inherited(section){}
	INHERIT_CREATURE;
};

template <typename T>
struct CWrapperAbstractMonster : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperAbstractMonster<T>	self_type;
	CWrapperAbstractMonster				(LPCSTR section) : inherited(section){}
	INHERIT_MONSTER;
};

template <typename T>
struct CWrapperAbstractItem : public T, public luabind::wrap_base {
	typedef T							inherited;
	typedef CWrapperAbstractItem<T>		self_type;
	CWrapperAbstractItem				(LPCSTR section) : inherited(section){}
	INHERIT_ITEM;
};

#define luabind_virtual_pure(a,b) \
	.def(	constructor<LPCSTR>())

//#ifndef USE_WRITER_READER
//#	define luabind_virtual_pure(a,b) \
//		.def(	constructor<LPCSTR>()) \
//		DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,save,void,NET_Packet&,NET_Packet*) \
//		DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,load,void,NET_Packet&,NET_Packet*) 
//#else
//#	define luabind_virtual_pure(a,b) \
//		.def(	constructor<LPCSTR>()) \
//		DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,save,void,NET_Packet&,NET_Packet*) \
//		DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,load,void,NET_Packet&,NET_Packet*) \
//		DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,save,void,IWriter&,IWriter*) \
//		DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_1(a,b,load,void,IReader&,IReader*)
//#endif

#define luabind_virtual_abstract(a,b) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,FillProps	) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,STATE_Write	) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,STATE_Read	) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,OnEvent		) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,init		)

#define luabind_virtual_alife(a,b) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_CONST_0(a,b,can_switch_online,bool) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_CONST_0(a,b,can_switch_offline,bool) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION_EXPLICIT_CONST_0(a,b,interactive,bool) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,used_ai_locations	) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,can_save			) \

#ifdef XRGAME_EXPORTS
#	define luabind_virtual_dynamic_alife(a,b) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,on_spawn			) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,on_before_register	) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,on_register			) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,on_unregister		)
#else
#	define luabind_virtual_dynamic_alife(a,b)
#endif

#ifdef XRGAME_EXPORTS
#	define luabind_virtual_creature(a,b) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,g_team	) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,g_squad	) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,g_group	) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,on_death)
#else // XRGAME_EXPORTS
#	define luabind_virtual_creature(a,b) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,g_team	) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,g_squad	) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,g_group	)
#endif // XRGAME_EXPORTS

#ifdef XRGAME_EXPORTS
#	define luabind_virtual_zone(a,b) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,update)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,smart_touch)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,enabled)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,suitable)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,register_npc)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,unregister_npc)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,task)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,detect_probability)
#else
#	define luabind_virtual_zone(a,b) \
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,update)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,smart_touch)\
		DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,detect_probability)
#endif

#define luabind_virtual_monster(a,b) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,update)

#define luabind_virtual_item(a,b) \
	DEFINE_LUABIND_VIRTUAL_FUNCTION(a,b,bfUseful)

#define luabind_virtual_Pure(a,b) \
	luabind_virtual_pure(a,b)

#define luabind_virtual_Abstract(a,b) \
	luabind_virtual_Pure(a,b) \
	luabind_virtual_abstract(a,b)

#define luabind_virtual_Alife(a,b) \
	luabind_virtual_Abstract(a,b) \
	luabind_virtual_alife(a,b)

#define luabind_virtual_DynamicAlife(a,b) \
	luabind_virtual_Alife(a,b) \
	luabind_virtual_dynamic_alife(a,b)

#define luabind_virtual_Creature(a,b) \
	luabind_virtual_DynamicAlife(a,b) \
	luabind_virtual_creature(a,b)

#define luabind_virtual_Zone(a,b) \
	luabind_virtual_DynamicAlife(a,b) \
	luabind_virtual_zone(a,b)

#define luabind_virtual_Monster(a,b) \
	luabind_virtual_Creature(a,b) \
	luabind_virtual_monster(a,b)

#define luabind_virtual_Item(a,b) \
	luabind_virtual_DynamicAlife(a,b) \
	luabind_virtual_item(a,b)

//////////////////////////////////////////////////////////////////////////
// 0
//////////////////////////////////////////////////////////////////////////
#define luabind_class_pure0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperPure<a>,b) \
	luabind_virtual_Pure(a,CWrapperPure<a>)

#define luabind_class_abstract0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstract<a>,b) \
	luabind_virtual_Abstract(a,CWrapperAbstract<a>)

#define luabind_class_alife0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstractALife<a>,b) \
	luabind_virtual_Alife(a,CWrapperAbstractALife<a>)

#define luabind_class_dynamic_alife0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstractDynamicALife<a>,b) \
	luabind_virtual_DynamicAlife(a,CWrapperAbstractDynamicALife<a>)

#define luabind_class_zone0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstractZone<a>,b) \
	luabind_virtual_Zone(a,CWrapperAbstractZone<a>)

#define luabind_class_creature0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstractCreature<a>,b) \
	luabind_virtual_Creature(a,CWrapperAbstractCreature<a>)

#define luabind_class_monster0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstractMonster<a>,b) \
	luabind_virtual_Monster(a,CWrapperAbstractMonster<a>)

#define luabind_class_item0(a,b) \
	DEFINE_LUABIND_CLASS_WRAPPER_0(a,CWrapperAbstractItem<a>,b) \
	luabind_virtual_Item(a,CWrapperAbstractItem<a>)

//////////////////////////////////////////////////////////////////////////
// 1
//////////////////////////////////////////////////////////////////////////
#define luabind_class_pure1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperPure<a>,b,c) \
	luabind_virtual_Pure(a,CWrapperPure<a>)

#define luabind_class_abstract1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstract<a>,b,c) \
	luabind_virtual_Abstract(a,CWrapperAbstract<a>)

#define luabind_class_alife1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstractALife<a>,b,c) \
	luabind_virtual_Alife(a,CWrapperAbstractALife<a>)

#define luabind_class_dynamic_alife1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstractDynamicALife<a>,b,c) \
	luabind_virtual_DynamicAlife(a,CWrapperAbstractDynamicALife<a>)

#define luabind_class_zone1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstractZone<a>,b,c) \
	luabind_virtual_Zone(a,CWrapperAbstractZone<a>)

#define luabind_class_creature1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstractCreature<a>,b,c) \
	luabind_virtual_Creature(a,CWrapperAbstractCreature<a>)

#define luabind_class_monster1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstractMonster<a>,b,c) \
	luabind_virtual_Monster(a,CWrapperAbstractMonster<a>)

#define luabind_class_item1(a,b,c) \
	DEFINE_LUABIND_CLASS_WRAPPER_1(a,CWrapperAbstractItem<a>,b,c) \
	luabind_virtual_Item(a,CWrapperAbstractItem<a>)

//////////////////////////////////////////////////////////////////////////
// 2
//////////////////////////////////////////////////////////////////////////
#define luabind_class_pure2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperPure<a>,b,c,d) \
	luabind_virtual_Pure(a,CWrapperPure<a>)

#define luabind_class_abstract2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstract<a>,b,c,d) \
	luabind_virtual_Abstract(a,CWrapperAbstract<a>)

#define luabind_class_alife2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstractALife<a>,b,c,d) \
	luabind_virtual_Alife(a,CWrapperAbstractALife<a>)

#define luabind_class_dynamic_alife2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstractDynamicALife<a>,b,c,d) \
	luabind_virtual_DynamicAlife(a,CWrapperAbstractDynamicALife<a>)

#define luabind_class_zone2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstractZone<a>,b,c,d) \
	luabind_virtual_Zone(a,CWrapperAbstractZone<a>)

#define luabind_class_creature2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstractCreature<a>,b,c,d) \
	luabind_virtual_Creature(a,CWrapperAbstractCreature<a>)

#define luabind_class_monster2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstractMonster<a>,b,c,d) \
	luabind_virtual_Monster(a,CWrapperAbstractMonster<a>)

#define luabind_class_item2(a,b,c,d) \
	DEFINE_LUABIND_CLASS_WRAPPER_2(a,CWrapperAbstractItem<a>,b,c,d) \
	luabind_virtual_Item(a,CWrapperAbstractItem<a>)

//////////////////////////////////////////////////////////////////////////
// 3
//////////////////////////////////////////////////////////////////////////
#define luabind_class_pure3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperPure<a>,b,c,d,e) \
	luabind_virtual_Pure(a,CWrapperPure<a>)

#define luabind_class_abstract3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstract<a>,b,c,d,e) \
	luabind_virtual_Abstract(a,CWrapperAbstract<a>)

#define luabind_class_alife3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstractALife<a>,b,c,d,e) \
	luabind_virtual_Alife(a,CWrapperAbstractALife<a>)

#define luabind_class_dynamic_alife3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstractDynamicALife<a>,b,c,d,e) \
	luabind_virtual_DynamicAlife(a,CWrapperAbstractDynamicALife<a>)

#define luabind_class_zone3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstractZone<a>,b,c,d,e) \
	luabind_virtual_Zone(a,CWrapperAbstractZone<a>)

#define luabind_class_creature3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstractCreature<a>,b,c,d,e) \
	luabind_virtual_Creature(a,CWrapperAbstractCreature<a>)

#define luabind_class_monster3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstractMonster<a>,b,c,d,e) \
	luabind_virtual_Monster(a,CWrapperAbstractMonster<a>)

#define luabind_class_item3(a,b,c,d,e) \
	DEFINE_LUABIND_CLASS_WRAPPER_3(a,CWrapperAbstractItem<a>,b,c,d,e) \
	luabind_virtual_Item(a,CWrapperAbstractItem<a>)

//////////////////////////////////////////////////////////////////////////
// 4
//////////////////////////////////////////////////////////////////////////
#define luabind_class_pure4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperPure<a>,b,c,d,e,f) \
	luabind_virtual_Pure(a,CWrapperPure<a>)

#define luabind_class_abstract4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstract<a>,b,c,d,e,f) \
	luabind_virtual_Abstract(a,CWrapperAbstract<a>)

#define luabind_class_alife4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstractALife<a>,b,c,d,e,f) \
	luabind_virtual_Alife(a,CWrapperAbstractALife<a>)

#define luabind_class_dynamic_alife4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstractDynamicALife<a>,b,c,d,e,f) \
	luabind_virtual_DynamicAlife(a,CWrapperAbstractDynamicALife<a>)

#define luabind_class_zone4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstractZone<a>,b,c,d,e,f) \
	luabind_virtual_Zone(a,CWrapperAbstractZone<a>)

#define luabind_class_creature4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstractCreature<a>,b,c,d,e,f) \
	luabind_virtual_Creature(a,CWrapperAbstractCreature<a>)

#define luabind_class_monster4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstractMonster<a>,b,c,d,e,f) \
	luabind_virtual_Monster(a,CWrapperAbstractMonster<a>)

#define luabind_class_item4(a,b,c,d,e,f) \
	DEFINE_LUABIND_CLASS_WRAPPER_4(a,CWrapperAbstractItem<a>,b,c,d,e,f) \
	luabind_virtual_Item(a,CWrapperAbstractItem<a>)

