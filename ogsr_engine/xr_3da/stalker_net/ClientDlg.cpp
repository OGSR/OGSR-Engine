// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "ClientDlg.h"
#include ".\clientdlg.h"
#include "Stalker_netDlg.h"
#include "PasswordDlg.h"

#include "Version_Define.h"
//-------------------------------------
#include "GameSpy.net/nonport.h"
#include "GameSpy.net/common/gsAvailable.h"
#include "GameSpy.net/qr2/qr2regkeys.h"
//-------------------------------------
#include "GameSpyFuncs.h"

GAME_TYPE	GetGameType (char* GameType)
{
	if (!GameType) return GAME_UNKNOWN;
	for (int i=0; i<GAME_END_LIST; i++)
	{
		if (!stricmp(GameType, g_GameTypeName[i]))
		{
			return GAME_TYPE(i);
		};
	};
	return GAME_UNKNOWN;
};

GAME_TYPE	GetGameType (byte u8GameType)
{
	if (!u8GameType) return GAME_UNKNOWN;
	switch (u8GameType)
	{
	case ST_GAME_ANY				: return GAME_UNKNOWN;
	case ST_GAME_SINGLE			: return GAME_SINGLE;
	case ST_GAME_DEATHMATCH		: return GAME_DEATHMATCH;
	case ST_GAME_CS				: return GAME_UNKNOWN;
	case ST_GAME_TEAMDEATHMATCH	: return GAME_TEAMDEATHMATCH;
	case ST_GAME_ARTEFACTHUNT		: return GAME_ARTEFACTHUNT;
	case ST_GAME_DUMMY				: return GAME_UNKNOWN;
	}
	return GAME_UNKNOWN;
};
// CClientDlg dialog

IMPLEMENT_DYNAMIC(CClientDlg, CSubDlg)
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(CClientDlg::IDD, pParent)
{
	m_serverBrowser = NULL;
	m_bQuickRefresh = FALSE;
}

CClientDlg::~CClientDlg()
{
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_JOIN, m_pBtnJoin);
	DDX_Control(pDX, IDC_REFRESH, m_pBtnRefresh);
	DDX_Control(pDX, IDC_SERVERSLIST, m_pServerList);
	//	DDX_Control(pDX, IDC_STATIC1, m_pStatic1);
	//	DDX_Control(pDX, IDC_STATIC2, m_pStatic2);
	DDX_Control(pDX, IDC_INTERNET, m_pGameSpyInternet);
	DDX_Control(pDX, IDC_LAN, m_pGameSpyLAN);
	DDX_Control(pDX, IDC_GSUPDATELIST, m_pGameSpyUpdateList);
	DDX_Control(pDX, IDC_GSREFRESHLIST, m_pGameSpyRefreshList);
	DDX_Control(pDX, IDC_FEMPTY, m_pFilter_Empty);
	DDX_Control(pDX, IDC_FFULL, m_pFilter_Full);
	DDX_Control(pDX, IDC_FPASSWORD, m_pFilter_Password_With);
	DDX_Control(pDX, IDC_FNOPASSWORD, m_pFilter_Password_Without);
	DDX_Control(pDX, IDC_FLISTEN, m_pFilter_Listen);
	DDX_Control(pDX, IDC_FNOFF, m_pFilter_NoFF);
	DDX_Control(pDX, IDC_FNOPUNKBUSTER, m_pFilter_PunkBuster_Without);
}

//SB - server list columns
#define COL_SERVERNAME   0
#define COL_MAPNAME      1
#define COL_GAMETYPE     2
#define COL_PLAYERS      3
#define COL_PING	     4

BOOL CClientDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();

	m_pBtnJoin.EnableWindow(FALSE);
	m_pBtnRefresh.EnableWindow(TRUE);

	RECT wRect;
	m_pServerList.GetWindowRect(&wRect);
	int NumColumns = 5;
	m_pServerList.InsertColumn(0, "Server Name", LVCFMT_CENTER, (wRect.right - wRect.left)/NumColumns-1);
	m_pServerList.InsertColumn(1, "Map Name", LVCFMT_CENTER, (wRect.right - wRect.left)/NumColumns-1);
	m_pServerList.InsertColumn(2, "Game Mode", LVCFMT_CENTER, (wRect.right - wRect.left)/NumColumns-1);
	m_pServerList.InsertColumn(3, "Players", LVCFMT_CENTER, (wRect.right - wRect.left)/NumColumns-1);
	m_pServerList.InsertColumn(4, "Ping", LVCFMT_CENTER, (wRect.right - wRect.left)/NumColumns-1);
//	m_pServerList.InsertColumn(5, "Protected", LVCFMT_CENTER, (wRect.right - wRect.left)/NumColumns-1);

	m_pServerList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
	//=============================================================================
	/*
	HICON hiconItem;     // icon for list view items 
	HIMAGELIST hSmall;   // image list for other views 
	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), 
		GetSystemMetrics(SM_CYSMICON), ILC_COLOR24, 2, 1); 
	hiconItem = AfxGetApp()->LoadIcon(MAKEINTRESOURCE(IDB_BITMAP1)); 
	ImageList_AddIcon(hSmall, hiconItem); 
	DestroyIcon(hiconItem); 
	m_pServerList.SetImageList(hSmall, LVSIL_SMALL); 
	*/

	BOOL res = m_pNewImageList.Create((IDB_BITMAP1), 16, 1, RGB(255, 255, 255));
	m_pServerList.SetImageList(&m_pNewImageList, LVSIL_SMALL);



//	hiconItem = LoadIcon(MAKEINTRESOURCE(IDI_ITEM)); 

