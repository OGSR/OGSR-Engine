////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects monsters for ALife simulator
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../../xrNetServer/net_utils.h"
#include "xrServer_Objects_ALife_Items.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "object_broker.h"
#include "alife_human_brain.h"

#ifndef AI_COMPILER
#	include "ai_space.h"
#	include "character_info.h"
#	include "specific_character.h"
#endif

#ifdef XRGAME_EXPORTS
#	include "ef_storage.h"
#	include "game_graph.h"
#	include "alife_simulator.h"
#	include "alife_registry_container.h"
#	include "ef_primary.h"
#	include "string_table.h"
#	include "alife_online_offline_group_brain.h"
#	include "alife_simulator.h"
#	include "alife_object_registry.h"
#	include "date_time.h"
#endif

void setup_location_types_section(GameGraph::TERRAIN_VECTOR &m_vertex_types, CInifile *ini, LPCSTR section)
{
	VERIFY3							(ini->section_exist(section),"cannot open section",section);
	GameGraph::STerrainPlace		terrain_mask;
	terrain_mask.tMask.resize		(GameGraph::LOCATION_TYPE_COUNT);

	CInifile::Sect& sect			= ini->r_section(section);
	CInifile::SectCIt				I = sect.Data.begin();
	CInifile::SectCIt				E = sect.Data.end();
	for ( ; I != E; ++I) {
		LPCSTR						S = *(*I).first;
		string16					I;
		u32							N = _GetItemCount(S);
		
		if (N != GameGraph::LOCATION_TYPE_COUNT)
			continue;

		for (u32 j=0; j<GameGraph::LOCATION_TYPE_COUNT; ++j)
			terrain_mask.tMask[j]	= GameGraph::_LOCATION_ID(atoi(_GetItem(S,j,I)));
		
		m_vertex_types.push_back	(terrain_mask);
	}
	
	if (!m_vertex_types.empty())
		return;

	for (u32 j=0; j<GameGraph::LOCATION_TYPE_COUNT; ++j)
		terrain_mask.tMask[j]		= 255;
	
	m_vertex_types.push_back		(terrain_mask);
}

void setup_location_types_line(GameGraph::TERRAIN_VECTOR &m_vertex_types, LPCSTR string)
{
	string16						I;
	GameGraph::STerrainPlace		terrain_mask;
	terrain_mask.tMask.resize		(GameGraph::LOCATION_TYPE_COUNT);
	
	u32								N = _GetItemCount(string)/GameGraph::LOCATION_TYPE_COUNT*GameGraph::LOCATION_TYPE_COUNT;
	
	if (!N) {
		for (u32 j=0; j<GameGraph::LOCATION_TYPE_COUNT; ++j)
			terrain_mask.tMask[j]	= 255;
		m_vertex_types.push_back	(terrain_mask);
		return;
	}

	m_vertex_types.reserve			(32);

	for (u32 i=0; i<N;) {
		for (u32 j=0; j<GameGraph::LOCATION_TYPE_COUNT; ++j, ++i)
			terrain_mask.tMask[j]	= GameGraph::_LOCATION_ID(atoi(_GetItem(string,i,I)));
		m_vertex_types.push_back	(terrain_mask);
	}
}

void setup_location_types(GameGraph::TERRAIN_VECTOR &m_vertex_types, CInifile *ini, LPCSTR string)
{
	m_vertex_types.clear			();
	if (ini->section_exist(string) && ini->line_count(string))
		setup_location_types_section(m_vertex_types,ini,string);
	else 
		setup_location_types_line	(m_vertex_types,string);
}

//////////////////////////////////////////////////////////////////////////

//возможное отклонение от значения репутации
//заданого в профиле и для конкретного персонажа
#define REPUTATION_DELTA	10
#define RANK_DELTA			10


//////////////////////////////////////////////////////////////////////////

using namespace ALife;

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTraderAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeTraderAbstract::CSE_ALifeTraderAbstract(LPCSTR caSection)
{
//	m_fCumulativeItemMass		= 0.f;
//	m_iCumulativeItemVolume		= 0;
	m_dwMoney					= 0;
	if (pSettings->line_exist(caSection, "money"))
		m_dwMoney 				= pSettings->r_u32(caSection, "money");
	m_fMaxItemMass				= pSettings->r_float(caSection, "max_item_mass");

	m_sCharacterProfile			= READ_IF_EXISTS(pSettings,r_string,caSection,"character_profile","default");
	m_SpecificCharacter			= NULL;

#ifdef XRGAME_EXPORTS
	m_community_index			= NO_COMMUNITY_INDEX;
	m_rank						= NO_RANK;
	m_reputation				= NO_REPUTATION;
#endif

	m_trader_flags.zero			();
	m_trader_flags.set			(eTraderFlagInfiniteAmmo,FALSE);
}

CSE_Abstract *CSE_ALifeTraderAbstract::init	()
{
	string4096					S;
	//sprintf						(S,"%s\r\n[game_info]\r\nname_id = default\r\n",!*base()->m_ini_string ? "" : *base()->m_ini_string);
	sprintf						(S,"%s\r\n[game_info]\r\n",!*base()->m_ini_string ? "" : *base()->m_ini_string);
	base()->m_ini_string		= S;

	return						(base());
}

CSE_ALifeTraderAbstract::~CSE_ALifeTraderAbstract()
{
}

void CSE_ALifeTraderAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	tNetPacket.w_u32			(m_dwMoney);

#ifdef XRGAME_EXPORTS
	tNetPacket.w_stringZ		(m_SpecificCharacter);
#else
	shared_str s;
	tNetPacket.w_stringZ		(s);
#endif
	tNetPacket.w_u32			(m_trader_flags.get());
//	tNetPacket.w_s32			(m_iCharacterProfile);
	tNetPacket.w_stringZ		(m_sCharacterProfile);

#ifdef XRGAME_EXPORTS
	tNetPacket.w_s32			(m_community_index);
	tNetPacket.w_s32			(m_rank);
	tNetPacket.w_s32			(m_reputation);
#else
	tNetPacket.w_s32			(NO_COMMUNITY_INDEX);
	tNetPacket.w_s32			(NO_RANK);
	tNetPacket.w_s32			(NO_REPUTATION);
#endif
	save_data					(m_character_name, tNetPacket);
}

void CSE_ALifeTraderAbstract::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	u16 m_wVersion = base()->m_wVersion;
	if (m_wVersion > 19) {
		if (m_wVersion < 108) {
			R_ASSERT			(!tNetPacket.r_u32());
		}

		if (m_wVersion < 36) {
			xr_vector<u16>		temp;
			load_data			(temp,tNetPacket);
		}

		if (m_wVersion > 62)
			tNetPacket.r_u32	(m_dwMoney);

		if ((m_wVersion > 75) && (m_wVersion < 98)){
			int tmp;
			tNetPacket.r_s32	(tmp);
#ifndef AI_COMPILER
			if(tmp!=-1)
				m_SpecificCharacter = CSpecificCharacter::IndexToId(tmp);
			else
				m_SpecificCharacter = NULL;

#else
			m_SpecificCharacter = NULL;
#endif
		}else if (m_wVersion>=98){
			tNetPacket.r_stringZ(m_SpecificCharacter);
		}

		if (m_wVersion > 77)
			m_trader_flags.assign(tNetPacket.r_u32());

		if ((m_wVersion > 81) && (m_wVersion < 96)) {
			int tmp;		
			tNetPacket.r_s32	(tmp);
#ifndef AI_COMPILER
			m_sCharacterProfile	= CCharacterInfo::IndexToId(tmp);
#else
			m_sCharacterProfile	= "default";
#endif
			VERIFY				(xr_strlen(m_sCharacterProfile));
		}
		else
			if(m_wVersion > 95)
				tNetPacket.r_stringZ	(m_sCharacterProfile);

		if (m_wVersion > 85)
			tNetPacket.r_s32	(m_community_index);
		if (m_wVersion > 86) {
			tNetPacket.r_s32	(m_rank);
			tNetPacket.r_s32	(m_reputation);
		}

		if (m_wVersion > 104) {
			load_data			(m_character_name, tNetPacket);
		}
	}

