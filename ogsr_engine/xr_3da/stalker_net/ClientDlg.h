#pragma once
#include "afxwin.h"
#include <afxcview.h>
#include "SubDlg.h"
//#include "GameTypes.h"

//=======================================
#include "GameSpy.net/ServerBrowsing/sb_serverbrowsing.h"
//=======================================
#define GAMETYPE_NAME_KEY						100
#define	DEDICATED_KEY							101
#define	G_FRIENDLY_FIRE_KEY						117
#pragma once
//#include <dplay8.h>
/*
#define IC			inline

class xrCriticalSection
{
private:
	CRITICAL_SECTION    m_CritSec;
public:
	xrCriticalSection	()
	{
		InitializeCriticalSection	( &m_CritSec );
	};

	~xrCriticalSection	()
	{
		DeleteCriticalSection		( &m_CritSec );
	};

	IC void		Enter	()
	{
		EnterCriticalSection		( &m_CritSec );
	};

	IC void		Leave	()
	{
		LeaveCriticalSection		( &m_CritSec );
	};
	IC BOOL		TryEnter()
	{
		return TryEnterCriticalSection( &m_CritSec );
	};
};
*/

// CClientDlg dialog

class CClientDlg : public CSubDlg
{
	DECLARE_DYNAMIC(CClientDlg)

public:
	CClientDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClientDlg();

// Dialog Data
	enum { IDD = IDD_CLIENTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

	CButton m_pBtnJoin;
	CButton m_pBtnRefresh;

	CRITICAL_SECTION		m_pCriticalSection;

	BOOL	Client_Create();
	BOOL	Client_Close();
	BOOL	Client_EnumHosts();

	void	UpdateServersList()	;
	void	ClearHostList();
//	GAME_TYPE	GetGameType (char* GameType);

	ServerBrowser m_serverBrowser;
	UINT_PTR m_timerID;
	
public:
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnDestroy();
protected:
//	int			m_nItemSelected;
	CListCtrl	m_pServerList;
	CImageList	m_pNewImageList;

	BOOL		m_bQuickRefresh;
public:
	
//	afx_msg void OnLvnItemchangedServerslist(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnItemActivateServerslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickServerslist(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_pStatic1;
	CStatic m_pStatic2;
	afx_msg void OnLvnItemchangedServerslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedJoin();

	CButton m_pGameSpyInternet;
	CButton m_pGameSpyLAN;
	afx_msg void OnBnClickedInternet();
	afx_msg void OnBnClickedLan();
	CButton m_pGameSpyUpdateList;
	CButton m_pGameSpyRefreshList;
	afx_msg void OnBnClickedGSUpdateList();
	afx_msg void OnTimer(UINT nIDEvent);
public:
	
	void		RefillServersList			();
	void		AddServerToList				(SBServer server);
	void		RemoveServerFromList		(SBServer server);
	void		OnGameSpyUpdateComplete		();
	afx_msg void OnLvnColumnclickServerslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickServerslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedGSRefreshList();
//------------------- Filters -------------------------------------------
protected:
	CButton m_pFilter_Empty;
	CButton m_pFilter_Full;
	CButton m_pFilter_Password_With;
	CButton m_pFilter_Password_Without;
	CButton m_pFilter_Listen;
	CButton m_pFilter_NoFF;
	CButton m_pFilter_PunkBuster_Without;
protected:
	typedef struct
	{
		bool Empty				;
		bool Full				;
		bool Password_With		;
		bool Password_Without	;
		bool Listen				;
		bool NoFF				;
		bool PunkBuster_Without	;
	}Filters;

	Filters		ViewFilters;
	void		OnFilterChanged			();
	void		CheckFilterButtons		();
	bool		CheckFilter				(void* PHost);
	/*
	afx_msg void OnBnClicked_Filter_Empty();
	afx_msg void OnBnClicked_Filter_Full();
	afx_msg void OnBnClicked_Filter_Password_With();
	afx_msg void OnBnClicked_Filter_Password_Without();
	afx_msg void OnBnClicked_Filter_Listen();
	afx_msg void OnBnClicked_Filter_NoFF();
	afx_msg void OnBnClicked_Filter_NoPunkBuster();
	*/
};