//	m_pServerList.SetIcon()
	//=============================================================================
	//.......................................................
	InitializeCriticalSection( &m_pCriticalSection );
	// Init COM so we can use CoCreateInstance
	CoInitialize( NULL);//, COINIT_MULTITHREADED );
	//.......................................................
	//=========== Game Spy =================================================
	m_timerID = 0;
	//SB - check that the game's backend is available
	qr2_register_key(GAMETYPE_NAME_KEY,   ("gametypename")  );
	qr2_register_key(DEDICATED_KEY,   ("dedicated")  );
	qr2_register_key(G_FRIENDLY_FIRE_KEY,   ("friendlyfire")  );
	


	GSIACResult result;
	GSIStartAvailableCheck(GAMESPY_GAMENAME);
	while((result = GSIAvailableCheckThink()) == GSIACWaiting)
		msleep(5);
	if(result != GSIACAvailable)
	{
		MessageBox("The backend is not available\n");
		return TRUE;
	}

	m_serverBrowser = ServerBrowserNew(GAMESPY_GAMENAME, GAMESPY_GAMENAME, SECRET_KEY, 0, MAX_UPDATES, QVERSION_QR2, SBFalse, SBCallback, this);
	if(!m_serverBrowser)
	{
		MessageBox("Unable to create the server browser object");
		return FALSE;
	}

	m_pGameSpyUpdateList.EnableWindow(TRUE);
	m_pGameSpyRefreshList.EnableWindow(TRUE);
	CheckRadioButton(IDC_INTERNET, IDC_LAN, IDC_LAN);
	//=====================================================================
	m_pFilter_Empty.SetCheck				(1);
	m_pFilter_Full.SetCheck					(1);
	m_pFilter_Password_With.SetCheck		(1);
	m_pFilter_Password_Without.SetCheck		(1);
	m_pFilter_Listen.SetCheck				(1);
	m_pFilter_NoFF.SetCheck					(1);
	m_pFilter_PunkBuster_Without.SetCheck	(1);

	CheckFilterButtons();
	//=====================================================================
	m_pBtnRefresh.EnableWindow(FALSE);
	//=====================================================================
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnDestroy()
{
	if(m_serverBrowser)
	{
		ServerBrowserFree(m_serverBrowser);
		m_serverBrowser = NULL;
	};
	
	CSubDlg::OnDestroy();

	// TODO: Add your message handler code here
	Client_Close();

	DeleteCriticalSection( &m_pCriticalSection);
	CoUninitialize();	
}

BEGIN_MESSAGE_MAP(CClientDlg, CSubDlg)
	ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh)
	ON_WM_DESTROY()
ON_NOTIFY(NM_CLICK, IDC_SERVERSLIST, OnNMClickServerslist)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVERSLIST, OnLvnItemchangedServerslist)
ON_BN_CLICKED(IDC_JOIN, OnBnClickedJoin)
//ON_BN_CLICKED(IDC_REFRESH, OnBnClickedRefresh1)
ON_BN_CLICKED(IDC_INTERNET, OnBnClickedInternet)
ON_BN_CLICKED(IDC_LAN, OnBnClickedLan)
ON_BN_CLICKED(IDC_GSUPDATELIST, OnBnClickedGSUpdateList)
ON_WM_TIMER()
ON_NOTIFY(LVN_COLUMNCLICK, IDC_SERVERSLIST, OnLvnColumnclickServerslist)
ON_NOTIFY(NM_RCLICK, IDC_SERVERSLIST, OnNMRclickServerslist)
ON_BN_CLICKED(IDC_GSREFRESHLIST, OnBnClickedGSRefreshList)
/*
ON_BN_CLICKED(IDC_FEMPTY, OnBnClicked_Filter_Empty)
ON_BN_CLICKED(IDC_FFULL, OnBnClicked_Filter_Full)
ON_BN_CLICKED(IDC_FPASSWORD, OnBnClicked_Filter_Password_With)
ON_BN_CLICKED(IDC_FNOPASSWORD, OnBnClicked_Filter_Password_Without)
ON_BN_CLICKED(IDC_FLISTEN, OnBnClicked_Filter_Listen)
ON_BN_CLICKED(IDC_FNOFF, OnBnClicked_Filter_NoFF)
ON_BN_CLICKED(IDC_FNOPUNKBUSTER, OnBnClicked_Filter_NoPunkBuster)
*/

ON_BN_CLICKED(IDC_FEMPTY, OnFilterChanged)
ON_BN_CLICKED(IDC_FFULL, OnFilterChanged)
ON_BN_CLICKED(IDC_FPASSWORD, OnFilterChanged)
ON_BN_CLICKED(IDC_FNOPASSWORD, OnFilterChanged)
ON_BN_CLICKED(IDC_FLISTEN, OnFilterChanged)
ON_BN_CLICKED(IDC_FNOFF, OnFilterChanged)
ON_BN_CLICKED(IDC_FNOPUNKBUSTER, OnFilterChanged)

END_MESSAGE_MAP()


// CClientDlg message handlers
/////////////////////////////////////////////////////////
#include <dplay8.h>
#include <dxerr.h>
#include <vector>
IDirectPlay8Client 		*g_pClient;
IDirectPlay8Address		*g_pClientDeviceAddress	;

DPNHANDLE				g_dpnhEnum = NULL;
//-------------------------------------------------
xrCriticalSection	net_csEnumeration
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(net_csEnumeration))
#endif // PROFILE_CRITICAL_SECTIONS
;

struct HOST_NODE
{
	DPN_APPLICATION_DESC	dpAppDesc;
	IDirectPlay8Address*	pHostAddress;
	char					dpCharAddress[128];	
	char					dpHostName[128];
	char					dpSessionName[128];
	char					dpServerName[128];
	GAME_TYPE				dpServerGameType;
	int						dpServerNumPlayers;
	int						dpServerMaxPlayers;
	bool					dpDedicated;
	bool					dpFFire;
	INT						dpPassword;
	int						dpPing;
	int						dpPort;
	//-----------------------------------
	SBServer				dpServer;
};

#define	xr_vector	std::vector
typedef unsigned	int		u32;

//. this vector may be removed after memory manager 
//. is destroyed
xr_vector <HOST_NODE>	net_Hosts;

// {0218FA8B-515B-4bf2-9A5F-2F079D1759F3}
static const GUID STALKER_NET_GUID = 
{ 0x218fa8b,  0x515b, 0x4bf2, { 0x9a, 0x5f, 0x2f, 0x7, 0x9d, 0x17, 0x59, 0xf3 } };
//---------------------------------------------------------------