#ifdef XRGAME_EXPORTS
	specific_character			();
#endif

}

void CSE_ALifeTraderAbstract::OnChangeProfile(PropValue* sender)
{
	m_SpecificCharacter = NULL;
#ifndef AI_COMPILER
	specific_character();
#endif
	base()->set_editor_flag		(ISE_Abstract::flVisualChange);
}

#ifndef AI_COMPILER


#ifdef XRGAME_EXPORTS

#include "game_base_space.h"
#include "Level.h"

#endif

shared_str CSE_ALifeTraderAbstract::specific_character()
{
#ifdef XRGAME_EXPORTS
#pragma todo("Dima to Yura, MadMax : Remove that hacks, please!")
	if (g_pGameLevel && Level().game && (GameID() != GAME_SINGLE)) return m_SpecificCharacter;
#endif

	if(m_SpecificCharacter.size())
		return m_SpecificCharacter;


	CCharacterInfo char_info;
	char_info.Load(character_profile());


	//профиль задан индексом
	if(char_info.data()->m_CharacterId.size() )
	{
		set_specific_character(char_info.data()->m_CharacterId);
		return m_SpecificCharacter;
	}
	//профиль задан шаблоном
	//
	//проверяем все информации о персонаже, запоминаем подходящие,
	//а потом делаем случайный выбор
	else
	{	
		m_CheckedCharacters.clear();
		m_DefaultCharacters.clear();

		for(int i=0; i<=CSpecificCharacter::GetMaxIndex(); i++)
		{
			CSpecificCharacter spec_char;
			shared_str id = CSpecificCharacter::IndexToId(i);
			spec_char.Load(id);

			if(spec_char.data()->m_bNoRandom) continue;

			bool class_found = false;
			for(std::size_t j=0; j<spec_char.data()->m_Classes.size(); j++)
			{
				if(char_info.data()->m_Class == spec_char.data()->m_Classes[j])
				{
					class_found = true;
					break;
				}
			}
			if(!char_info.data()->m_Class.size() || class_found)
			{
				//запомнить пподходящий персонаж с флажком m_bDefaultForCommunity
				if(spec_char.data()->m_bDefaultForCommunity)
					m_DefaultCharacters.push_back(id);

				if(char_info.data()->m_Rank == NO_RANK || _abs(spec_char.Rank() - char_info.data()->m_Rank)<RANK_DELTA)
				{
					if(char_info.data()->m_Reputation == NO_REPUTATION || _abs(spec_char.Reputation() - char_info.data()->m_Reputation)<REPUTATION_DELTA)
					{
#ifdef XRGAME_EXPORTS
						int* count = NULL;
						if(ai().get_alife())
							count = ai().alife().registry(specific_characters).object(id, true);
						//если индекс еще не был использован
						if(NULL == count)
#endif
							m_CheckedCharacters.push_back(id);
					}
				}
			}
		}
		R_ASSERT3(!m_DefaultCharacters.empty(), 
			"no default specific character set for class", *char_info.data()->m_Class);

#ifdef XRGAME_EXPORTS
		if(m_CheckedCharacters.empty())
			char_info.m_SpecificCharacterId = m_DefaultCharacters[Random.randI(m_DefaultCharacters.size())];
		else
			char_info.m_SpecificCharacterId = m_CheckedCharacters[Random.randI(m_CheckedCharacters.size())];
#else
			char_info.m_SpecificCharacterId = m_DefaultCharacters[Random.randI(m_DefaultCharacters.size())];
#endif

		set_specific_character(char_info.m_SpecificCharacterId);
		return m_SpecificCharacter;
	}
}

void CSE_ALifeTraderAbstract::set_specific_character	(shared_str new_spec_char)
{
	R_ASSERT(new_spec_char.size());

#ifdef XRGAME_EXPORTS
	//убрать предыдущий номер из реестра
	if ( m_SpecificCharacter.size() ) 
	{
		if(ai().get_alife())
			ai().alife().registry(specific_characters).remove(m_SpecificCharacter, true);
	}
#endif
	m_SpecificCharacter = new_spec_char;


#ifdef XRGAME_EXPORTS
	if(ai().get_alife())
	{
		//запомнить, то что мы использовали индекс
		int a = 1;
		ai().alife().registry(specific_characters).add(m_SpecificCharacter, a, true);
	}
#endif

	CSpecificCharacter selected_char;
	selected_char.Load(m_SpecificCharacter);
	if(selected_char.Visual())
	{
		CSE_Visual* visual = smart_cast<CSE_Visual*>(base()); VERIFY(visual);
		if(xr_strlen(selected_char.Visual())>0)
			visual->set_visual(selected_char.Visual());
	}

#ifdef XRGAME_EXPORTS

	if(NO_COMMUNITY_INDEX == m_community_index)
	{
		m_community_index = selected_char.Community().index();
		CSE_ALifeCreatureAbstract* creature = smart_cast<CSE_ALifeCreatureAbstract*>(base());
		if (creature)
			creature->s_team = selected_char.Community().team();
	}



//----
	CSE_ALifeMonsterAbstract* monster = smart_cast<CSE_ALifeMonsterAbstract*>(base());
	if(monster&&selected_char.terrain_sect().size()){
		setup_location_types_section	(monster->m_tpaTerrain, pSettings, *(selected_char.terrain_sect()));
	}
//----
	if(NO_RANK == m_rank)
		m_rank = selected_char.Rank();

	if(NO_REPUTATION == m_reputation)
		m_reputation = selected_char.Reputation();

	m_character_name = *(CStringTable().translate(selected_char.Name()));
	
	LPCSTR gen_name = "GENERATE_NAME_";
	if( strstr(m_character_name.c_str(),gen_name) ){
		//select name and lastname
		xr_string subset			= m_character_name.c_str()+xr_strlen(gen_name);

		string32					t1;
		strconcat					(t1,"stalker_names_",subset.c_str());
		u32 name_cnt				= pSettings->r_u32(t1, "name_cnt");
		u32 last_name_cnt			= pSettings->r_u32(t1, "last_name_cnt");
		
		string512			S;
		xr_string n			= "name_";
		n					+= subset;
		n					+= "_";
		n					+= itoa(::Random.randI(name_cnt),S,10);
		m_character_name	= *(CStringTable().translate(n.c_str()));
		m_character_name	+= " ";

		n					= "lname_";
		n					+= subset;
		n					+= "_";
		n					+= itoa(::Random.randI(last_name_cnt),S,10);
		m_character_name	+= *(CStringTable().translate(n.c_str()));


	
	}
	u32 min_m = selected_char.MoneyDef().min_money;
	u32 max_m = selected_char.MoneyDef().max_money;
	if(min_m!=0 && max_m!=0){
		m_dwMoney = min_m;
		if(min_m!=max_m)	m_dwMoney += ::Random.randI(max_m-min_m);
	}
#else
	//в редакторе специфический профиль оставляем не заполненым
	m_SpecificCharacter = NULL;
#endif
}

void CSE_ALifeTraderAbstract::set_character_profile(shared_str new_profile)
{
	m_sCharacterProfile = new_profile;
}

