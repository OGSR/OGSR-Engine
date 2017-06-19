#pragma once
#include "afxwin.h"


// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPasswordDlg();

// Dialog Data
	enum { IDD = IDD_PSWDLG };

protected:
	char* pPsw;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	virtual INT_PTR DoModal( char* Psw );
	CEdit m_pPswCtrl;
protected:
	virtual void OnOK();
};
