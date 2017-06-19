#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "SubDlg.h"
#include "GameTypes.h"
#include "SVServerOptDlg.h"
#include "SVGameTypeDlg.h"
#include "SVGameOptDlg.h"
#include "SVRespawnDlg.h"
#include "SVWeatherOptions.h"
#include "SVSpectatorDlg.h"
#include "ServerOpts.h"
// CServerDlg dialog



class CServerDlg : public CSubDlg
{
	DECLARE_DYNAMIC(CServerDlg)

public:
	CServerDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CServerDlg();

// Dialog Data
	enum { IDD = IDD_SERVERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void	LoadMapList();
	void	ParseWeather(char** s, char*e);
	bool	GetMapName	(char* pMapName);


	DECLARE_MESSAGE_MAP()
	CButton m_pStartServerBtn;
	CListBox m_pMapList;

	char	m_Maps[GAME_END_LIST][20][1024];
	int		m_MapsNum[GAME_END_LIST];	

	DEF_VECTOR(shared_str_vec, shared_str)
	DEF_MAP(storage_map, EGameTypes, shared_str_vec)
	storage_map				m_maps;


	BOOL	AddDlg(CSubDlg* pNewDlg, CWnd* ParentWnd, int &CX, int &CY);

public:
	void	UpdateMapList(GAME_TYPE GameType);

	afx_msg void OnBnClickedStartServer();
	afx_msg void OnLbnSelchangeMapList();

//	CTabCtrl m_pSVTabCtrl;
//	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);

	GAME_TYPE	m_GameType;

	SVServerOptDlg	*m_pSVServerOptDlg;
	SVGameTypeDlg	*m_pSVGameTypeDlg;
	SVGameOptDlg	*m_pSVGameOptDlg;
	SVRespawnDlg	*m_pSVRepawnDlg;
	SVSpectatorDlg	*m_pSVSpectatorOptsDlg;
	ServerOpts2		*m_pSVO2;

	SVWeatherOptions *m_pSVWeatherOptDlg;

	void		SwitchGameType(GAME_TYPE NewGameType);
//	afx_msg void OnBnSetfocusStartserver();
//	afx_msg void OnBnClickedButton1();
//	afx_msg void OnTtnGetDispInfoCustom1(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedStartserver2();
	afx_msg void OnBnClickedServerOptions();
	CButton m_pServerOptionsButton;
	CListBox m_pMapList2;
	CButton m_p2List;
	CButton m_p2List2;
	afx_msg void OnBnClicked2List2();
	afx_msg void OnBnClicked2List();

	void	SaveMapList();
//	afx_msg void OnLbnDblclkMapList();
};