shared_str CSE_ALifeTraderAbstract::character_profile()
{
	return	m_sCharacterProfile;
}

#endif


#ifdef XRGAME_EXPORTS

//для работы с relation system
u16								CSE_ALifeTraderAbstract::object_id		() const
{
	return base()->ID;
}

CHARACTER_COMMUNITY_INDEX		CSE_ALifeTraderAbstract::Community	() const
{
	return m_community_index;
}

LPCSTR			CSE_ALifeTraderAbstract::CommunityName () const
{
	return *CHARACTER_COMMUNITY::IndexToId(m_community_index);
}

CHARACTER_RANK_VALUE CSE_ALifeTraderAbstract::Rank		()
{
	specific_character();
	return m_rank;
}

void CSE_ALifeTraderAbstract::SetRank(CHARACTER_RANK_VALUE val)
{
	specific_character();
	m_rank = val;
}

CHARACTER_REPUTATION_VALUE	CSE_ALifeTraderAbstract::Reputation ()
{
	specific_character();
	return m_reputation;
}

#endif

void CSE_ALifeTraderAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
};

void CSE_ALifeTraderAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
};


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeTrader
////////////////////////////////////////////////////////////////////////////

CSE_ALifeTrader::CSE_ALifeTrader			(LPCSTR caSection) : CSE_ALifeDynamicObjectVisual(caSection), CSE_ALifeTraderAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
}

CSE_ALifeTrader::~CSE_ALifeTrader			()
{
}

#ifdef DEBUG
bool CSE_ALifeTrader::match_configuration	() const
{
	return						(!strstr(Core.Params,"-designer"));
}
#endif

CSE_Abstract *CSE_ALifeTrader::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(base());
}

CSE_Abstract *CSE_ALifeTrader::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeTrader::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeTrader::STATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeTrader::STATE_Read			(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	inherited2::STATE_Read		(tNetPacket, size);
	if ((m_wVersion > 35) && (m_wVersion < 118))
		tNetPacket.r_u32		();
		
	if ((m_wVersion > 29) && (m_wVersion < 118)) {
		u32						l_dwCount	= tNetPacket.r_u32();
		for (int i=0 ; i<(int)l_dwCount; ++i) {
			shared_str			temp;
			tNetPacket.r_stringZ(temp);
			tNetPacket.r_u32	();
			for (int i=0, n=tNetPacket.r_u32(); i<n; ++i) {
				tNetPacket.r_stringZ(temp);
				tNetPacket.r_u32	();
				tNetPacket.r_u32	();
			}
		}
	}

	if ((m_wVersion > 30) && (m_wVersion < 118)) {
		u32						count = tNetPacket.r_u32();
		shared_str				temp;
		for (u32 i=0; i<count; ++i) {
			tNetPacket.r_stringZ(temp);
			tNetPacket.r_u32	();
			tNetPacket.r_float	();
			tNetPacket.r_float	();
		}
	}
}

void CSE_ALifeTrader::UPDATE_Write			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeTrader::UPDATE_Read			(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
};

bool CSE_ALifeTrader::interactive			() const
{
	return						(false);
}

void CSE_ALifeTrader::FillProps				(LPCSTR _pref, PropItemVec& items)
{
	inherited1::FillProps		(_pref,items);
	inherited2::FillProps		(_pref,items);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCustomZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCustomZone::CSE_ALifeCustomZone	(LPCSTR caSection) : CSE_ALifeSpaceRestrictor(caSection)
{
	m_owner_id					= u32(-1);
	m_maxPower					= pSettings->r_float(caSection,"min_start_power");
	if (pSettings->line_exist(caSection,"hit_type"))
		m_tHitType				= ALife::g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	else
		m_tHitType				= ALife::eHitTypeMax;
	m_enabled_time				= 0;
	m_disabled_time				= 0;
	m_start_time_shift			= 0;

}

CSE_ALifeCustomZone::~CSE_ALifeCustomZone	()
{
}

void CSE_ALifeCustomZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
	
	tNetPacket.r_float			(m_maxPower);

	if (m_wVersion < 113) {
		tNetPacket.r_float		();
		tNetPacket.r_u32		();
	}

	if ((m_wVersion > 66) && (m_wVersion < 118)) {
		tNetPacket.r_u32		();
	}

	if(m_wVersion > 102)
		tNetPacket.r_u32		(m_owner_id);

	if (m_wVersion > 105) {
		tNetPacket.r_u32		(m_enabled_time);
		tNetPacket.r_u32		(m_disabled_time);
	}
	if (m_wVersion > 106) {
		tNetPacket.r_u32		(m_start_time_shift);
	}

}

void CSE_ALifeCustomZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(m_maxPower);
	tNetPacket.w_u32			(m_owner_id);
	tNetPacket.w_u32			(m_enabled_time);
	tNetPacket.w_u32			(m_disabled_time);
	tNetPacket.w_u32			(m_start_time_shift);
}

void CSE_ALifeCustomZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
//	tNetPacket.r_u32			(m_owner_id);
}

void CSE_ALifeCustomZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
//	tNetPacket.w_u32			(m_owner_id);
}

//xr_token TokenAnomalyType[]={
//	{ "Gravi",			eAnomalousZoneTypeGravi			},
//	{ "Fog",			eAnomalousZoneTypeFog			},
//	{ "Radioactive",	eAnomalousZoneTypeRadio			},
//	{ "Plant",			eAnomalousZoneTypePlant			},
//	{ "Gelatine",		eAnomalousZoneTypeGelatine		},
//	{ "Fluff",			eAnomalousZoneTypeFluff			},
//	{ "Rusty Hair",		eAnomalousZoneTypeRustyHair		},
//	{ "RustyWhistlers",	eAnomalousZoneTypeRustyWhistlers},
//	{ 0,				0}
//};

void CSE_ALifeCustomZone::FillProps		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProps		(pref,items);
	PHelper().CreateU32			(items,PrepareKey(pref,*s_name,"on/off mode\\Shift time (sec)"),	&m_start_time_shift,0,100000);
	PHelper().CreateU32			(items,PrepareKey(pref,*s_name,"on/off mode\\Enabled time (sec)"),	&m_enabled_time,	0,100000);
	PHelper().CreateU32			(items,PrepareKey(pref,*s_name,"on/off mode\\Disabled time (sec)"),	&m_disabled_time,	0,100000);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeAnomalousZone
////////////////////////////////////////////////////////////////////////////
CSE_ALifeAnomalousZone::CSE_ALifeAnomalousZone(LPCSTR caSection) : CSE_ALifeCustomZone(caSection)
{
	m_offline_interactive_radius	= 30.f;
	m_artefact_spawn_count			= 32;
	m_spawn_flags.set				(flSpawnDestroyOnSpawn,TRUE);
}

CSE_Abstract *CSE_ALifeAnomalousZone::init			()
{
	inherited::init				();
	return						(base());
}

CSE_Abstract *CSE_ALifeAnomalousZone::base			()
{
	return						(inherited::base());
}

const CSE_Abstract *CSE_ALifeAnomalousZone::base	() const
{
	return						(inherited::base());
}

CSE_ALifeAnomalousZone::~CSE_ALifeAnomalousZone		()
{
}

u32	CSE_ALifeAnomalousZone::ef_anomaly_type			() const
{
	return						(pSettings->r_u32(name(),"ef_anomaly_type"));
}

u32	CSE_ALifeAnomalousZone::ef_weapon_type			() const
{
	return						(pSettings->r_u32(name(),"ef_weapon_type"));
}

u32	CSE_ALifeAnomalousZone::ef_creature_type		() const
{
	return						(inherited::ef_weapon_type());
}

