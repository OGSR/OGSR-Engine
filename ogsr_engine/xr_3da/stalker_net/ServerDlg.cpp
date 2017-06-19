// ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "ServerDlg.h"
#include ".\serverdlg.h"
#include "Stalker_netDlg.h"
#include "windows.h"


#include "Version_Define.h"
// CServerDlg dialog

xr_token game_types[]={
	{ "any game",		ST_GAME_ANY		},
	{ "single",			ST_GAME_SINGLE		},
	{ "deathmatch",		ST_GAME_DEATHMATCH },
	//	{ "CTF",			GAME_CTF		},
	//	{ "assault",		GAME_ASSAULT	},
	{ "counterstrike",	ST_GAME_CS			},
	{ "teamdeathmatch",	ST_GAME_TEAMDEATHMATCH },
	{ "artefacthunt",	ST_GAME_ARTEFACTHUNT },
	{ 0,				0				}
};

IMPLEMENT_DYNAMIC(CServerDlg, CSubDlg)
CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(CServerDlg::IDD, pParent)
{
	m_pSVServerOptDlg	= NULL;
	m_pSVGameTypeDlg	= NULL;
	m_pSVGameOptDlg		= NULL;
	m_pSVRepawnDlg		= NULL;
	m_pSVO2				= NULL;
	m_pSVWeatherOptDlg	= NULL;
	m_pSVSpectatorOptsDlg	= NULL;
}

CServerDlg::~CServerDlg()
{
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STARTSERVER, m_pStartServerBtn);
	DDX_Control(pDX, IDC_MAPLIST, m_pMapList);
	DDX_Control(pDX, IDC_SERVEROPTIONS, m_pServerOptionsButton);
	DDX_Control(pDX, IDC_MAPLIST2, m_pMapList2);
	DDX_Control(pDX, IDC_2LIST, m_p2List);
	DDX_Control(pDX, IDC_2LIST2, m_p2List2);
}


BEGIN_MESSAGE_MAP(CServerDlg, CDialog)
	ON_BN_CLICKED(IDC_STARTSERVER, OnBnClickedStartServer)
	ON_LBN_SELCHANGE(IDC_MAPLIST, OnLbnSelchangeMapList)
//	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
//ON_BN_CLICKED(IDC_STARTSERVER2, OnBnClickedStartserver2)
ON_BN_CLICKED(IDC_SERVEROPTIONS, OnBnClickedServerOptions)
ON_BN_CLICKED(IDC_2LIST2, OnBnClicked2List2)
ON_BN_CLICKED(IDC_2LIST, OnBnClicked2List)
ON_LBN_DBLCLK(IDC_MAPLIST, OnBnClicked2List2)
ON_LBN_DBLCLK(IDC_MAPLIST2, OnBnClicked2List)
//ON_LBN_DBLCLK(IDC_MAPLIST, OnLbnDblclkMapList)
END_MESSAGE_MAP()