HRESULT WINAPI Network_MessageHandler(	PVOID pvUserContext,
									  	DWORD dwMessageType,
									  	PVOID pMessageData );
void CClientDlg::OnBnClickedRefresh()
{
	ClearHostList();

	m_pBtnJoin.EnableWindow(FALSE);

	if (!Client_Create()) return;
	
	m_pBtnRefresh.EnableWindow(FALSE);

	if (!Client_EnumHosts())
	{
		m_pBtnRefresh.EnableWindow(TRUE);
		Client_Close();
		return;
	};
	Client_Close();
	//-----------------------------------------------------
	m_pBtnRefresh.EnableWindow(TRUE);
	UpdateServersList();
};

void CClientDlg::UpdateServersList()	
{
	m_pServerList.SetRedraw(FALSE);
	//-------------------------------------------
	int			m_nItemSelected = -1;
	for (int i=0; i<m_pServerList.GetItemCount(); i++)
	{
		if (m_pServerList.GetItemState(i, (UINT)-1)& LVIS_SELECTED)
		{
			m_nItemSelected = i;
			break;
		}
	}
	//-------------------------------------------
	m_pServerList.DeleteAllItems();

	if (!net_Hosts.size()) 
	{
		m_pServerList.SetRedraw(TRUE);
		m_pServerList.Invalidate(TRUE);
		return;
	};
	
	int NumItems = 0;

	for (u32 I=0; I<net_Hosts.size(); I++)
	{
		HOST_NODE&	N = net_Hosts	[I];

		switch (N.dpServerGameType)
		{
//		case GAME_UNKNOWN:
		case GAME_END_LIST:
			continue;
		}

		if (!CheckFilter(&N))
		{
			continue;
		}
		
		LVITEM iItem;
		iItem.mask = LVIF_TEXT | LVIF_IMAGE;
		iItem.iItem = NumItems;
		iItem.iSubItem = 0;
		iItem.pszText = N.dpServerName;
		iItem.iImage = (N.dpPassword == 0) ? 0 : 1;

		CString tmp;
		tmp.Format("%d/%d", N.dpServerNumPlayers, (N.dpServerMaxPlayers == 0) ? 32 : N.dpServerMaxPlayers);

		m_pServerList.InsertItem(&iItem);
		m_pServerList.SetItemText(NumItems, 0, N.dpServerName);
		m_pServerList.SetItemText(NumItems, 1, N.dpSessionName);
		m_pServerList.SetItemText(NumItems, 2, g_GameTypeName[N.dpServerGameType]);
		m_pServerList.SetItemText(NumItems, 3, tmp);
		tmp.Format("%d", N.dpPing);
		m_pServerList.SetItemText(NumItems, 4, tmp);
//		tmp.Format("%s", N.dpPassword ? "Yes" : "No");
//		m_pServerList.SetItemText(NumItems, 5, tmp);
		m_pServerList.SetItemData(NumItems, I);

		NumItems++;
	};
//	m_pServerList.SetItemState(NumItems-1, LVIS_SELECTED)

	if (m_nItemSelected != -1)
	{
		UINT NewItemState = m_pServerList.GetItemState(m_nItemSelected, (UINT)-1) | LVIS_SELECTED;
		m_pServerList.SetItemState(m_nItemSelected, NewItemState, (UINT)-1);
	}

	m_pServerList.SetRedraw(TRUE);
	m_pServerList.Invalidate(TRUE);
};

void CClientDlg::RemoveServerFromList	(SBServer server)
{
	xr_vector <HOST_NODE>::iterator it = net_Hosts.begin();
	for (;it!=net_Hosts.end(); ++it)
	{
		HOST_NODE&	N = *(it);//net_Hosts	[I];
		//		if	( !stricmp(N.dpCharAddress, NODE->dpCharAddress))
		if (N.dpServer == server)
		{
			net_Hosts.erase(it);
			break;
		}
	};
};

static bool inAdd = false;
void CClientDlg::AddServerToList		(SBServer server)
{
	if (!server) return;
	net_csEnumeration.Enter			();

	HOST_NODE	NewNode, *NODE = &NewNode;
	ZeroMemory	(NODE, sizeof(HOST_NODE));
	sprintf(NODE->dpCharAddress, "%s:%d", SBServerGetPublicAddress(server), SBServerGetPublicQueryPort(server));
	sprintf(NODE->dpHostName, "%s", SBServerGetPublicAddress(server));
	//---------------------------------------------------------------------
	bool new_server = true;
	for (u32 I=0; I<net_Hosts.size(); I++)
	{
		HOST_NODE&	N = net_Hosts	[I];
		//		if	( !stricmp(N.dpCharAddress, NODE->dpCharAddress))
		if (N.dpServer == server)
		{
			new_server = false;
			NODE = &N;
			break;
		}
	};
	//---------------------------------------------------------------------
	sprintf(NODE->dpServerName, "%s", SBServerGetStringValue(server, qr2_registered_key_list[HOSTNAME_KEY], NODE->dpHostName));
	//  [5/20/2005]
	char dbgStr[1024];
	sprintf(dbgStr, "%s - %s\n", NODE->dpHostName, NODE->dpServerName);
	OutputDebugString(dbgStr);
	//  [5/20/2005]

	sprintf(NODE->dpSessionName, "%s", SBServerGetStringValue(server, qr2_registered_key_list[MAPNAME_KEY], "Unknown"));
	
	NODE->dpServerGameType = GetGameType((char*)SBServerGetStringValue(server, qr2_registered_key_list[GAMETYPE_NAME_KEY], "UNKNOWN"));
	string1024 xtmp;
	strcpy(xtmp, (char*)SBServerGetStringValue(server, qr2_registered_key_list[GAMETYPE_KEY], ""));

	NODE->dpServerGameType = GetGameType((byte)SBServerGetIntValue(server, qr2_registered_key_list[GAMETYPE_KEY], 0));

	NODE->dpPassword	= SBServerGetBoolValue(server, qr2_registered_key_list[PASSWORD_KEY], SBFalse);
	NODE->dpPing = SBServerGetPing(server);
	NODE->dpServerNumPlayers = SBServerGetIntValue(server, qr2_registered_key_list[NUMPLAYERS_KEY], 0);
	NODE->dpServerMaxPlayers = SBServerGetIntValue(server, qr2_registered_key_list[MAXPLAYERS_KEY], 32);
	NODE->dpDedicated	= SBServerGetBoolValue(server, qr2_registered_key_list[DEDICATED_KEY], SBFalse) == SBTrue;
	NODE->dpFFire		= SBServerGetBoolValue(server, qr2_registered_key_list[G_FRIENDLY_FIRE_KEY], SBFalse) == SBTrue;
	NODE->dpPort		= SBServerGetIntValue(server, qr2_registered_key_list[HOSTPORT_KEY], 0);
	NODE->dpServer = server;

	char PlayerName[1024] = "";
	sprintf(PlayerName, "%s", SBServerGetPlayerStringValue(server, 0, "player", "Unknown"));

	SBBool HasFullKeys = SBServerHasFullKeys(server);
	if (new_server)
		net_Hosts.push_back(*NODE);
	else
	{
 		int x=0;
		x=x;
	}

	if (m_bQuickRefresh)
	{
		m_pGameSpyUpdateList.EnableWindow(TRUE);
		m_pGameSpyRefreshList.EnableWindow(TRUE);
		m_bQuickRefresh = false;
	};
	net_csEnumeration.Leave			();
};

