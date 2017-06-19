// Stalker_netDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ServerDlg.h"
#include "ClientDlg.h"
#include "CDKeyDlg.h"


// CStalker_netDlg dialog
class CStalker_netDlg : public CDialog
{
// Construction
public:
	CStalker_netDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STALKER_NET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	///////////////////////////////////////////////
public:
	CTabCtrl m_pTabCtrl;
	CServerDlg		*m_pServerDlg;
	CClientDlg		*m_pClientDlg;
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedScatchinput();
	CButton m_pCatchInput;
	CEdit m_pPlayerName;
	afx_msg void OnBnClickedButton1();
	CButton m_pBuild;
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	CCDKeyDlg		*m_pCDKeyDlg;	
protected:
	virtual void OnCancel();
	virtual void OnOK();
	bool	m_bCloseAllowed;
public:
	afx_msg void OnClose();
protected:
	void	CallToEnterCDKey	();
	void	GetCDKey			(char* CDKeyStr);
	void	CreateCDKeyEntry	();
private:
public:
	CButton m_pCDKeyBtn;
//	afx_msg void OnBnClickedBuild2();
	CButton m_pPrefetch;
	CButton m_pR2;
	CButton m_pDistort;
	CButton m_pLogsPath;
	afx_msg void OnBnClickedLogsPath();
};
