// PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "PasswordDlg.h"
#include ".\passworddlg.h"


static char PswStr[1024] = "";
// CPasswordDlg dialog

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialog)
CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
}

CPasswordDlg::~CPasswordDlg()
{
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_pPswCtrl);
}

BOOL CPasswordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pPswCtrl.SetWindowText(PswStr);

	return TRUE;
};

BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
END_MESSAGE_MAP()


// CPasswordDlg message handlers
INT_PTR CPasswordDlg::DoModal( char* Psw )
{
	pPsw = Psw;

	return CDialog::DoModal();
};
void CPasswordDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_pPswCtrl.GetWindowText(PswStr, 64);
	strcpy(pPsw, PswStr);
	CDialog::OnOK();
}
