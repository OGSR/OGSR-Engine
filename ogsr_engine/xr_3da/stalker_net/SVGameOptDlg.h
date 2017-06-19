#pragma once
#include "afxwin.h"
#include "SubDlg.h"



// SVGameOptDlg dialog

class SVGameOptDlg : public CSubDlg
{
	DECLARE_DYNAMIC(SVGameOptDlg)

public:
	SVGameOptDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVGameOptDlg();
	virtual BOOL OnInitDialog();

// Dialog Data
	enum { IDD = IDD_GAMEOPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_pFragLimit;
	CEdit m_pTimeLimit;
	CSliderCtrl m_pFriedlyFire;
	CStatic m_pFFAmount;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CComboBox m_pArtefactsNum;
	CStatic m_pStaticFL;
	CStatic m_pStaticTL;
	CStatic m_pStaticFF;
	CStatic m_pStaticAN;
	CEdit m_pArtefactDelay;
	CStatic m_pStaticAD;

	void				OnGameTypeSwitch(byte NewGameType);
	CStatic m_pStaticAST;
	CEdit m_pArtefactStay;
//	afx_msg void OnBnClickedDedicated2();
	CButton m_pNoAnomalies;

//	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CButton m_pAutoTeamBalance;
	CButton m_pFriendlyIndicators;
	CButton m_pFriendlyNames;
	CEdit m_pAnomalySetTime;
	afx_msg void OnBnClickedNoAnomalies();
	CEdit m_pWarmUpTime;
	CStatic m_pStaticWU;
	CButton m_pAutoTeamSwap;
};