void		CClientDlg::RefillServersList			()
{
	OutputDebugString("CClientDlg::RefillServersList \n");
	ClearHostList();
	// clear the server count
	//	m_serverCount = 0;
	//	m_servers.SetWindowText("0");

	// go through the list of servers
	for(int i = 0; i < ServerBrowserCount(m_serverBrowser) ; i++)
	{
		// if we got basic info for it, put it back in the list
		SBServer server = ServerBrowserGetServer(m_serverBrowser, i);
		//		if(SBServerHasBasicKeys(server))
		AddServerToList(server);//, FALSE);
	}

	UpdateServersList();
};

//void CClientDlg::OnLvnItemchangedServerslist(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//
//	POSITION pos = m_pServerList.GetFirstSelectedItemPosition();
//	if (pos == NULL) return;
//
//	u32 nItem = (u32) m_pServerList.GetNextSelectedItem(pos);
//	if (nItem < 0 || nItem >= net_Hosts.size()) return;
//}

void CClientDlg::OnNMClickServerslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMITEMACTIVATE *pAct = (NMITEMACTIVATE*)pNMHDR;
	*pResult = 0;

	if (pAct->iItem >= 0 && u32(pAct->iItem) <net_Hosts.size())
	{
		m_pBtnJoin.EnableWindow(TRUE);
	}
	else
	{
		m_pBtnJoin.EnableWindow(FALSE);
	};
}

void CClientDlg::OnNMRclickServerslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
};

void CClientDlg::OnLvnItemchangedServerslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CClientDlg::OnLvnColumnclickServerslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	// track ascending/descending
	static bool ascending = false;
	ascending = !ascending;

	switch(pNMLV->iSubItem)
	{
	case COL_SERVERNAME :
		{
			ServerBrowserSort(m_serverBrowser, (SBBool)ascending, (char*)qr2_registered_key_list[HOSTNAME_KEY], sbcm_stricase);
		}break;
	case COL_MAPNAME :
		{
			ServerBrowserSort(m_serverBrowser, (SBBool)ascending, (char*)qr2_registered_key_list[MAPNAME_KEY], sbcm_stricase);
		}break;
	case COL_GAMETYPE :
		{
			ServerBrowserSort(m_serverBrowser, (SBBool)ascending, (char*)qr2_registered_key_list[GAMETYPE_KEY], sbcm_stricase);
		}break;
	case COL_PLAYERS :
		{
			ServerBrowserSort(m_serverBrowser, (SBBool)ascending, (char*)qr2_registered_key_list[NUMPLAYERS_KEY], sbcm_int);
		}break;
	case COL_PING :
		{
			ServerBrowserSort(m_serverBrowser, (SBBool)ascending, "ping", sbcm_int);
		}break;
	}

	RefillServersList			();  

	*pResult = 0;
}