void CSE_ALifeAnomalousZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);

	if (m_wVersion > 21) {
		tNetPacket.r_float		(m_offline_interactive_radius);
		if (m_wVersion < 113) {

			tNetPacket.r_float			();

			shared_str					temp;
			for (u16 i=0, n=tNetPacket.r_u16(); i<n; ++i) {
				tNetPacket.r_stringZ	(temp);
				
				if (m_wVersion > 26)
					tNetPacket.r_float	();
				else
					tNetPacket.r_u32	();
			}
		}
	}
	
	if (m_wVersion > 25) {
		tNetPacket.r_u16		(m_artefact_spawn_count);
		tNetPacket.r_u32		(m_artefact_position_offset);
	}

	if ((m_wVersion < 67) && (m_wVersion > 27)) {
		tNetPacket.r_u32		();
	}

	if ((m_wVersion > 38) && (m_wVersion < 113))
		tNetPacket.r_float		();

	if ((m_wVersion > 78) && (m_wVersion < 113)) {
		tNetPacket.r_float		();
		tNetPacket.r_float		();
		tNetPacket.r_float		();
	}
	if( (m_wVersion == 102) ){ //fuck
		u32 dummy;
		tNetPacket.r_u32		(dummy);
	}

}

void CSE_ALifeAnomalousZone::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_float			(m_offline_interactive_radius);
	tNetPacket.w_u16			(m_artefact_spawn_count);
	tNetPacket.w_u32			(m_artefact_position_offset);
}

void CSE_ALifeAnomalousZone::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeAnomalousZone::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write	(tNetPacket);
}

void CSE_ALifeAnomalousZone::FillProps		(LPCSTR pref, PropItemVec& items)
{
	inherited::FillProps			(pref,items);
	PHelper().CreateFloat			(items,PrepareKey(pref,*s_name,"offline interactive radius"),			&m_offline_interactive_radius,	0.f,	100.f);
	PHelper().CreateU16				(items,PrepareKey(pref,*s_name,"ALife\\Artefact spawn places count"),	&m_artefact_spawn_count,		32,		256);
	PHelper().CreateFlag32			(items,PrepareKey(pref,*s_name,"ALife\\Visible for AI"),				&m_flags,						flVisibleForAI);
}

//////////////////////////////////////////////////////////////////////////
//SE_ALifeTorridZone
//////////////////////////////////////////////////////////////////////////
CSE_ALifeTorridZone::CSE_ALifeTorridZone	(LPCSTR caSection)
:CSE_ALifeCustomZone(caSection),CSE_Motion()
{
}

CSE_ALifeTorridZone::~CSE_ALifeTorridZone	()
{
}

CSE_Motion* CSE_ALifeTorridZone::motion		()
{
	return						(this);
}

void CSE_ALifeTorridZone::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	CSE_Motion::motion_read		(tNetPacket);
	set_editor_flag				(flMotionChange);
}

void CSE_ALifeTorridZone::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	CSE_Motion::motion_write	(tNetPacket);

}

void CSE_ALifeTorridZone::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeTorridZone::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

void CSE_ALifeTorridZone::FillProps(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps		(pref, values);
	inherited2::FillProps		(pref, values);
}

//////////////////////////////////////////////////////////////////////////
//CSE_ALifeZoneVisual
//////////////////////////////////////////////////////////////////////////
CSE_ALifeZoneVisual::CSE_ALifeZoneVisual	(LPCSTR caSection)
:CSE_ALifeAnomalousZone(caSection),CSE_Visual(caSection)
{
	if (pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
//	if(pSettings->line_exist(caSection,"blast_animation"))
//		attack_animation=pSettings->r_string(caSection,"blast_animation");
}

CSE_ALifeZoneVisual::~CSE_ALifeZoneVisual	()
{

}

CSE_Visual* CSE_ALifeZoneVisual::visual	()
{
	return		static_cast<CSE_Visual*>(this);
}
void CSE_ALifeZoneVisual::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	visual_read					(tNetPacket,m_wVersion);
	tNetPacket.r_stringZ(startup_animation);
	tNetPacket.r_stringZ(attack_animation);
}

void CSE_ALifeZoneVisual::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	visual_write				(tNetPacket);
	tNetPacket.w_stringZ(startup_animation);
	tNetPacket.w_stringZ(attack_animation);
}

void CSE_ALifeZoneVisual::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeZoneVisual::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

void CSE_ALifeZoneVisual::FillProps(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps		(pref, values);
	inherited2::FillProps		(pref, values);
	ISE_Abstract* abstract		= smart_cast<ISE_Abstract*>(this); VERIFY(abstract);
	PHelper().CreateChoose(values,	PrepareKey(pref,abstract->name(),"Attack animation"),	&attack_animation, smSkeletonAnims,0,(void*)*visual_name);
}
//-------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreatureAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCreatureAbstract::CSE_ALifeCreatureAbstract(LPCSTR caSection)	: CSE_ALifeDynamicObjectVisual(caSection)
{
	s_team = s_squad = s_group	= 0;
	o_model						= 0.f;
	o_torso.pitch				= 0.f;
	o_torso.yaw					= 0.f;
	o_torso.roll				= 0.f;
	fHealth						= 1;
	m_bDeathIsProcessed			= false;
	m_fAccuracy					= 25.f;
	m_fIntelligence				= 25.f;
	m_fMorale					= 100.f;
	m_ef_creature_type			= pSettings->r_u32(caSection,"ef_creature_type");
	m_ef_weapon_type			= READ_IF_EXISTS(pSettings,r_u32,caSection,"ef_weapon_type",u32(-1));
	m_ef_detector_type			= READ_IF_EXISTS(pSettings,r_u32,caSection,"ef_detector_type",u32(-1));
	m_killer_id					= ALife::_OBJECT_ID(-1);
	m_game_death_time			= 0;
}

CSE_ALifeCreatureAbstract::~CSE_ALifeCreatureAbstract()
{
}

#ifdef DEBUG
bool CSE_ALifeCreatureAbstract::match_configuration	() const
{
	return						(!strstr(Core.Params,"-designer"));
}
#endif

u32	CSE_ALifeCreatureAbstract::ef_creature_type	() const
{
	return						(m_ef_creature_type);
}

u32	 CSE_ALifeCreatureAbstract::ef_weapon_type	() const
{
	VERIFY						(m_ef_weapon_type != u32(-1));
	return						(m_ef_weapon_type);
}

u32	 CSE_ALifeCreatureAbstract::ef_detector_type() const
{
	VERIFY						(m_ef_detector_type != u32(-1));
	return						(m_ef_detector_type);
}

#ifdef XRGAME_EXPORTS
void CSE_ALifeCreatureAbstract::on_death		(CSE_Abstract *killer)
{
	VERIFY						(!m_game_death_time);
	m_game_death_time			= Level().GetGameTime();
	fHealth						= -1.f;
}
#endif // XRGAME_EXPORTS

void CSE_ALifeCreatureAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
	tNetPacket.w_u8				(s_team	);
	tNetPacket.w_u8				(s_squad);
	tNetPacket.w_u8				(s_group);
	tNetPacket.w_float			(fHealth);
	save_data					(m_dynamic_out_restrictions,tNetPacket);
	save_data					(m_dynamic_in_restrictions,tNetPacket);
	tNetPacket.w				(&m_killer_id,sizeof(m_killer_id));
	tNetPacket.w				(&m_game_death_time,sizeof(m_game_death_time));
}

