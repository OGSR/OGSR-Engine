#pragma once
#include "SubDlg.h"
#include "afxwin.h"


// CCDKeyDlg dialog

class CCDKeyDlg : public CDialog
{
	DECLARE_DYNAMIC(CCDKeyDlg)

public:
	CCDKeyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCDKeyDlg();

// Dialog Data
	enum { IDD = IDD_CDKEYDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CEdit* m_pCDKA[4];
	CEdit m_pCDK1;
	CEdit m_pCDK2;
	CEdit m_pCDK3;
	CEdit m_pCDK4;
	CButton m_pOk;
	CButton m_pCancel;
//	afx_msg void OnEnMaxtextCDK();
	afx_msg void OnEnChangeCDK();
protected:
	virtual void OnOK();
	char* m_pNewCDKey;
public:
	virtual INT_PTR DoModal( char* NewCDKey );
};