void CClientDlg::OnBnClickedJoin()
{
	m_pBtnJoin.EnableWindow(FALSE);
	POSITION pos = m_pServerList.GetFirstSelectedItemPosition();
	if (pos == NULL) return;
	
	u32 nItem = (u32) m_pServerList.GetNextSelectedItem(pos);
	if (nItem < 0 || nItem >= net_Hosts.size()) return;

	int iCatchInput = 0;
	int NameLen = 0;
	int iPrefetch = 0;
	int iR2 = 0;
	int iDistort = 1;
	char sLogsPath[1024] = "";
	char Name[1024];
	char NameAdd[1024];
	char sCDKeyStr[1024] = "";
	CStalker_netDlg* pMainDlg = (CStalker_netDlg*) (GetParent()->GetParent());
	if (pMainDlg)
	{
		iCatchInput = pMainDlg->m_pCatchInput.GetCheck();
		NameLen = pMainDlg->m_pPlayerName.GetWindowTextLength();
		if (NameLen) 
		{
			pMainDlg->m_pPlayerName.GetWindowText(Name, 1023);
			Name[32]=0;
			sprintf(NameAdd, "/name=%s", Name);
		};
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
			if (0 != xr_strcmp(tmp, "- No CD Key -"))
				sprintf(sCDKeyStr, "/cdkey=%s", tmp);
		}
	};

	//-noprefetch
	char cmdline[1024];
	HOST_NODE	N = net_Hosts[m_pServerList.GetItemData(nItem)];
	char	PortStr[1024];
	if (N.dpPort != 0)
	{
		sprintf(PortStr, "/port=%d", N.dpPort);
	}

	char PswStr[1024] = "";
	if (N.dpPassword != 0)
	{
		CPasswordDlg dlg;
		char Psw[64];
		INT_PTR nResponse = dlg.DoModal(Psw);
		if (nResponse == IDOK)
		{
			sprintf(PswStr, "/psw=%s", Psw);
		};
		if (nResponse == IDCANCEL) return;
	};	
	/* Server(%s/deathmatch)*/ 
	sprintf(cmdline, "xr_3da.exe -xclsx %s%s%s%s%s %s -nocache -external -start client(%s%s%s%s%s)", //server/name
							(iCatchInput == 1) ? "-i " : "",
							(iPrefetch == 1) ? "" : "-noprefetch ",
							(iR2 == 1) ? "-r2" : "",
							(iDistort == 1) ? "" : "-nodistort",
							(sLogsPath[0] == 0) ? "" : sLogsPath,
							//-------------------------------------
							LTX, 
							//-------------------------------------
///							N.dpSessionName,
							N.dpHostName,//N.dpServerName,
							(NameLen) ? NameAdd : "",
							(N.dpPort!=0) ? PortStr : "",
							(N.dpPassword != 0) ? PswStr : "",
							(sCDKeyStr[0]) ? sCDKeyStr : ""
							);

	OutputDebugString( cmdline );
	int res = WinExec(cmdline, SW_SHOW);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL	CClientDlg::Client_Create()
{
	//+++++++++++++++++++ Create a DirectPlay Client Object ++++++++++++++++++++++++//
	// Create the IDirectPlay8Client object.
	HRESULT hr = CoCreateInstance(CLSID_DirectPlay8Client, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IDirectPlay8Client, 
		(LPVOID*) &g_pClient);

	if (FAILED(hr)) return false;

	// Initialize IDirectPlay8Client object.

	DWORD	dwFlags			= 0;
	//		dwFlags				|= DPNINITIALIZE_HINT_LANSESSION;
	//		dwFlags				|= DPNINITIALIZE_DISABLELINKTUNING;

	hr = g_pClient->Initialize(NULL, ( PFNDPNMESSAGEHANDLER)Network_MessageHandler, dwFlags );
	if (FAILED(hr))
	{
		Client_Close();
		return false;
	};

	//++++++++++++++++++++ Create the Client Device Address Object ++++++++++++++++++++++++//
	// Create our IDirectPlay8Address Device Address
	hr = CoCreateInstance(	CLSID_DirectPlay8Address, NULL,
		CLSCTX_INPROC_SERVER,
		IID_IDirectPlay8Address,
		(LPVOID*) &g_pClientDeviceAddress ); 

	if (FAILED(hr))
	{
		Client_Close();
		return false;
	};

	// Set the SP for our Device Address
	hr = g_pClientDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP );
	if (FAILED(hr))
	{
		Client_Close();
		return false;
	};
	
	hr = g_pClientDeviceAddress->SetSP(&CLSID_DP8SP_TCPIP );
	if (FAILED(hr))
	{
		Client_Close();
		return false;
	};
/*
	u32 c_port			= 5445+1;
	hr = g_pClientDeviceAddress->AddComponent	(DPNA_KEY_PORT, &c_port, sizeof(c_port), DPNA_DATATYPE_DWORD );
	if (FAILED(hr))
	{
		Client_Close();
		return false;
	};
*/
	char URLA[1024];
	DWORD Len;
	Len = 1024;
	hr = g_pClientDeviceAddress->GetURLA(URLA, &Len);
	// Set the Port for our Device Address
	/*
	DWORD dwPort = 2333;
	hr = g_pClientDeviceAddress->AddComponent(DPNA_KEY_PORT, &dwPort, sizeof(dwPort),DPNA_DATATYPE_DWORD);
	if (FAILED(hr))
	{
	tntConsole_Print("\nAnable to set PORT .");
	tntNetwork_Client_Close();
	return hr;
	};
	*/

	Len = 1024;
	hr = g_pClientDeviceAddress->GetURLA(URLA, &Len);

	DPN_SP_CAPS	dnSPCaps;
	Memory.mem_fill(&dnSPCaps, 0, sizeof(dnSPCaps));
	dnSPCaps.dwSize = sizeof(dnSPCaps);

	hr = g_pClient->GetSPCaps(&CLSID_DP8SP_TCPIP, &dnSPCaps, 0);
	/////////// Setting Client Info //////////////
/*
	DPN_PLAYER_INFO	PlayerInfo;
	memset(&PlayerInfo, 0, sizeof(PlayerInfo));
	PlayerInfo.dwSize = sizeof(PlayerInfo);
	PlayerInfo.dwInfoFlags = DPNINFO_NAME | DPNINFO_DATA;
	PlayerInfo.pwszName = (WCHAR*) g_pClientDesc.ServerName;
	PlayerInfo.dwPlayerFlags = 0;
	PlayerInfo.dwDataSize = sizeof(g_ClientDesc);
	PlayerInfo.pvData = (void*) &g_ClientDesc;

	hr = g_pClient->SetClientInfo(&PlayerInfo , NULL, NULL, DPNSETCLIENTINFO_SYNC);
	if (FAILED(hr))
	{
		
	};
*/
	DPN_PLAYER_INFO				Pinfo;
	ZeroMemory					(&Pinfo,sizeof(Pinfo));
	Pinfo.dwSize				= sizeof(Pinfo);
	Pinfo.dwInfoFlags			= DPNINFO_NAME;
//	sprintf(Pinfo.pwszName, "Stalket_net");
	Pinfo.pwszName				= (WCHAR*) "Stalker_net";//ClientNameUNICODE;
	hr = g_pClient->SetClientInfo	(&Pinfo,0,0,DPNSETCLIENTINFO_SYNC);
	if (FAILED(hr))
	{
		Client_Close();
		return false;
	};

	return true;
};

