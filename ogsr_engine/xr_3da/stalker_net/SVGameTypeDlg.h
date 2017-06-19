#pragma once
#include "afxwin.h"
#include "SubDlg.h"



// SVGameTypeDlg dialog

class SVGameTypeDlg : public CSubDlg
{
	DECLARE_DYNAMIC(SVGameTypeDlg)

public:
	SVGameTypeDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVGameTypeDlg();

// Dialog Data
	enum { IDD = IDD_GAMETYPE };

protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_pGameDM;
	CButton m_pGameTDM;
	CButton m_pGameAHunt;
	afx_msg void OnBnClickedGameDM();
	afx_msg void OnBnClickedGameTDM();
	afx_msg void OnBnClickedGameAHunt();

	void				OnGameTypeSwitch(byte NewGameType);

};