BOOL CServerDlg::AddDlg(CSubDlg* pNewDlg, CWnd* ParentWnd, int &CX, int &CY)
{
	if (!pNewDlg) return FALSE;
	pNewDlg->SetParent(ParentWnd);
	pNewDlg->ShowWindow(SW_SHOW);
	pNewDlg->SetWindowPos(NULL, CX, CY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	RECT R;
	pNewDlg->GetWindowRect(&R);
	CY += R.bottom - R.top + 1;

	return TRUE;
};

BOOL CServerDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();
	//-------------------------------------------------------
	int CX = 0; RECT R;
	m_pMapList.GetWindowRect(&R);
//	CX = R.right + 0;
	//-------------------------------------------------------
	m_pSVO2 = new ServerOpts2();
	if (m_pSVO2)
	{
		BOOL ret = m_pSVO2->Create(IDD_SERVEROPTS2,this);
		if(!ret)   //Create failed.
			AfxMessageBox("Error creating Respawn Dialog");
		m_pSVO2->ShowWindow(SW_SHOW);
//		m_pSVO2->SetParent(GetParent()->GetParent());
		m_pSVO2->SetParent(this);
//		m_pSVO2->CenterWindow(NULL);
		m_pSVO2->SetWindowPos(NULL, CX, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
		AfxMessageBox("Error Creating Respawn Dialog Object");
	//-------------------------------------------------------
	CX = 5;
	int CY = 3;
	if (m_pSVO2)
	{
		m_pSVServerOptDlg = new SVServerOptDlg();
		if (m_pSVServerOptDlg)
		{
			BOOL ret = m_pSVServerOptDlg->Create(IDD_SERVEROPT,m_pSVO2);
			if(!ret)   //Create failed.
				AfxMessageBox("Error Creating Server Options Dialog");
			AddDlg(m_pSVServerOptDlg, m_pSVO2, CX, CY);			
		}
		else
			AfxMessageBox("Error Creating Dialog Object");
		//-------------------------------------------------------
		m_pSVGameTypeDlg = new SVGameTypeDlg();
		if (m_pSVGameTypeDlg)
		{
			BOOL ret = m_pSVGameTypeDlg->Create(IDD_GAMETYPE,m_pSVO2);
			if(!ret)   //Create failed.
				AfxMessageBox("Error Creating Game Type Dialog");
			AddDlg(m_pSVGameTypeDlg, m_pSVO2, CX, CY);			
		}
		else
			AfxMessageBox("Error Creating Dialog Object");
		//-------------------------------------------------------
		m_pSVGameOptDlg = new SVGameOptDlg();
		if (m_pSVGameOptDlg)
		{
			BOOL ret = m_pSVGameOptDlg->Create(IDD_GAMEOPTIONS,m_pSVO2);
			if(!ret)   //Create failed.
				AfxMessageBox("Error Creating Game Options Dialog");
			AddDlg(m_pSVGameOptDlg, m_pSVO2, CX, CY);			
		}
		else
			AfxMessageBox("Error Creating Dialog Object");
		//-------------------------------------------------------
		
//		int LastCY = CY;
		m_pSVRepawnDlg = new SVRespawnDlg();
		if (m_pSVRepawnDlg)
		{
			BOOL ret = m_pSVRepawnDlg->Create(IDD_RESPAWN,m_pSVO2);
			if(!ret)   //Create failed.
				AfxMessageBox("Error Creating Respawn Options Dialog");
			AddDlg(m_pSVRepawnDlg, m_pSVO2, CX, CY);
		}
		else
			AfxMessageBox("Error Creating Dialog Object");
		//-------------------------------------------------------
		m_pSVSpectatorOptsDlg = new SVSpectatorDlg();
		if (m_pSVSpectatorOptsDlg)
		{
			BOOL ret = m_pSVSpectatorOptsDlg->Create(IDD_SPECTATOROPTS,m_pSVO2);
			if(!ret)   //Create failed.
				AfxMessageBox("Error Creating Spectator Options Dialog");
			AddDlg(m_pSVSpectatorOptsDlg, m_pSVO2, CX, CY);
		}
		else
			AfxMessageBox("Error Creating Dialog Object");		
		//-------------------------------------------------------
		m_pSVWeatherOptDlg = new SVWeatherOptions();
		if (m_pSVWeatherOptDlg)
		{
			BOOL ret = m_pSVWeatherOptDlg->Create(IDD_WEATHEROPTIONS,m_pSVO2);
			if(!ret)   //Create failed.
				AfxMessageBox("Error Creating Weather Options Dialog");
			AddDlg(m_pSVWeatherOptDlg, m_pSVO2, CX, CY);
		}
		else
			AfxMessageBox("Error Creating Dialog Object");		
		
		//-------------------------------------------------------
		m_pSVO2->GetClientRect(&R);
		if (CY > R.bottom)
			m_pSVO2->SetScrollRange(SB_VERT, 0, CY - R.bottom, TRUE);
		else
			m_pSVO2->SetScrollRange(SB_VERT, 0, 0, TRUE);
	};	
	//---------------------------------------	
	LoadMapList();

	m_pStartServerBtn.EnableWindow(false);
	//---------------------------------------
	SwitchGameType(GAME_DEATHMATCH);
	
	if (m_maps[ST_GAME_DEATHMATCH].size() == 0) 
	{
		m_pSVGameTypeDlg->m_pGameDM.EnableWindow(false);
		SwitchGameType(GAME_TEAMDEATHMATCH);
	};
	if (m_maps[ST_GAME_TEAMDEATHMATCH].size() == 0) 
	{
		m_pSVGameTypeDlg->m_pGameTDM.EnableWindow(false);
		SwitchGameType(GAME_ARTEFACTHUNT);
	};
	if (m_maps[ST_GAME_ARTEFACTHUNT].size() == 0) 
	{
		m_pSVGameTypeDlg->m_pGameAHunt.EnableWindow(false);
		SwitchGameType(GAME_DEATHMATCH);
	};
	
	//-------------------------------------------
	OnBnClickedServerOptions();
	return TRUE;  // return TRUE  unless you set the focus to a control
}
// CServerDlg message handlers

#include "Misc_Ruts.h"

//DEF_VECTOR(shared_str_vec, shared_str)
//DEF_MAP(storage_map, EGameTypes, shared_str_vec)
//storage_map				m_maps;

void	CServerDlg::LoadMapList()
{
	string_path			fn;
	FS.update_path		(fn, "$game_config$", MAP_LIST);
	CInifile			map_list_cfg(fn);

	// maps
	for (int k=0; game_types[k].name; ++k)
	{
		EGameTypes _id			= (EGameTypes)game_types[k].id;
		LPCSTR _name			= game_types[k].name;

		if( !map_list_cfg.section_exist(_name) ) continue;

		CInifile::Sect& S		= map_list_cfg.r_section(_name);
		CInifile::SectCIt it	= S.Data.begin(), end = S.Data.end();

		for (;it!=end; ++it)
		{
			shared_str _map_name = it->first;
			m_maps[_id].push_back	(_map_name);
//			strcpy(m_Maps[MapListType][m_MapsNum[MapListType]++], token);
		}
		//		std::sort(m_maps[_id].begin(), m_maps[_id].end(), MP_map_cmp);
	}

	//weather
	shared_str				weather_sect = "weather";
	CInifile::Sect& S		= map_list_cfg.r_section(weather_sect);
	CInifile::SectCIt it		= S.Data.begin(), end = S.Data.end();

	shared_str				WeatherType;
	shared_str				WeatherTime;

	for (;it!=end; ++it){
		WeatherType			= it->first;
		WeatherTime			= map_list_cfg.r_string(weather_sect, *WeatherType);

//		AddWeather			(WeatherType, WeatherTime);
		if (m_pSVWeatherOptDlg) m_pSVWeatherOptDlg->AddWeather(*WeatherType, *WeatherTime);
	}
}
/*
{
	char Buffer[4096];
	memset(Buffer, 0, sizeof(Buffer));
	FILE* f = fopen(MAP_LIST, "rb");
	if (!f) return;

	size_t NumRead = fread(Buffer, 1, sizeof(Buffer), f);
	if (!NumRead) return;
	fclose(f);


	char token[1024];
	char* s = Buffer;
	char* e = Buffer + strlen(Buffer) + 1;

	int MapListType = GAME_UNKNOWN;
	
	while (1)
	{
		if (!GetToken(&s, e, token)) break;
		if (!xr_strcmp(token, "deathmatch")) MapListType = GAME_DEATHMATCH;
		else
		{
			if (!xr_strcmp(token, "teamdeathmatch")) MapListType = GAME_TEAMDEATHMATCH;
			else
			{
				if (!xr_strcmp(token, "artefacthunt")) MapListType = GAME_ARTEFACTHUNT;
				else 
				{
					if (!xr_strcmp(token, "weather"))
					{
						ParseWeather(&s, e);
						continue;
					}
					else break;
				};				
			}
		}
		if (!GetToken(&s, e, token)) break;
		if (xr_strcmp(token, "{")) break;
		
		m_MapsNum[MapListType] = 0;
		while (1)
		{
			if (!GetToken(&s, e, token)) break;
			if (!xr_strcmp(token, "}")) break;

			if (!xr_strcmp(token, "mapname"))
			{
				GetToken(&s, e, token);
				strcpy(m_Maps[MapListType][m_MapsNum[MapListType]++], token);
			};
		};
	};
};
*/

void	CServerDlg::ParseWeather(char** ps, char* e)
{
//	char* s = *ps;
	char token[1024];

	while (1)
	{	
		if (!GetToken(ps, e, token)) break;
		if (xr_strcmp(token, "{")) break;
		while (1)
		{
			if (!GetToken(ps, e, token)) break;
			if (!xr_strcmp(token, "}")) return;
			if (!xr_strcmp(token, "startweather"))
			{
				char WeatherType[1024], WeatherTime[1024];
				GetToken(ps, e, WeatherType);
				GetToken(ps, e, WeatherTime);

				m_pSVWeatherOptDlg->AddWeather(WeatherType, WeatherTime);
			};
		};
	};
};

void	CServerDlg::UpdateMapList(GAME_TYPE GameType)
{
	m_pMapList.ResetContent();
	m_pMapList2.ResetContent();

	EGameTypes xGameType = ST_GAME_ANY;
	switch (GameType)
	{
	case GAME_DEATHMATCH: xGameType = ST_GAME_DEATHMATCH; break;
	case GAME_TEAMDEATHMATCH: xGameType = ST_GAME_TEAMDEATHMATCH; break;
	case GAME_ARTEFACTHUNT: xGameType = ST_GAME_ARTEFACTHUNT; break;
	}
//	for (int i=0; i<m_MapsNum[GameType]; i++)
//	{
//		m_pMapList.AddString(m_Maps[GameType][i]);
//	};
	for (u32 i=0; i<(u32)m_maps[xGameType].size(); i++)
	{
		m_pMapList.AddString(*(m_maps[xGameType][i]));
	};
};

bool CServerDlg::GetMapName	(char* pMapName)
{
	/*
	int CurSel = m_pMapList.GetCurSel();
	if (CurSel == LB_ERR) return false;
	int Len = m_pMapList.GetText(CurSel, pMapName);
	if (Len == LB_ERR) return false;
	return true;
	*/
	if (!m_pMapList2.GetCount()) return false;
	m_pMapList2.GetText(0, pMapName);
	return true;
};

void CServerDlg::OnLbnSelchangeMapList()
{
	return;
	// TODO: Add your control notification handler code here
	char MapName[1024];
	if (GetMapName(MapName))
		m_pStartServerBtn.EnableWindow(TRUE);
	else
		m_pStartServerBtn.EnableWindow(FALSE);
}

#include <process.h>

void CServerDlg::OnBnClickedStartServer()
{
	// TODO: Add your control notification handler code here

	char MapName[1024];
	if (!GetMapName(MapName)) return;
	SaveMapList();

	int iCatchInput = 1;
	int iDedicated = 0;
	int iSpectatorOnly = 0;
	int iBuild = 0;
	int iPrefetch = 1;
	int iR2 = 0;
	int iDistort = 1;
	int NameLen = 0;
	char Name[1024];
	char NameAdd[1024];
	char pTeam[2][1024] = {"/team=1", "/team=2"};
	char sLogsPath[1024] = "";
	char sCDKeyStr[1024] = "";
	
	int FragLimit = 0;
	int TimeLimit = 0;
	char FragLimStr[1024];
	char TimeLimStr[1024];
	char SpectrStr[1024];
	char ArtefactNumStr[1024]; 
	int ArtefactDelay = 0;
	char ArtefactDelayStr[1024]; 
	int ArtefactStay = 0;
	char ArtefactStayStr[1024]; 
	char ReinforcementStr[1024];
	
	
	if (m_pSVGameOptDlg->m_pFragLimit.GetWindowTextLength())
	{
		CString tmpText;
		m_pSVGameOptDlg->m_pFragLimit.GetWindowText(tmpText);
		FragLimit = atoi(tmpText);
		sprintf(FragLimStr, "/fraglimit=%d", FragLimit);
	};
	if (m_pSVGameOptDlg->m_pTimeLimit.GetWindowTextLength())
	{
		CString tmpText;
		m_pSVGameOptDlg->m_pTimeLimit.GetWindowText(tmpText);
		TimeLimit = atoi(tmpText);
		sprintf(TimeLimStr, "/timelimit=%d", TimeLimit);
	};
	char FriendlyFireStr[1024];
	sprintf(FriendlyFireStr, "/ffire=%d", m_pSVGameOptDlg->m_pFriedlyFire.GetPos());
	char AutoTeamBalanceStr[1024] = "";
	if (m_pSVGameOptDlg->m_pAutoTeamBalance.IsWindowEnabled())
		if (m_pSVGameOptDlg->m_pAutoTeamBalance.GetCheck())
			sprintf(AutoTeamBalanceStr, "/abalance=1");
	
	char AutoTeamSwapStr[1024] = "";
	if (m_pSVGameOptDlg->m_pAutoTeamSwap.IsWindowEnabled())
		if (m_pSVGameOptDlg->m_pAutoTeamSwap.GetCheck())
			sprintf(AutoTeamSwapStr, "/aswap=1");
	
	if (m_GameType == GAME_ARTEFACTHUNT)
	{
		DWORD_PTR ItemData = m_pSVGameOptDlg->m_pArtefactsNum.GetItemData(m_pSVGameOptDlg->m_pArtefactsNum.GetCurSel());
		sprintf(ArtefactNumStr, "/anum=%d", ItemData);
	
		CString tmpText;
		m_pSVGameOptDlg->m_pArtefactDelay.GetWindowText(tmpText);
		ArtefactDelay = atoi(tmpText);
		sprintf(ArtefactDelayStr, "/ardelta=%d", ArtefactDelay);
		
		m_pSVGameOptDlg->m_pArtefactStay.GetWindowText(tmpText);
		ArtefactStay = atoi(tmpText);
		sprintf(ArtefactStayStr, "/astime=%d", ArtefactStay);

		if (m_pSVRepawnDlg->m_pACaptured.GetCheck())
		{
			sprintf(ReinforcementStr, "/reinf=-1", tmpText);
		}
		else
		{

			m_pSVRepawnDlg->m_pReinforcementTime.GetWindowText(tmpText);
			int ReinfTime = atoi(tmpText);
			if (ReinfTime > 0)
				sprintf(ReinforcementStr, "/reinf=%s", tmpText);
			else
				sprintf(ReinforcementStr, "");
		};;
	}
	CStalker_netDlg* pMainDlg = (CStalker_netDlg*) (GetParent()->GetParent());
	if (pMainDlg)
	{
		iCatchInput = pMainDlg->m_pCatchInput.GetCheck();

		NameLen = pMainDlg->m_pPlayerName.GetWindowTextLength();
		if (NameLen) 
		{
			pMainDlg->m_pPlayerName.GetWindowText(Name, 1023);
			Name[32] = 0;
			sprintf(NameAdd, "/name=%s", Name);
		};

		iBuild = pMainDlg->m_pBuild.GetCheck();
		iPrefetch  = pMainDlg->m_pPrefetch.GetCheck();
		iR2 = pMainDlg->m_pR2.GetCheck();
		iDistort = pMainDlg->m_pDistort.GetCheck();
		if (pMainDlg->m_pLogsPath.GetWindowTextLength() != 0)
		{
			char tmp[1024];
			pMainDlg->m_pLogsPath.GetWindowText(tmp, 1024);
			sprintf(sLogsPath, "-overlaypath %s ", tmp);
		}
		if (pMainDlg->m_pCDKeyBtn.GetWindowTextLength() != 0)
		{
			char tmp[1024];
			pMainDlg->m_pCDKeyBtn.GetWindowText(tmp, 1024);
			if (xr_strcmp(tmp, "- No CD Key -") != 0)
				sprintf(sCDKeyStr, "/cdkey=%s", tmp);
		}
	};
	//--------- Server Options -----------------------------
	char HostNameStr[1024] = "";
	if (m_pSVServerOptDlg->m_pHostName.GetWindowTextLength() > 0)
	{
		char tmp[1024];
		m_pSVServerOptDlg->m_pHostName.GetWindowText(tmp, 1024);
		sprintf(HostNameStr, "/hname=%s", tmp);
	};
	char PasswordStr[1024] = "";
	if (m_pSVServerOptDlg->m_pPassword.GetWindowTextLength() > 0)
	{
		char tmp[1024];
		m_pSVServerOptDlg->m_pPassword.GetWindowText(tmp, 1024);
		sprintf(PasswordStr, "/psw=%s", tmp);
	}
	char MaxPlayersStr[1024] = "";
	if (m_pSVServerOptDlg->m_pMaxPlayers.GetWindowTextLength() > 0)
	{
		char tmp[1024];
		m_pSVServerOptDlg->m_pMaxPlayers.GetWindowText(tmp, 1024);
		sprintf(MaxPlayersStr, "/maxplayers=%s", tmp);
	};
	iDedicated = m_pSVServerOptDlg->m_pDedicated.GetCheck();
	iSpectatorOnly = m_pSVServerOptDlg->m_pSpectrMode.GetCheck();
	if (iSpectatorOnly)
	{
		CString tmpText;
		m_pSVServerOptDlg->m_pSpectrSwitchTime.GetWindowText(tmpText);
		int SpectrTime = atoi(tmpText);
		sprintf(SpectrStr, "/spectr=%d", SpectrTime);
	};
	int iPublic = m_pSVServerOptDlg->m_pPublic.GetCheck();
	int iCheckCDKey = 0;
	if (m_pSVServerOptDlg->m_pCheckCDKey.IsWindowEnabled())
		iCheckCDKey = m_pSVServerOptDlg->m_pCheckCDKey.GetCheck();

	int iRPFreezeTime = 0;
	char RPFreezeTimeStr[1024] = "";
	if (m_pSVRepawnDlg->m_pRPFreezeTime.GetWindowTextLength())
	{
		CString tmpText;
		m_pSVRepawnDlg->m_pRPFreezeTime.GetWindowText(tmpText);
		iRPFreezeTime = atoi(tmpText);
		sprintf(RPFreezeTimeStr, "/rpfrz=%i", iRPFreezeTime);
	}
	char MapRotStr[1024] = "";
//	if (m_pSVServerOptDlg->m_pMapRotation.GetCheck() &&
//		m_pSVServerOptDlg->m_pMapRotationFile.GetWindowTextLength())
//	{
//		CString MapRotFileName;
//		m_pSVServerOptDlg->m_pMapRotationFile.GetWindowText(MapRotFileName);
//		MapRotFileName += ".ltx";
//		sprintf(MapRotStr, "/maprot=%s", MapRotFileName);
//	}
	
	char VoteStr[1024] = "";
	if (m_pSVServerOptDlg->m_pVotingAllowed.GetCheck())
	{
		sprintf(VoteStr, "/vote=1");
	}
	char FIStr[1024] = "";
	if (m_pSVGameOptDlg->m_pFriendlyIndicators.IsWindowEnabled() && m_pSVGameOptDlg->m_pFriendlyIndicators.GetCheck())
	{
		sprintf(FIStr, "/fi=1");
	}
	char FNStr[1024] = "";
	if (m_pSVGameOptDlg->m_pFriendlyNames.IsWindowEnabled() && m_pSVGameOptDlg->m_pFriendlyNames.GetCheck())
	{
		sprintf(FNStr, "/fn=1");
	}
	char DmgBlkStr[1024] = "";
	if (m_pSVRepawnDlg->m_pDmBlockLimit.GetWindowTextLength())
	{
		CString DmgBlkLimit;
		m_pSVRepawnDlg->m_pDmBlockLimit.GetWindowText(DmgBlkLimit);
		sprintf(DmgBlkStr, "/dmgblock=%d", atol(DmgBlkLimit));
	}
	char DmgBlkStrInd[1024] = "";
	if (m_pSVRepawnDlg->m_pDmgBlkInd.IsWindowEnabled() && m_pSVRepawnDlg->m_pDmgBlkInd.GetCheck())
	{
		sprintf(DmgBlkStrInd, "/dmbi=1");
	}
	char ForceRespawnStr[1024] = "";
	if (m_pSVRepawnDlg->m_pForceRespawn.IsWindowEnabled() && m_pSVRepawnDlg->m_pForceRespawn.GetCheck())
	{
		CString ForceRespawnTime;
		m_pSVRepawnDlg->m_pForceRespawnTime.GetWindowText(ForceRespawnTime);
		if (atol(ForceRespawnTime))
		{
			sprintf(DmgBlkStr, "/frcrspwn=%d", atol(ForceRespawnTime));
		}
	};
	//-------------------------------------------
	char AnomalySetTimeStr[1024] = "";
	if (m_pSVGameOptDlg->m_pNoAnomalies.GetCheck()==0)
	{
		CString AnomalySetTime;
		m_pSVGameOptDlg->m_pAnomalySetTime.GetWindowText(AnomalySetTime);
		sprintf(DmgBlkStr, "/anslen=%d", atol(AnomalySetTime));
	}
//-----------------------------------------------------------------------------
	char WarmUpStr[1024] = "";
	if (m_pSVGameOptDlg->m_pWarmUpTime.GetWindowTextLength()>0)
	{
		CString WarmUpTime;
		m_pSVGameOptDlg->m_pWarmUpTime.GetWindowText(WarmUpTime);
		if (atol(WarmUpTime)>0)
			sprintf(WarmUpStr, "/warmup=%d", atol(WarmUpTime));
	}
//-----------------------------------------------------------------------------
	//------------- Weather ---------------------------
	char WeatherCoeff[1024] = "";
	char WeatherTime[1024] = "";
	if (m_pSVWeatherOptDlg->m_pWeatherSpeedCoeff.GetWindowTextLength())
	{
		CString WeatherSpeedCoeff;
		m_pSVWeatherOptDlg->m_pWeatherSpeedCoeff.GetWindowText(WeatherSpeedCoeff);
		sprintf(WeatherCoeff, "/etimef=%d", atol(WeatherSpeedCoeff));
	};
	if (m_pSVWeatherOptDlg->m_pStartWeather.GetCount())
	{
		DWORD Time = DWORD(m_pSVWeatherOptDlg->m_pStartWeather.GetItemData(m_pSVWeatherOptDlg->m_pStartWeather.GetCurSel()));
		sprintf(WeatherTime, "/estime=%d:%d", Time/60, Time%60);
	}

	//-------------------------------------------------
	//ports
	string512 SVPortStr = "";
	if (m_pSVServerOptDlg->m_pSVPort.GetWindowTextLength() > 0)
	{
		char tmp[1024];
		m_pSVServerOptDlg->m_pSVPort.GetWindowText(tmp, 1024);
		int Port = atol(tmp);		
		clamp(Port, START_PORT, END_PORT);
		sprintf(SVPortStr, "/portsv=%d", Port);
	};
	string512 CLPortStr = "";
	if (m_pSVServerOptDlg->m_pCLPort.GetWindowTextLength() > 0)
	{
		char tmp[1024];
		m_pSVServerOptDlg->m_pCLPort.GetWindowText(tmp, 1024);
		int Port = atol(tmp);		
		clamp(Port, START_PORT, END_PORT);
		sprintf(CLPortStr, "/portcl=%d", Port);
	};
	string512 GSPortStr = "";
	if (m_pSVServerOptDlg->m_pGSPort.GetWindowTextLength() > 0)
	{
		char tmp[1024];
		m_pSVServerOptDlg->m_pGSPort.GetWindowText(tmp, 1024);
		int Port = atol(tmp);		
		clamp(Port, START_PORT, END_PORT);
		sprintf(GSPortStr, "/portgs=%d", Port);
	};

//	-noprefetch
	string512	temp;
	char cmdline[4096];
	sprintf(cmdline, "%sxr_3da.exe %s -xclsx %s%s%s%s%s%s%s -nointro -external -nocache -start Server(%s/%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s) client(localhost%s%s%s)", 
			(iDedicated == 1) ? "dedicated\\" : "",
			(iDedicated == 1) ? "-nosound" : "",
			//---------------------------------
			(iCatchInput == 1 || iDedicated == 1) ? "-i " : "", 
			(iBuild == 1) ? "-build " : "",
			(iPrefetch == 1) ? "" : "-noprefetch ",
			(iR2 == 1) ? "-r2 " : "",
			(iDistort == 1) ? "" : "-nodistort ",
			(sLogsPath[0] == 0) ? "" : sLogsPath,
			//---------------------------------
			LTX,
			//---------------------------------
			MapName,
			//---------------------------------
			g_GameTypeName[m_GameType],
			HostNameStr,
			PasswordStr,
			MaxPlayersStr,
			(iPublic) ? "/public=1" : "",
			(iCheckCDKey) ? "/cdkey=1" : "",
			(iSpectatorOnly) ? SpectrStr : "",
			(FragLimit) ? FragLimStr : "",
			(TimeLimit) ? TimeLimStr : "",
			AutoTeamBalanceStr,
			AutoTeamSwapStr,
			(m_GameType != GAME_DEATHMATCH) ? FriendlyFireStr :"",
			(m_GameType == GAME_ARTEFACTHUNT) ? ArtefactNumStr : "",
			(m_GameType == GAME_ARTEFACTHUNT) ? ArtefactStayStr : "",
			(m_GameType == GAME_ARTEFACTHUNT) ? ArtefactDelayStr : "",
			(m_GameType == GAME_ARTEFACTHUNT) ? ReinforcementStr : "",
			(iRPFreezeTime) ? RPFreezeTimeStr : "",
			(m_pSVGameOptDlg->m_pNoAnomalies.GetCheck() == 1) ? "/ans=0" : "/ans=1",	
			(AnomalySetTimeStr[0]) ? AnomalySetTimeStr : "",
			(MapRotStr[0]) ? MapRotStr : "",
			(VoteStr[0]) ? VoteStr : "",
			(FIStr[0]) ? FIStr : "",
			(FNStr[0]) ? FNStr : "",
			(DmgBlkStr[0]) ? DmgBlkStr : "",
			(DmgBlkStrInd[0]) ? DmgBlkStrInd : "",
			(WeatherTime[0]) ? WeatherTime : "",
			(WeatherCoeff[0]) ? WeatherCoeff : "",
			(WarmUpStr[0]) ? WarmUpStr : "",
			m_pSVSpectatorOptsDlg->GetSpectatorModesStr(temp,sizeof(temp)),
			(SVPortStr[0]) ? SVPortStr : "",
			(GSPortStr[0]) ? GSPortStr : "",			
			//-------------------------------------
			(NameLen) ? NameAdd : "",
			(CLPortStr[0]) ? CLPortStr : "",
			(sCDKeyStr[0]) ? sCDKeyStr : ""
			);
	
//	if (xr_strlen(cmdline) > 4096) _ASSERT(0);
	OutputDebugString( cmdline );
	int res = WinExec(cmdline, SW_SHOW);	
	//-------------------------------------------------------
	/*
	char* _args[3];
	// check for need to execute something external

	{
		string4096 ModuleFileName = "";		
		GetModuleFileName(NULL, ModuleFileName, 4096);

		string4096 ModuleFilePath = "";
		char* ModuleName = NULL;
		GetFullPathName(ModuleFileName, 4096, ModuleFilePath, &ModuleName);
		ModuleName[0] = 0;

		if (iDedicated == 1)
		{
			char* envpath =getenv("PATH");
			string4096	NewEnvPath = "";
			sprintf(NewEnvPath , "PATH=%s;%s", ModuleFilePath,envpath);
			_putenv(NewEnvPath);

			strcat(ModuleFilePath, "dedicated\\xr_3DA.exe");
		}
		else
			strcat(ModuleFilePath, "xr_3da.exe");

		_args[0] = ModuleFilePath;//g_sLaunchOnExit_app;
		_args[1] = cmdline;//g_sLaunchOnExit_params;
		_args[2] = NULL;		

		_spawnv(_P_NOWAIT, _args[0], _args);//, _envvar);
	}
	*/
}

void		CServerDlg::SwitchGameType(GAME_TYPE NewGameType)
{
	m_GameType = NewGameType;

	if (m_pSVGameTypeDlg) m_pSVGameTypeDlg->OnGameTypeSwitch(NewGameType);
	if (m_pSVGameOptDlg) m_pSVGameOptDlg->OnGameTypeSwitch(NewGameType);
	if (m_pSVRepawnDlg) m_pSVRepawnDlg->OnGameTypeSwitch(NewGameType);
	if (m_pSVSpectatorOptsDlg) m_pSVSpectatorOptsDlg->OnGameTypeSwitch(NewGameType);

	UpdateMapList(m_GameType);
};

//void CServerDlg::OnBnClickedStartserver2()
//{
//	// TODO: Add your control notification handler code here
//}

void CServerDlg::OnBnClickedServerOptions()
{
	// TODO: Add your control notification handler code here
	if (m_pServerOptionsButton.GetCheck())
	{
		m_pSVO2->ShowWindow(SW_SHOW);

		m_pMapList.ShowWindow(SW_HIDE);		m_pMapList.EnableWindow(FALSE);

		m_p2List.ShowWindow(SW_HIDE);		m_p2List.EnableWindow(FALSE);
		m_p2List2.ShowWindow(SW_HIDE);		m_p2List2.EnableWindow(FALSE);

		m_pMapList2.EnableWindow(FALSE);
	}
	else
	{
		m_pSVO2->ShowWindow(SW_HIDE);

		m_pMapList.ShowWindow(SW_SHOW);		m_pMapList.EnableWindow(TRUE);

		m_p2List.ShowWindow(SW_SHOW);		m_p2List.EnableWindow(TRUE);
		m_p2List2.ShowWindow(SW_SHOW);		m_p2List2.EnableWindow(TRUE);

		m_pMapList2.EnableWindow(TRUE);
	};
}

void CServerDlg::OnBnClicked2List2()
{
	// TODO: Add your control notification handler code here
	int NumSelected = m_pMapList.GetSelCount();
	if (!NumSelected) return;

	int SelIndx[1024];
	m_pMapList.GetSelItems(1024, SelIndx);

	char pMapName[1024];
	for (int i=0; i < NumSelected; i++)
	{
		m_pMapList.GetText(SelIndx[i], pMapName);
		m_pMapList.SetSel(SelIndx[i], FALSE);
		if (!pMapName[0]) continue;
		m_pMapList2.AddString(pMapName);
	};

	m_pMapList2.RedrawWindow();
	
	//-----------------------------------------------
	if (m_pMapList2.GetCount())
		m_pStartServerBtn.EnableWindow(TRUE);
	else
		m_pStartServerBtn.EnableWindow(FALSE);
}

void CServerDlg::OnBnClicked2List()
{
	// TODO: Add your control notification handler code here
	int NumSelected = m_pMapList2.GetSelCount();
	if (!NumSelected) return;

	int SelIndx[1024];
	m_pMapList2.GetSelItems(1024, SelIndx);

	for (int i=NumSelected-1; i>=0; i--)
	{
		m_pMapList2.DeleteString(SelIndx[i]);
	};

	m_pMapList2.RedrawWindow();
	//-----------------------------------------------
	if (m_pMapList2.GetCount())
		m_pStartServerBtn.EnableWindow(TRUE);
	else
		m_pStartServerBtn.EnableWindow(FALSE);
}

void	CServerDlg::SaveMapList()
{
//	if (!m_pSVServerOptDlg->m_pMapRotation.GetCheck()) return;
//	if (!m_pSVServerOptDlg->m_pMapRotationFile.GetWindowTextLength()) return;
	
	CString MapRotFileName = MAPROT_LIST_NAME;
//	m_pSVServerOptDlg->m_pMapRotationFile.GetWindowText(MapRotFileName);
//	MapRotFileName += ".ltx";

	string_path			MapRotFileFullPath;

	u32 size = xr_strlen(MapRotFileName);
//	memmove(MapRotFileFullPath, MapRotFileName, xr_strlen(MapRotFileName));
	strcpy(MapRotFileFullPath, MapRotFileName);
	
	FS.update_path		(MapRotFileFullPath, "$app_data_root$", MapRotFileName);
	CStalker_netDlg* pMainDlg = (CStalker_netDlg*) (GetParent()->GetParent());
	if (pMainDlg && pMainDlg->m_pLogsPath.GetWindowTextLength() != 0)
	{
		char tmp[1024];
		string_path NewPath;
		pMainDlg->m_pLogsPath.GetWindowText(tmp, 1024);
		sprintf(NewPath, "%s%s", tmp, MapRotFileFullPath);
		strcpy(MapRotFileFullPath, NewPath);
	}
	if (m_pMapList2.GetCount()<=0)
	{
		FS.file_delete(MapRotFileFullPath);
		return;
	};

	IWriter*		fs	= FS.w_open(MapRotFileFullPath);
	if (!fs) return;

	int SelIndx[1024];
	m_pMapList.GetSelItems(1024, SelIndx);

	char pMapName[1024];
	for (int i=0; i < m_pMapList2.GetCount(); i++)
	{
		m_pMapList2.GetText(i, pMapName);
		if (!pMapName[0]) continue;

		fs->w_printf("sv_addmap %s\n", pMapName);
	};		

	FS.w_close		(fs);
};

//void CServerDlg::OnLbnDblclkMapList()
//{
//	// TODO: Add your control notification handler code here
//}