BOOL	CClientDlg::Client_Close()
{
	if (g_pClient) 
	{
		if (g_dpnhEnum) g_pClient->CancelAsyncOperation(g_dpnhEnum, DPNCANCEL_ENUM);
		g_dpnhEnum = NULL;

		g_pClient->Close(0);
		g_pClient->Release();
	}	
	g_pClient = NULL;
	if (g_pClientDeviceAddress) g_pClientDeviceAddress->Release();
	g_pClientDeviceAddress = NULL;
	return true;
};

void	CClientDlg::ClearHostList()
{
	// Clean up Host _list_
	net_csEnumeration.Enter			();
	for (u32 i=0; i<net_Hosts.size(); i++) {
		HOST_NODE&	N = net_Hosts[i];
		if (N.pHostAddress) N.pHostAddress->Release();
	}
	net_Hosts.clear					();
	net_csEnumeration.Leave			();
};

BOOL	CClientDlg::Client_EnumHosts()
{
	if (!g_pClient) return false;
	///////////////////////////////////////////////////////////////////
	DPN_APPLICATION_DESC AppDesc;
	ZeroMemory(&AppDesc, sizeof(DPN_APPLICATION_DESC));
	AppDesc.dwSize = sizeof(DPN_APPLICATION_DESC);
	AppDesc.guidApplication = STALKER_NET_GUID;

	HRESULT hr;

	IDirectPlay8Address		*net_Address_server = NULL;
	u32 psNET_Port = 5445;
	char Name[1024];
	sprintf(Name, "%s", "");
	hr = (CoCreateInstance	(CLSID_DirectPlay8Address,NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay8Address,(LPVOID*) &net_Address_server )); 
	hr = (net_Address_server->SetSP			(&CLSID_DP8SP_TCPIP  ));
	hr = (net_Address_server->AddComponent	(DPNA_KEY_PORT,	&psNET_Port, sizeof(psNET_Port), DPNA_DATATYPE_DWORD ));
//	hr = (net_Address_server->AddComponent	(DPNA_KEY_HOSTNAME, Name, (strlen(Name) + 1), DPNA_DATATYPE_STRING ));
	
	if (g_dpnhEnum) g_pClient->CancelAsyncOperation(g_dpnhEnum, DPNCANCEL_ENUM);
	g_dpnhEnum = NULL;
	GUID tmpGuid = STALKER_NET_GUID;
	hr = g_pClient->EnumHosts(	&AppDesc,  // pApplicationDesc
		net_Address_server,  // Host Address
		g_pClientDeviceAddress,// Device Address
		&tmpGuid, sizeof(STALKER_NET_GUID), //NULL, 0,
		1,         // dwEnumCount
		0,         // dwRetryInterval
		1500,      // dwTimeOut
		NULL,      // pvUserContext
		NULL,//&m_dpnhEnum,      // pAsyncHandle
		DPNENUMHOSTS_SYNC// | DPNENUMHOSTS_OKTOQUERYFORADDRESSING
		); // dwFlags 
	/////////////////////////////////////////////////////////////////
//	logfilePrint("end enum call");
//	char Text[256];
//	sprintf(Text, "Result code %X", HRESULT_CODE(hr));
//	logfilePrint(Text);

	switch (hr)
	{
	case S_OK:
		{
//			tntConsole_Print("\nEnum Ok.");
//			sprintf(m_pEnumEndCommand, "%s", pEndEnumCommand);
//			logfilePrint("S_OK");
			int x=0;
			x=x;
		}break;
	case DPNERR_INVALIDDEVICEADDRESS:
		{
//			logfilePrint("DPNERR_INVALIDDEVICEADDRESS");
//			tntConsole_Print("\nEnum DPNERR_INVALIDDEVICEADDRESS.");
			int x=0;
			x=x;
		}break;
	case DPNERR_INVALIDFLAGS:
		{
//			logfilePrint("DPNERR_INVALIDFLAGS");
//			tntConsole_Print("\nEnum DPNERR_INVALIDFLAGS.");
			int x=0;
			x=x;
		}break;
	case DPNERR_INVALIDHOSTADDRESS:
		{
//			logfilePrint("DPNERR_INVALIDHOSTADDRESS");
//			tntConsole_Print("\nEnum DPNERR_INVALIDHOSTADDRESS.");
			int x=0;
			x=x;
		}break;
	case DPNERR_INVALIDPARAM:
		{
//			logfilePrint("DPNERR_INVALIDPARAM");
//			tntConsole_Print("\nEnum DPNERR_INVALIDPARAM.");
			int x=0;
			x=x;
		}break;
	case DPNERR_ENUMQUERYTOOLARGE:
		{
//			logfilePrint("DPNERR_ENUMQUERYTOOLARGE");
//			tntConsole_Print("\nEnum DPNERR_ENUMQUERYTOOLARGE.");
			int x=0;
			x=x;
		}break;
	case DPNSUCCESS_PENDING :
		{
//			logfilePrint("DPNSUCCESS_PENDING");
//			tntConsole_Print("\nEnum DPNSUCCESS_PENDING .");
//			sprintf(m_pEnumEndCommand, "%s", pEndEnumCommand);
			int x=0;
			x=x;
		}break;
	case DPNERR_USERCANCEL:
		{
			int x=0;
			x=x;
		}break;
	};
	if (hr != S_OK)
	{
		const char* text = DXGetErrorString(hr);
		int x=0;
		x=x;
	}
	net_Address_server->Release();
	return true;
};

