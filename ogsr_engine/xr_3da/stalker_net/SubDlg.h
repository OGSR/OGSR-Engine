#pragma once


// CSubDlg dialog

class CSubDlg : public CDialog
{
	DECLARE_DYNAMIC(CSubDlg)

public:
	CSubDlg(UINT nIDTemplate, CWnd* pParent = NULL);   // standard constructor
	virtual ~CSubDlg();

	virtual void PostNcDestroy( );

// Dialog Data
	enum { IDD = IDD_SUBDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnCancel();
	virtual void OnOK();
};