void CSE_ALifeCreatureAbstract::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket, size);
	tNetPacket.r_u8				(s_team	);
	tNetPacket.r_u8				(s_squad);
	tNetPacket.r_u8				(s_group);
	if (m_wVersion > 18)
		tNetPacket.r_float		(fHealth);

	if (m_wVersion < 115)
		fHealth	/=100.0f;

	if (m_wVersion < 32)
		visual_read				(tNetPacket,m_wVersion);
	o_model						= o_torso.yaw;

	if (m_wVersion > 87) {
		load_data				(m_dynamic_out_restrictions,tNetPacket);
		load_data				(m_dynamic_in_restrictions,tNetPacket);
	}
	if (m_wVersion > 94)
		tNetPacket.r			(&m_killer_id,sizeof(m_killer_id));

	o_torso.pitch				= o_Angle.x;
	o_torso.yaw					= o_Angle.y;

	if (m_wVersion > 115)
		tNetPacket.r			(&m_game_death_time,sizeof(m_game_death_time));
}

void CSE_ALifeCreatureAbstract::UPDATE_Write(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
	
	tNetPacket.w_float			(fHealth);
	
	tNetPacket.w_u32			(timestamp		);
	tNetPacket.w_u8				(flags			);
	tNetPacket.w_vec3			(o_Position		);
	tNetPacket.w_float /*w_angle8*/			(o_model		);
	tNetPacket.w_float /*w_angle8*/			(o_torso.yaw	);
	tNetPacket.w_float /*w_angle8*/			(o_torso.pitch	);
	tNetPacket.w_float /*w_angle8*/			(o_torso.roll	);
	tNetPacket.w_u8				(s_team);
	tNetPacket.w_u8				(s_squad);
	tNetPacket.w_u8				(s_group);
};

void CSE_ALifeCreatureAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
	
	tNetPacket.r_float			(fHealth);
	
	tNetPacket.r_u32			(timestamp		);
	tNetPacket.r_u8				(flags			);
	tNetPacket.r_vec3			(o_Position		);
	tNetPacket.r_float /*r_angle8*/			(o_model		);
	tNetPacket.r_float /*r_angle8*/			(o_torso.yaw	);
	tNetPacket.r_float /*r_angle8*/			(o_torso.pitch	);
	tNetPacket.r_float /*r_angle8*/			(o_torso.roll	);
	
	tNetPacket.r_u8				(s_team);
	tNetPacket.r_u8				(s_squad);
	tNetPacket.r_u8				(s_group);
};

u8 CSE_ALifeCreatureAbstract::g_team		()
{
	return s_team;
}

u8 CSE_ALifeCreatureAbstract::g_squad		()
{
	return s_squad;
}

u8 CSE_ALifeCreatureAbstract::g_group		()
{
	return s_group;
}

void CSE_ALifeCreatureAbstract::FillProps	(LPCSTR pref, PropItemVec& items)
{
  	inherited::FillProps			(pref,items);
    PHelper().CreateU8				(items,PrepareKey(pref,*s_name, "Team"),		&s_team, 	0,64,1);
    PHelper().CreateU8				(items,PrepareKey(pref,*s_name, "Squad"),	&s_squad, 	0,64,1);
    PHelper().CreateU8				(items,PrepareKey(pref,*s_name, "Group"),	&s_group, 	0,64,1);
   	PHelper().CreateFloat			(items,PrepareKey(pref,*s_name,"Personal",	"Health" 				),&fHealth,	0,2,5);
}

bool CSE_ALifeCreatureAbstract::used_ai_locations	() const
{
	return						(true);
}

bool CSE_ALifeCreatureAbstract::can_switch_online	() const
{
	return						(inherited::can_switch_online());
}

bool CSE_ALifeCreatureAbstract::can_switch_offline	() const
{
	return						(inherited::can_switch_offline() && (g_Health() > 0.f));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterAbstract
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterAbstract::CSE_ALifeMonsterAbstract(LPCSTR caSection)	: CSE_ALifeCreatureAbstract(caSection), CSE_ALifeSchedulable(caSection)
{
	m_tNextGraphID				= m_tGraphID;
	m_tPrevGraphID				= m_tGraphID;
	m_fCurSpeed					= 0.0f;
	m_fDistanceFromPoint		= 0.0f;
	m_fDistanceToPoint			= 0.0f;

	m_fMaxHealthValue	 		= pSettings->r_float	(caSection,"MaxHealthValue");
	if (pSettings->line_exist(caSection,"hit_power")) {
		m_fHitPower				= pSettings->r_float(caSection,"hit_power");
		m_tHitType				= ALife::g_tfString2HitType(pSettings->r_string(caSection,"hit_type"));
	}
	else {
		m_fHitPower				= 0;
		m_tHitType				= ALife::eHitTypeMax;
	}

	{
		string64					S;
		m_fpImmunityFactors.resize	(ALife::eHitTypeMax);
		svector<float,ALife::eHitTypeMax>::iterator	B = m_fpImmunityFactors.begin(), I = B;
		svector<float,ALife::eHitTypeMax>::iterator	E = m_fpImmunityFactors.end();

		LPCSTR imm_section = caSection;
		if(pSettings->line_exist(caSection, "immunities_sect"))
			imm_section = pSettings->r_string(caSection, "immunities_sect");
		for ( ; I != E; ++I)
			*I					= READ_IF_EXISTS(pSettings,r_float,imm_section,strcat(strcpy(S,ALife::g_cafHitType2String(ALife::EHitType(I - B))),"_immunity"),1.f);
	}

	if (pSettings->line_exist(caSection,"retreat_threshold"))
		m_fRetreatThreshold		= pSettings->r_float(caSection,"retreat_threshold");
	else
		m_fRetreatThreshold		= 0.2f;

	m_fEyeRange					= pSettings->r_float(caSection,"eye_range");

	m_fGoingSpeed				= pSettings->r_float(caSection, "going_speed");
	m_fCurrentLevelGoingSpeed	= READ_IF_EXISTS(pSettings,r_float,caSection,"current_level_going_speed",m_fGoingSpeed);

	setup_location_types		(m_tpaTerrain,pSettings,pSettings->r_string(caSection,"terrain"));

	m_tpBestDetector			= this;

	m_brain						= 0;
	m_smart_terrain_id			= 0xffff;
	m_task_reached				= false;

	m_rank						= (pSettings->line_exist(caSection,"rank")) ? pSettings->r_s32(caSection,"rank") : 0;

#ifdef XRGAME_EXPORTS
	m_stay_after_death_time_interval	= generate_time(1,1,1,pSettings->r_u32("monsters_common","stay_after_death_time_interval"),0,0);
#endif // XRGAME_EXPORTS
}

CSE_ALifeMonsterAbstract::~CSE_ALifeMonsterAbstract()
{
	xr_delete					(m_brain);
}

CALifeMonsterBrain *CSE_ALifeMonsterAbstract::create_brain	()
{
	return						(xr_new<CALifeMonsterBrain>(this));
}

CSE_Abstract *CSE_ALifeMonsterAbstract::init			()
{
	inherited1::init			();
	inherited2::init			();
	
	if (spawn_ini().section_exist("alife") && spawn_ini().line_exist("alife","terrain"))
		setup_location_types	(m_tpaTerrain,&spawn_ini(),spawn_ini().r_string("alife","terrain"));

	m_brain						= create_brain();

	return						(base());
}

CSE_Abstract *CSE_ALifeMonsterAbstract::base			()
{
	return						(inherited1::base());
}

u32	CSE_ALifeMonsterAbstract::ef_creature_type			() const
{
	return						(inherited1::ef_creature_type());
}

u32	CSE_ALifeMonsterAbstract::ef_weapon_type			() const
{
	return						(inherited1::ef_weapon_type());
}

u32	CSE_ALifeMonsterAbstract::ef_detector_type			() const
{
	return						(inherited1::ef_detector_type());
}

const CSE_Abstract *CSE_ALifeMonsterAbstract::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeMonsterAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	tNetPacket.w_stringZ		(m_out_space_restrictors);
	tNetPacket.w_stringZ		(m_in_space_restrictors);
	tNetPacket.w				(&m_smart_terrain_id,sizeof(m_smart_terrain_id));
	tNetPacket.w				(&m_task_reached,sizeof(m_task_reached));
}

