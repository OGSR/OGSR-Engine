#pragma once
#include "afxwin.h"
#include "SubDlg.h"



// SVSpectatorDlg  dialog

class SVSpectatorDlg : public CSubDlg
{
	DECLARE_DYNAMIC(SVSpectatorDlg )

public:
	SVSpectatorDlg (CWnd* pParent = NULL);   // standard constructor
	virtual ~SVSpectatorDlg ();

// Dialog Data
	enum { IDD = IDD_SPECTATOROPTS };

protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void				OnGameTypeSwitch(byte NewGameType);
	CButton m_SP_TeamOnly;
	CButton m_SP_FFM;
	CButton m_SP_FEM;
	CButton m_SP_LAM;
	CButton m_SP_FLM;

	LPCSTR GetSpectatorModesStr	(LPSTR result, const u32 &result_size);
};