void	ConvertAddressToName(char* URLA, char* ServerName)
{
	if (!URLA || !ServerName) return;
	char Address[1024];
	Memory.mem_fill(Address, 0, 1024);
	char *pAddress = Address;
	char* pURLA = strstr(URLA, "hostname=");
	if (!pURLA) return;
	pURLA = strstr(pURLA, "=");
	if (!pURLA) return;
	pURLA++;
	char* pTmp = strstr(pURLA, ";");
	int Len = (int)xr_strlen(pURLA);
	if (pTmp) Len = int(pTmp - pURLA);
	
	memcpy(Address, pURLA, pTmp - pURLA);
	Address[pTmp - pURLA+1] = 0;

	char xAddr[4];
	xAddr[0] = atoi(pAddress); pAddress = strstr(pAddress, ".")+1;
	xAddr[1] = atoi(pAddress); pAddress = strstr(pAddress, ".")+1;
	xAddr[2] = atoi(pAddress); pAddress = strstr(pAddress, ".")+1;
	xAddr[3] = atoi(pAddress); 

	hostent *pHostEnt = gethostbyaddr(xAddr, 4, AF_INET );
	if (!pHostEnt) return;
	sprintf(ServerName, "%s", pHostEnt->h_name);

	if (strstr(ServerName, ".gsc")) *(strstr(ServerName, ".gsc")) = 0;
};

HRESULT WINAPI Network_MessageHandler(	PVOID pvUserContext,
										 DWORD dwMessageType,
										 PVOID pMessageData )
{
	switch (dwMessageType)
	{
	case DPN_MSGID_ENUM_HOSTS_QUERY:
		{
		}break;
	case DPN_MSGID_RETURN_BUFFER:
		{
		}break;
	case DPN_MSGID_ENUM_HOSTS_RESPONSE:
		{
			PDPNMSG_ENUM_HOSTS_RESPONSE     pEnumHostsResponseMsg;
			const DPN_APPLICATION_DESC*     pDesc;
			// HOST_NODE*                      pHostNode = NULL;
			// WCHAR*                          pwszSession = NULL;

			pEnumHostsResponseMsg			= (PDPNMSG_ENUM_HOSTS_RESPONSE) pMessageData;
			pDesc							= pEnumHostsResponseMsg->pApplicationDescription;

			// Insert each host response if it isn't already present
			net_csEnumeration.Enter			();
			BOOL	bHostRegistered			= FALSE;
			for (u32 I=0; I<net_Hosts.size(); I++)
			{
				HOST_NODE&	N = net_Hosts	[I];
				if	( pDesc->guidInstance == N.dpAppDesc.guidInstance)
				{
					// This host is already in the list
					bHostRegistered = TRUE;
					break;
				}
			}

			if (!bHostRegistered) 
			{
				// This host session is not in the list then so insert it.
				HOST_NODE	NODE;
				ZeroMemory	(&NODE, sizeof(HOST_NODE));

				// Copy the Host Address
				HRESULT hr = pEnumHostsResponseMsg->pAddressSender->Duplicate(&NODE.pHostAddress );
				memcpy(&NODE.dpAppDesc,pDesc,sizeof(DPN_APPLICATION_DESC));

				// Null out all the pointers we aren't copying
				NODE.dpAppDesc.pwszSessionName					= NULL;
				NODE.dpAppDesc.pwszPassword						= NULL;
				NODE.dpAppDesc.pvReservedData					= NULL;
				NODE.dpAppDesc.dwReservedDataSize				= 0;
//				NODE.dpAppDesc.pvApplicationReservedData		= NULL;
//				NODE.dpAppDesc.dwApplicationReservedDataSize	= 0;

				char URLA[1024];
				DWORD Len;
				Len = 1024;
				hr = NODE.pHostAddress->GetURLA(URLA, &Len);

				ConvertAddressToName(URLA, NODE.dpServerName);
				///////////////////////////////////////////////////////////
				NODE.dpServerNumPlayers = NODE.dpAppDesc.dwCurrentPlayers-1;
				///////////////////////////////////////////////////////////
				NODE.dpServerGameType = GAME_UNKNOWN;
				if (NODE.dpAppDesc.pvApplicationReservedData != 0)
				{
					char GameType[1024];
					strcpy(GameType, (char*)NODE.dpAppDesc.pvApplicationReservedData);
					if (strstr(GameType, "/")) *(strstr(GameType, "/")) = 0;

					NODE.dpServerGameType = GetGameType(GameType);
				}
				///////////////////////////////////////////////////////////


				if( pDesc->pwszSessionName)
					WideCharToMultiByte(CP_ACP,0,pDesc->pwszSessionName,-1,NODE.dpSessionName,sizeof(NODE.dpSessionName),0,0);

				net_Hosts.push_back			(NODE);
			}
			net_csEnumeration.Leave			();
		}break;
	case DPN_MSGID_INDICATE_CONNECT:
		{
		}break;
	case DPN_MSGID_CREATE_PLAYER:
		{
		}break;
	case DPN_MSGID_CONNECT_COMPLETE :
		{
		}break;
	case DPN_MSGID_DESTROY_PLAYER:
		{
		}break;
	case DPN_MSGID_ASYNC_OP_COMPLETE:
		{
		}break;
	case DPN_MSGID_RECEIVE :
		{
		}break;
	case DPN_MSGID_SEND_COMPLETE :
		{
		}break;
	case DPN_MSGID_REMOVE_PLAYER_FROM_GROUP:
		{
		}break;
	case DPN_MSGID_TERMINATE_SESSION :
		{
		}break;
	default:
		{
			//tntConsole_UrgentPrint("Unknown Message.");
		}break;
	};
	return S_OK;
};

void CClientDlg::OnBnClickedInternet()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_INTERNET, IDC_LAN, IDC_INTERNET);
}

void CClientDlg::OnBnClickedLan()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton(IDC_INTERNET, IDC_LAN, IDC_LAN);
}

