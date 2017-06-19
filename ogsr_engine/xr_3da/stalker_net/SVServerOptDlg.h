#pragma once
#include "SubDlg.h"
#include "afxwin.h"


// SVServerOptDlg dialog

class SVServerOptDlg : public CSubDlg
{
	DECLARE_DYNAMIC(SVServerOptDlg)

public:
	SVServerOptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVServerOptDlg();

// Dialog Data
	enum { IDD = IDD_SERVEROPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	void	SetPort(CEdit* pPortItem, int NumPorts, int* UsedPorts);

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_pHostName;
	CEdit m_pPassword;
	CEdit m_pMaxPlayers;
	CButton m_pSpectrMode;
	CEdit m_pSpectrSwitchTime;
	CButton m_pDedicated;
	CButton m_pPublic;
	afx_msg void OnBnClickedSpectatoronly();
	CButton m_pCheckCDKey;
	afx_msg void OnBnClickedPublic();
//	CButton m_pMapRotation;
//	CEdit m_pMapRotationFile;
	afx_msg void OnBnClickedMapRotation();
	CButton m_pVotingAllowed;
	CEdit m_pSVPort;
	CEdit m_pGSPort;
	CEdit m_pCLPort;
	afx_msg void OnBnClickedSvPortButton();
	afx_msg void OnBnClickedClPortbutton();
	afx_msg void OnBnClickedGsPortbutton();
};
