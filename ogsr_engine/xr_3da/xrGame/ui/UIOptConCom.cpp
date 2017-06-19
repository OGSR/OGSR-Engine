#include "StdAfx.h"
#include "../../xr_ioconsole.h"
#include "../../xr_ioc_cmd.h"
#include "UIOptConCom.h"
#include "../../../xrcore/xrCore.h"
#include "../game_base_space.h"
#include "../RegistryFuncs.h"
#include "../battleye.h"

xr_token g_GameModes	[] = {
	{ "st_deathmatch",			GAME_DEATHMATCH	},
	{ "st_team_deathmatch",		GAME_TEAMDEATHMATCH	},
	{ "st_artefacthunt",		GAME_ARTEFACTHUNT	},
	{ 0,						0}
};

CUIOptConCom::CUIOptConCom()
{
	strcpy(m_playerName, "");
}

class CCC_UserName: public CCC_String{
public:
	CCC_UserName(LPCSTR N, LPSTR V, int _size) : CCC_String(N, V, _size)  { bEmptyArgsHandled = false; };	
	virtual void Execute(LPCSTR arguments)
	{
		string512 str;
		strcpy(str, arguments);
		if(xr_strlen(str)>17)
			str[17] = 0;

		CCC_String::Execute(str);	

		WriteRegistry_StrValue(REGISTRY_VALUE_USERNAME, value);
	}
	virtual void	Save	(IWriter *F)	{};
};


void CUIOptConCom::Init()
{
	ReadPlayerNameFromRegistry();
	CMD3(CCC_UserName,	"mm_net_player_name", m_playerName,	64);

	m_iMaxPlayers		= 32;
	m_curGameMode		= GAME_DEATHMATCH;
	CMD4(CCC_Integer,	"mm_net_srv_maxplayers",			&m_iMaxPlayers,	2, 32);
	CMD3(CCC_Token,		"mm_net_srv_gamemode",				&m_curGameMode,	g_GameModes);
	m_uNetSrvParams.zero();
	CMD3(CCC_Mask,		"mm_mm_net_srv_dedicated",			&m_uNetSrvParams,	flNetSrvDedicated);
	CMD3(CCC_Mask,		"mm_net_con_publicserver",			&m_uNetSrvParams,	flNetConPublicServer);
	CMD3(CCC_Mask,		"mm_net_con_spectator_on",			&m_uNetSrvParams,	flNetConSpectatorOn);
#ifdef BATTLEYE
	CMD3(CCC_Mask,		"mm_net_use_battleye",				&m_uNetSrvParams,	flNetConBattlEyeOn);
//-	CMD3(CCC_Mask,		"mm_net_battleye_auto_update",		&m_uNetSrvParams,	flNetConBattlEyeAutoUpdate);
#endif // BATTLEYE

	m_iNetConSpectator	= 20;
	CMD4(CCC_Integer,	"mm_net_con_spectator",				&m_iNetConSpectator, 1, 32);

	m_iReinforcementType = 1;
	CMD4(CCC_Integer,	"mm_net_srv_reinforcement_type",	&m_iReinforcementType, 0, 2 );

	//strcpy_s			(m_sReinforcementType,"reinforcement");
	//CMD3(CCC_String,	"mm_net_srv_reinforcement_type",	m_sReinforcementType, sizeof(m_sReinforcementType));
	
	m_fNetWeatherRate = 1.0f;
	CMD4(CCC_Float,		"mm_net_weather_rateofchange",		&m_fNetWeatherRate,	0.0, 100.0f);

	strcpy(m_serverName, Core.CompName);
	CMD3(CCC_String,	"mm_net_srv_name",					m_serverName,	sizeof(m_serverName));

	m_uNetFilter.one	();
	CMD3(CCC_Mask,		"mm_net_filter_empty",				&m_uNetFilter,		fl_empty);
	CMD3(CCC_Mask,		"mm_net_filter_full",				&m_uNetFilter,		fl_full);
	CMD3(CCC_Mask,		"mm_net_filter_pass",				&m_uNetFilter,		fl_pass);
	CMD3(CCC_Mask,		"mm_net_filter_wo_pass",			&m_uNetFilter,		fl_wo_pass);
	CMD3(CCC_Mask,		"mm_net_filter_wo_ff",				&m_uNetFilter,		fl_wo_ff);
	CMD3(CCC_Mask,		"mm_net_filter_listen",				&m_uNetFilter,		fl_listen);

#ifdef BATTLEYE
	CMD3(CCC_Mask,		"mm_net_filter_battleye",			&m_uNetFilter,		fl_battleye);
#endif // BATTLEYE

};

void		CUIOptConCom::ReadPlayerNameFromRegistry	()
{
	ReadRegistry_StrValue(REGISTRY_VALUE_USERNAME, m_playerName);
	if(xr_strlen(m_playerName)>17)
		m_playerName[17] = 0;
};

void		CUIOptConCom::WritePlayerNameToRegistry		()
{
	if(xr_strlen(m_playerName)>17)
		m_playerName[17] = 0;

	WriteRegistry_StrValue(REGISTRY_VALUE_USERNAME, m_playerName);
};