void CSE_ALifeMonsterAbstract::STATE_Read	(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	if (m_wVersion > 72) {
		tNetPacket.r_stringZ	(m_out_space_restrictors);
		if (m_wVersion > 73)
			tNetPacket.r_stringZ(m_in_space_restrictors);
	}

	if (m_wVersion > 111)
		tNetPacket.r			(&m_smart_terrain_id,sizeof(m_smart_terrain_id));

	if (m_wVersion > 113)
		tNetPacket.r			(&m_task_reached,sizeof(m_task_reached));

}

void CSE_ALifeMonsterAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	tNetPacket.w				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.w				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
//	tNetPacket.w				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
//	tNetPacket.w				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.w				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.w				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

void CSE_ALifeMonsterAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	tNetPacket.r				(&m_tNextGraphID,			sizeof(m_tNextGraphID));
	tNetPacket.r				(&m_tPrevGraphID,			sizeof(m_tPrevGraphID));
//	tNetPacket.r				(&m_fGoingSpeed,			sizeof(m_fGoingSpeed));
//	tNetPacket.r				(&m_fCurSpeed,				sizeof(m_fCurSpeed));
	tNetPacket.r				(&m_fDistanceFromPoint,		sizeof(m_fDistanceFromPoint));
	tNetPacket.r				(&m_fDistanceToPoint,		sizeof(m_fDistanceToPoint));
};

void CSE_ALifeMonsterAbstract::FillProps		(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProps		(pref,items);
	
	PHelper().CreateFlag32		(items,	PrepareKey(pref,*s_name,"ALife\\No move in offline"),	&m_flags,			flOfflineNoMove);
	PHelper().CreateFlag32		(items,	PrepareKey(pref,*s_name,"Use smart terrain tasks"),	&m_flags,			flUseSmartTerrains);

	if (pSettings->line_exist(s_name,"SpaceRestrictionSection")) {
		LPCSTR					gcs = pSettings->r_string(s_name,"SpaceRestrictionSection");
		PHelper().CreateChoose	(items, PrepareKey(pref,*s_name,"out space restrictions"),&m_out_space_restrictors, smSpawnItem, 0, (void*)gcs, 16);
		PHelper().CreateChoose	(items, PrepareKey(pref,*s_name,"in space restrictions"),&m_in_space_restrictors,  smSpawnItem, 0, (void*)gcs, 16);
	}
}

bool CSE_ALifeMonsterAbstract::need_update	(CSE_ALifeDynamicObject *object)
{
	return						(CSE_ALifeSchedulable::need_update(object) && (fHealth > EPS_L));
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreatureActor
////////////////////////////////////////////////////////////////////////////

CSE_ALifeCreatureActor::CSE_ALifeCreatureActor	(LPCSTR caSection) : CSE_ALifeCreatureAbstract(caSection), CSE_ALifeTraderAbstract(caSection),CSE_PHSkeleton(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	m_u16NumItems				= 0;
//	fArmor						= 0.f;
	fRadiation					= 0.f;
	accel.set					(0.f,0.f,0.f);
	velocity.set				(0.f,0.f,0.f);
	m_holderID					=u16(-1);
}

CSE_ALifeCreatureActor::~CSE_ALifeCreatureActor()
{
}

#ifdef DEBUG
bool CSE_ALifeCreatureActor::match_configuration	() const
{
	return						(true);
}
#endif

CSE_Abstract *CSE_ALifeCreatureActor::init			()
{
	inherited1::init			();
	inherited2::init			();
	return						(inherited1::base());
}

CSE_Abstract *CSE_ALifeCreatureActor::base			()
{
	return						(inherited1::base());
}

const CSE_Abstract *CSE_ALifeCreatureActor::base	() const
{
	return						(inherited1::base());
}

void CSE_ALifeCreatureActor::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion < 21) {
		CSE_ALifeDynamicObject::STATE_Read(tNetPacket,size);
		tNetPacket.r_u8			(s_team	);
		tNetPacket.r_u8			(s_squad);
		tNetPacket.r_u8			(s_group);
		if (m_wVersion > 18)
			tNetPacket.r_float	(fHealth);
		if (m_wVersion >= 3)
			visual_read			(tNetPacket,m_wVersion);
	}
	else {
		inherited1::STATE_Read	(tNetPacket,size);
		inherited2::STATE_Read	(tNetPacket,size);
		if (m_wVersion < 32)
			visual_read			(tNetPacket,m_wVersion);
	}
	if(m_wVersion>91)
	{
		inherited3::STATE_Read(tNetPacket,size);
	}
	if(m_wVersion>88)
	{
		m_holderID=tNetPacket.r_u16();
	}
};

void CSE_ALifeCreatureActor::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	inherited3::STATE_Write		(tNetPacket);
	tNetPacket.w_u16(m_holderID);
};

void CSE_ALifeCreatureActor::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited3::load(tNetPacket);
	m_holderID=tNetPacket.r_u16();
}
void CSE_ALifeCreatureActor::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	tNetPacket.r_u16			(mstate		);
	tNetPacket.r_sdir			(accel		);
	tNetPacket.r_sdir			(velocity	);
	tNetPacket.r_float			(fRadiation	);
	tNetPacket.r_u8				(weapon		);
	////////////////////////////////////////////////////
	tNetPacket.r_u16			(m_u16NumItems);

	if (!m_u16NumItems) return;

	if (m_u16NumItems == 1)
	{
		tNetPacket.r_u8					( *((u8*)&(m_AliveState.enabled)) );

		tNetPacket.r_vec3				( m_AliveState.angular_vel );
		tNetPacket.r_vec3				( m_AliveState.linear_vel );

		tNetPacket.r_vec3				( m_AliveState.force );
		tNetPacket.r_vec3				( m_AliveState.torque );

		tNetPacket.r_vec3				( m_AliveState.position );

		tNetPacket.r_float				( m_AliveState.quaternion.x );
		tNetPacket.r_float				( m_AliveState.quaternion.y );
		tNetPacket.r_float				( m_AliveState.quaternion.z );
		tNetPacket.r_float				( m_AliveState.quaternion.w );

		return;
	};	
	////////////// Import dead body ////////////////////
	Msg	("A mi ni hera tut ne chitaem (m_u16NumItems == %d)",m_u16NumItems);
	{
		m_BoneDataSize = tNetPacket.r_u8();
		u32 BodyDataSize = 24 + m_BoneDataSize*m_u16NumItems;
		tNetPacket.r(m_DeadBodyData, BodyDataSize);
	};
};
void CSE_ALifeCreatureActor::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
	tNetPacket.w_u16			(mstate		);
	tNetPacket.w_sdir			(accel		);
	tNetPacket.w_sdir			(velocity	);
	tNetPacket.w_float			(fRadiation	);
	tNetPacket.w_u8				(weapon		);
	////////////////////////////////////////////////////
	tNetPacket.w_u16			(m_u16NumItems);
	if (!m_u16NumItems) return;	

	if (m_u16NumItems == 1)
	{
		tNetPacket.w_u8					( m_AliveState.enabled );

		tNetPacket.w_vec3				( m_AliveState.angular_vel );
		tNetPacket.w_vec3				( m_AliveState.linear_vel );

		tNetPacket.w_vec3				( m_AliveState.force );
		tNetPacket.w_vec3				( m_AliveState.torque );

		tNetPacket.w_vec3				( m_AliveState.position );

		tNetPacket.w_float				( m_AliveState.quaternion.x );
		tNetPacket.w_float				( m_AliveState.quaternion.y );
		tNetPacket.w_float				( m_AliveState.quaternion.z );
		tNetPacket.w_float				( m_AliveState.quaternion.w );	

		return;
	};
	////////////// Export dead body ////////////////////
	{
		tNetPacket.w_u8(m_BoneDataSize);
		u32 BodyDataSize = 24 + m_BoneDataSize*m_u16NumItems;
		tNetPacket.w(m_DeadBodyData, BodyDataSize);
	};
}

