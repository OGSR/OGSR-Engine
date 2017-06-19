#pragma once
#include "afxwin.h"
#include "SubDlg.h"


// SVRespawnDlg dialog

class SVRespawnDlg : public CSubDlg
{
	DECLARE_DYNAMIC(SVRespawnDlg)

public:
	SVRespawnDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVRespawnDlg();

// Dialog Data
	enum { IDD = IDD_RESPAWN };

protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_pReinforcement;
	CButton m_pACaptured;
	CEdit m_pReinforcementTime;
	afx_msg void OnBnClickedReinforcement();
	afx_msg void OnBnClickedArtefactCaptured();
	afx_msg void OnEnChangeReinforcementtime();

	void				OnGameTypeSwitch(byte NewGameType);
//	afx_msg void OnEnChangeReinforcementtime2();
	CEdit m_pRPFreezeTime;
	CEdit m_pDmBlockLimit;
	CButton m_pDmgBlkInd;
	CEdit m_pForceRespawnTime;
	CButton m_pForceRespawn;
	afx_msg void OnBnClickedForceRespawn();
};