void CClientDlg::OnBnClickedGSUpdateList()
{
	UpdateData();
	
	// if we're doing an update, cancel it
	SBState state = ServerBrowserState(m_serverBrowser);
	if((state != sb_connected) && (state != sb_disconnected))
	{
		ServerBrowserHalt(m_serverBrowser);
//		m_pGameSpyUpdateList.EnableWindow(TRUE);
//		m_pGameSpyRefreshList.EnableWindow(TRUE);
//		MessageBox("GameSpy Refresh Stopped\n");

//		m_pGameSpyUpdateList.EnableWindow(TRUE);
//		m_pGameSpyRefreshList.EnableWindow(TRUE);
		if (m_timerID)
			KillTimer(m_timerID);
		m_timerID = 0;

//		return;
	};

//	m_pGameSpyUpdateList.EnableWindow(FALSE);
//	m_pGameSpyRefreshList.EnableWindow(FALSE);

	// clear the server browser and list
	ServerBrowserClear(m_serverBrowser);
	ClearHostList();
	m_pBtnJoin.EnableWindow(FALSE);

	// set a timer
	if(!m_timerID)
		m_timerID = SetTimer(TIMER_ID, TIMER_FREQUENCY, NULL);

	// fields we're interested in
	unsigned char fields[] = { HOSTNAME_KEY, NUMPLAYERS_KEY, MAXPLAYERS_KEY, MAPNAME_KEY, GAMETYPE_KEY, PASSWORD_KEY, HOSTPORT_KEY };
	int numFields = sizeof(fields) / sizeof(fields[0]);

	// check for internet/lan
	bool internet = (IsDlgButtonChecked(IDC_INTERNET) == BST_CHECKED);

	// do an update
	SBError error;
	if(internet)
		error = ServerBrowserUpdate(m_serverBrowser, SBTrue, SBFalse, fields, numFields, (char *)(const char *)"");
	else
		error = ServerBrowserLANUpdate(m_serverBrowser, SBTrue, START_PORT_LAN, END_PORT_LAN);

	if (error != sbe_noerror)
	{
		MessageBox("GameSpy Error!", "Error", MB_OK);
		m_pGameSpyUpdateList.EnableWindow(TRUE);
		m_pGameSpyRefreshList.EnableWindow(TRUE);
		if (m_timerID)
			KillTimer(m_timerID);
		m_timerID = 0;
	}
}

void CClientDlg::OnBnClickedGSRefreshList()
{
	if (!net_Hosts.size()) return;
	int nItem = -1;
	for (int i=0; i<m_pServerList.GetItemCount(); i++)
	{
		if (m_pServerList.GetItemState(i, (UINT)-1) & LVIS_SELECTED)
		{
			nItem = i;
			break;
		}
	}

	if (nItem == -1) return;

	HOST_NODE	N = net_Hosts[m_pServerList.GetItemData(nItem)];
	UINT res = m_pServerList.GetItemState(nItem, (UINT)-1);
	if (!N.dpServer) return;

//	m_pGameSpyUpdateList.EnableWindow(FALSE);
//	m_pGameSpyRefreshList.EnableWindow(FALSE);

	m_bQuickRefresh = TRUE;
	ServerBrowserAuxUpdateServer(m_serverBrowser, N.dpServer, SBTrue, SBTrue);

//	if(!m_timerID)
//		m_timerID = SetTimer(TIMER_ID, TIMER_FREQUENCY, NULL);
//	m_pGameSpyUpdateList.EnableWindow(TRUE);
//	m_pGameSpyRefreshList.EnableWindow(TRUE);

}

void CClientDlg::OnGameSpyUpdateComplete()
{
	m_pGameSpyUpdateList.EnableWindow(TRUE);
	m_pGameSpyRefreshList.EnableWindow(TRUE);
	if (m_timerID != 0) 
	{
		KillTimer(m_timerID);
		m_timerID = 0;
	};

//	UpdateServersList();
	RefillServersList();
};

void CClientDlg::OnTimer(UINT nIDEvent)
{
	// think if our timer was called
	if(nIDEvent == m_timerID)
		ServerBrowserThink(m_serverBrowser);

	CSubDlg::OnTimer(nIDEvent);
}
//---------------------------------------------------------------------------------


/*
void CClientDlg::OnBnClicked_Filter_Empty()
{
	// TODO: Add your control notification handler code here
}

void CClientDlg::OnBnClicked_Filter_Full()
{
	// TODO: Add your control notification handler code here
}

void CClientDlg::OnBnClicked_Filter_Password_With()
{
	// TODO: Add your control notification handler code here
}

void CClientDlg::OnBnClicked_Filter_Password_Without()
{
	// TODO: Add your control notification handler code here
}

void CClientDlg::OnBnClicked_Filter_Listen()
{
	// TODO: Add your control notification handler code here
}

void CClientDlg::OnBnClicked_Filter_NoFF()
{
	// TODO: Add your control notification handler code here
}

void CClientDlg::OnBnClicked_Filter_NoPunkBuster()
{
	// TODO: Add your control notification handler code here
}
*/
void	CClientDlg::CheckFilterButtons	()
{
	ViewFilters.Empty				 = (m_pFilter_Empty.GetCheck				() != 0);
	ViewFilters.Full				 = (m_pFilter_Full.GetCheck					() != 0);
	ViewFilters.Password_With		 = (m_pFilter_Password_With.GetCheck		() != 0);
	ViewFilters.Password_Without	 = (m_pFilter_Password_Without.GetCheck		() != 0);
	ViewFilters.Listen				 = (m_pFilter_Listen.GetCheck				() != 0);
	ViewFilters.NoFF				 = (m_pFilter_NoFF.GetCheck					() != 0);
	ViewFilters.PunkBuster_Without	 = (m_pFilter_PunkBuster_Without.GetCheck	() != 0);	
};

bool	CClientDlg::CheckFilter				(void* PHost)
{
	HOST_NODE* pHost = (HOST_NODE*)PHost;
	if (!pHost) return true;

	if (!ViewFilters.Empty				&& !pHost->dpServerNumPlayers) return false;
	if (!ViewFilters.Full				&& pHost->dpServerMaxPlayers == pHost->dpServerNumPlayers) return false;
	if (!ViewFilters.Password_With		&& pHost->dpPassword != 0) return false;
	if (!ViewFilters.Password_Without	&& pHost->dpPassword != 1) return false;
	if (!ViewFilters.Listen				&& !pHost->dpDedicated) return false;
	if (!ViewFilters.NoFF				&& !pHost->dpFFire)		return false;
//	if (!ViewFilters.PunkBuster_Without	 

	return true;
}

void	CClientDlg::OnFilterChanged			()
{
	CheckFilterButtons();
	UpdateServersList();
};