void CSE_ALifeCreatureActor::FillProps		(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProps		(pref,items);
  	inherited2::FillProps		(pref,items);
}

#ifdef XRGAME_EXPORTS
void CSE_ALifeCreatureActor::spawn_supplies	()
{
	inherited1::spawn_supplies	();
	inherited2::spawn_supplies	();
}
#endif

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreatureCrow
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCreatureCrow::CSE_ALifeCreatureCrow(LPCSTR caSection) : CSE_ALifeCreatureAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
}

CSE_ALifeCreatureCrow::~CSE_ALifeCreatureCrow()
{
}

void CSE_ALifeCreatureCrow::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	if (m_wVersion > 20) {
		inherited::STATE_Read	(tNetPacket,size);
		if (m_wVersion < 32)
			visual_read			(tNetPacket,m_wVersion);
	}
}

void CSE_ALifeCreatureCrow::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeCreatureCrow::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeCreatureCrow::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeCreatureCrow::FillProps			(LPCSTR pref, PropItemVec& values)
{
  	inherited::FillProps			(pref,values);
}

bool CSE_ALifeCreatureCrow::used_ai_locations	() const
{
	return						(false);
}


////////////////////////////////////////////////////////////////////////////
// CSE_ALifeCreaturePhantom
////////////////////////////////////////////////////////////////////////////
CSE_ALifeCreaturePhantom::CSE_ALifeCreaturePhantom(LPCSTR caSection) : CSE_ALifeCreatureAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	m_flags.set					(flUseSwitches,FALSE);
	m_flags.set					(flSwitchOffline,FALSE);
}

CSE_ALifeCreaturePhantom::~CSE_ALifeCreaturePhantom()
{
}

void CSE_ALifeCreaturePhantom::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifeCreaturePhantom::STATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifeCreaturePhantom::UPDATE_Read		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeCreaturePhantom::UPDATE_Write		(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeCreaturePhantom::FillProps		(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProps		(pref,values);
}

bool CSE_ALifeCreaturePhantom::used_ai_locations	() const
{
	return						(false);
}

////////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterZombie
////////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterZombie::CSE_ALifeMonsterZombie	(LPCSTR caSection) : CSE_ALifeMonsterAbstract(caSection)
{
	if (pSettings->section_exist(caSection) && pSettings->line_exist(caSection,"visual"))
		set_visual				(pSettings->r_string(caSection,"visual"));
	// personal charactersitics
	fEyeFov						= 120;
	fEyeRange					= 30;
	fHealth						= 200;
	fMinSpeed					= 1.5;
	fMaxSpeed					= 1.75;
	fAttackSpeed				= 2.0;
	fMaxPursuitRadius			= 100;
	fMaxHomeRadius				= 30;
	// attack
	fHitPower					= 20.0;
	u16HitInterval				= 1000;
	fAttackDistance				= 1.0f;
	fAttackAngle				= 15;
}

CSE_ALifeMonsterZombie::~CSE_ALifeMonsterZombie()
{
}

void CSE_ALifeMonsterZombie::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	// inherited properties
	inherited::STATE_Read		(tNetPacket,size);
	// personal characteristics
	tNetPacket.r_float			(fEyeFov);
	tNetPacket.r_float			(fEyeRange);
	if (m_wVersion <= 5)
		tNetPacket.r_float		(fHealth);
	tNetPacket.r_float			(fMinSpeed);
	tNetPacket.r_float			(fMaxSpeed);
	tNetPacket.r_float			(fAttackSpeed);
	tNetPacket.r_float			(fMaxPursuitRadius);
	tNetPacket.r_float			(fMaxHomeRadius);
	// attack
	tNetPacket.r_float			(fHitPower);
	tNetPacket.r_u16			(u16HitInterval);
	tNetPacket.r_float			(fAttackDistance);
	tNetPacket.r_float			(fAttackAngle);
}

void CSE_ALifeMonsterZombie::STATE_Write	(NET_Packet	&tNetPacket)
{
	// inherited properties
	inherited::STATE_Write		(tNetPacket);
	// personal characteristics
	tNetPacket.w_float			(fEyeFov);
	tNetPacket.w_float			(fEyeRange);
	tNetPacket.w_float			(fMinSpeed);
	tNetPacket.w_float			(fMaxSpeed);
	tNetPacket.w_float			(fAttackSpeed);
	tNetPacket.w_float			(fMaxPursuitRadius);
	tNetPacket.w_float			(fMaxHomeRadius);
	// attack
	tNetPacket.w_float			(fHitPower);
	tNetPacket.w_u16			(u16HitInterval);
	tNetPacket.w_float			(fAttackDistance);
	tNetPacket.w_float			(fAttackAngle);
}

void CSE_ALifeMonsterZombie::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeMonsterZombie::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeMonsterZombie::FillProps		(LPCSTR pref, PropItemVec& items)
{
   	inherited::FillProps			(pref, items);
	// personal characteristics
	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Field of view" 		),&fEyeFov,							0,170,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Eye range" 			),&fEyeRange,						0,300,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Minimum speed" 		),&fMinSpeed,						0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Maximum speed" 		),&fMaxSpeed,						0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Attack speed" 			),&fAttackSpeed,					0,10,0.1f);
   	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Pursuit distance" 		),&fMaxPursuitRadius,				0,300,10);
   	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Personal",	"Home distance" 		),&fMaxHomeRadius,					0,300,10);
	// attack																			 	
	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Attack",		"Hit power" 			),&fHitPower,						0,200,5);
	PHelper().CreateU16  			(items, PrepareKey(pref,*s_name,"Attack",		"Hit interval" 			),&u16HitInterval,					0,65535,500);
	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Attack",		"Distance" 				),&fAttackDistance,					0,300,10);
	PHelper().CreateFloat			(items, PrepareKey(pref,*s_name,"Attack",		"Maximum angle" 		),&fAttackAngle,					0,100,1);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeMonsterBase
//////////////////////////////////////////////////////////////////////////
CSE_ALifeMonsterBase::CSE_ALifeMonsterBase	(LPCSTR caSection) : CSE_ALifeMonsterAbstract(caSection),CSE_PHSkeleton(caSection)
{
    set_visual					(pSettings->r_string(caSection,"visual"));
	m_spec_object_id			= 0xffff;
	
}

CSE_ALifeMonsterBase::~CSE_ALifeMonsterBase()
{
}

void CSE_ALifeMonsterBase::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket,size);
	if(m_wVersion>=68)
		inherited2::STATE_Read		(tNetPacket,size);

	if (m_wVersion>=109)
		tNetPacket.r_u16			(m_spec_object_id);	
}

void CSE_ALifeMonsterBase::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	
	tNetPacket.w_u16			(m_spec_object_id);	
}

void CSE_ALifeMonsterBase::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeMonsterBase::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited1::UPDATE_Write		(tNetPacket);
	inherited2::UPDATE_Write		(tNetPacket);
}

