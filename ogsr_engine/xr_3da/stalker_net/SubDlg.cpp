// SubDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "SubDlg.h"


// CSubDlg dialog

IMPLEMENT_DYNAMIC(CSubDlg, CDialog)
CSubDlg::CSubDlg(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog(nIDTemplate, pParent)
{
}

CSubDlg::~CSubDlg()
{
}

void CSubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSubDlg, CDialog)
END_MESSAGE_MAP()

void CSubDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	return;
	CDialog::OnCancel();
}

void CSubDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	return;
	CDialog::OnOK();
}
// CSubDlg message handlers

void CSubDlg::PostNcDestroy( )
{
	CDialog::PostNcDestroy();
	delete this;
}