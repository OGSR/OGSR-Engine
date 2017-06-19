#pragma once
#include "SubDlg.h"


// ServerOpts2 dialog

class ServerOpts2 : public CSubDlg
{
	DECLARE_DYNAMIC(ServerOpts2)

public:
	ServerOpts2(CWnd* pParent = NULL);   // standard constructor
	virtual ~ServerOpts2();

// Dialog Data
	enum { IDD = IDD_SERVEROPTS2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