void CSE_ALifeMonsterBase::load(NET_Packet &tNetPacket)
{
	inherited1::load(tNetPacket);
	inherited2::load(tNetPacket);
}

void CSE_ALifeMonsterBase::FillProps	(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps			(pref,values);
	inherited2::FillProps			(pref,values);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifePsyDogPhantom
//////////////////////////////////////////////////////////////////////////
CSE_ALifePsyDogPhantom::CSE_ALifePsyDogPhantom	(LPCSTR caSection) : CSE_ALifeMonsterBase(caSection)
{
}

CSE_ALifePsyDogPhantom::~CSE_ALifePsyDogPhantom()
{
}

void CSE_ALifePsyDogPhantom::STATE_Read		(NET_Packet	&tNetPacket, u16 size)
{
	inherited::STATE_Read		(tNetPacket,size);
}

void CSE_ALifePsyDogPhantom::STATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::STATE_Write		(tNetPacket);
}

void CSE_ALifePsyDogPhantom::UPDATE_Read	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Read		(tNetPacket);
}

void CSE_ALifePsyDogPhantom::UPDATE_Write	(NET_Packet	&tNetPacket)
{
	inherited::UPDATE_Write		(tNetPacket);
}

void CSE_ALifePsyDogPhantom::FillProps	(LPCSTR pref, PropItemVec& values)
{
	inherited::FillProps			(pref,values);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeHumanAbstract
//////////////////////////////////////////////////////////////////////////
CSE_ALifeHumanAbstract::CSE_ALifeHumanAbstract(LPCSTR caSection) : CSE_ALifeTraderAbstract(caSection), CSE_ALifeMonsterAbstract(caSection)
{
	m_group_id					= 0xffff;
}

CSE_ALifeHumanAbstract::~CSE_ALifeHumanAbstract()
{
}

CALifeMonsterBrain *CSE_ALifeHumanAbstract::create_brain	()
{
	m_brain						= xr_new<CALifeHumanBrain>(this);
	return						(m_brain);
}

CSE_Abstract *CSE_ALifeHumanAbstract::init			()
{
	inherited1::init			();
	inherited2::init			();

	return						(base());
}

CSE_Abstract *CSE_ALifeHumanAbstract::base			()
{
	return						(inherited2::base());
}

const CSE_Abstract *CSE_ALifeHumanAbstract::base	() const
{
	return						(inherited2::base());
}

void CSE_ALifeHumanAbstract::STATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
	brain().on_state_write		(tNetPacket);
}

void CSE_ALifeHumanAbstract::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	inherited2::STATE_Read		(tNetPacket, size);
	brain().on_state_read		(tNetPacket);
	if ((m_wVersion >= 110) && (m_wVersion < 112))
		tNetPacket.r			(&m_smart_terrain_id,sizeof(m_smart_terrain_id));
}

void CSE_ALifeHumanAbstract::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
};

void CSE_ALifeHumanAbstract::UPDATE_Read	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	
	if (m_wVersion < 110) {
		tNetPacket.r_u32		();
		tNetPacket.r_u32		();
		tNetPacket.r_u32		();
	}
};

void CSE_ALifeHumanAbstract::FillProps		(LPCSTR pref, PropItemVec& items)
{
  	inherited1::FillProps		(pref,items);
  	inherited2::FillProps		(pref,items);
	PHelper().CreateFlag32		(items,	PrepareKey(pref,*s_name,"Group behaviour"),			&m_flags,			flGroupBehaviour);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeHumanStalker
//////////////////////////////////////////////////////////////////////////
CSE_ALifeHumanStalker::CSE_ALifeHumanStalker(LPCSTR caSection) : CSE_ALifeHumanAbstract(caSection),CSE_PHSkeleton(caSection)
{
	m_trader_flags.set			(eTraderFlagInfiniteAmmo,TRUE);
	m_start_dialog				= "";
}

CSE_ALifeHumanStalker::~CSE_ALifeHumanStalker()
{
}

void CSE_ALifeHumanStalker::STATE_Write		(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);
	inherited2::STATE_Write		(tNetPacket);
}

void CSE_ALifeHumanStalker::STATE_Read		(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);
	
	if (m_wVersion > 67)
		inherited2::STATE_Read	(tNetPacket, size);

	if ((m_wVersion > 90) && (m_wVersion < 111))
		tNetPacket.r_u8			();
}

void CSE_ALifeHumanStalker::UPDATE_Write	(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
	inherited2::UPDATE_Write	(tNetPacket);
	tNetPacket.w_stringZ		(m_start_dialog);
}

void CSE_ALifeHumanStalker::UPDATE_Read		(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
	inherited2::UPDATE_Read		(tNetPacket);
	tNetPacket.r_stringZ		(m_start_dialog);
}

void CSE_ALifeHumanStalker::load			(NET_Packet &tNetPacket)
{
	inherited1::load			(tNetPacket);
	inherited2::load			(tNetPacket);
}

void CSE_ALifeHumanStalker::FillProps		(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps		(pref,values);
	inherited2::FillProps		(pref,values);
}

//////////////////////////////////////////////////////////////////////////
// CSE_ALifeOnlineOfflineGroup
//////////////////////////////////////////////////////////////////////////

CSE_ALifeOnlineOfflineGroup::CSE_ALifeOnlineOfflineGroup	(LPCSTR caSection) : CSE_ALifeDynamicObject(caSection), CSE_ALifeSchedulable(caSection)
{
}

CSE_Abstract *CSE_ALifeOnlineOfflineGroup::base				()
{
	return						(this);
}

const CSE_Abstract *CSE_ALifeOnlineOfflineGroup::base		() const
{
	return						(this);
}

CSE_Abstract *CSE_ALifeOnlineOfflineGroup::init				()
{
	inherited1::init			();
	inherited2::init			();

#ifdef XRGAME_EXPORTS
	m_brain						= xr_new<CALifeOnlineOfflineGroupBrain>(this);
#endif

	VERIFY						(m_members.empty());
	m_flags.set					(flUsedAI_Locations,FALSE);

	return						(this);
}

CSE_ALifeOnlineOfflineGroup::~CSE_ALifeOnlineOfflineGroup	()
{
#ifdef XRGAME_EXPORTS
	xr_delete					(m_brain);
#endif
}

void CSE_ALifeOnlineOfflineGroup::STATE_Write				(NET_Packet &tNetPacket)
{
	inherited1::STATE_Write		(tNetPacket);

#if 1
	tNetPacket.w_u32			(m_members.size());

	MEMBERS::iterator			I = m_members.begin();
	MEMBERS::iterator			E = m_members.end();
	for ( ; I != E; ++I)
		save_data				((*I).first,tNetPacket);
#endif
}

void CSE_ALifeOnlineOfflineGroup::STATE_Read				(NET_Packet &tNetPacket, u16 size)
{
	inherited1::STATE_Read		(tNetPacket, size);

#if 1
	u32							container_size = tNetPacket.r_u32();
	for (u32 i=0; i<container_size; ++i) {
		MEMBERS::value_type		pair;
		load_data				(pair.first,tNetPacket);
		pair.second				= 0;
		m_members.insert		(pair);
	}
#endif
}

void CSE_ALifeOnlineOfflineGroup::UPDATE_Write				(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Write	(tNetPacket);
}

void CSE_ALifeOnlineOfflineGroup::UPDATE_Read				(NET_Packet &tNetPacket)
{
	inherited1::UPDATE_Read		(tNetPacket);
}

void CSE_ALifeOnlineOfflineGroup::FillProps					(LPCSTR pref, PropItemVec& values)
{
	inherited1::FillProps		(pref,values);
}
