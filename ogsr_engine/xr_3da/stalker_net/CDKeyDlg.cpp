// CDKeyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "CDKeyDlg.h"
#include ".\cdkeydlg.h"


// CCDKeyDlg dialog

IMPLEMENT_DYNAMIC(CCDKeyDlg, CDialog)
CCDKeyDlg::CCDKeyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCDKeyDlg::IDD, pParent)
{
}

CCDKeyDlg::~CCDKeyDlg()
{
}

void CCDKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CDK1, m_pCDK1);
	DDX_Control(pDX, IDC_CDK2, m_pCDK2);
	DDX_Control(pDX, IDC_CDK3, m_pCDK3);
	DDX_Control(pDX, IDC_CDK4, m_pCDK4);
	DDX_Control(pDX, IDOK, m_pOk);
	DDX_Control(pDX, IDCANCEL, m_pCancel);
}

BOOL CCDKeyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pCDKA[0] = &m_pCDK1;
	m_pCDKA[1] = &m_pCDK2;
	m_pCDKA[2] = &m_pCDK3;
	m_pCDKA[3] = &m_pCDK4;

	m_pCDK1.SetLimitText(4);
	m_pCDK2.SetLimitText(4);
	m_pCDK3.SetLimitText(4);
	m_pCDK4.SetLimitText(4);

	m_pOk.EnableWindow(FALSE);
	m_pCancel.EnableWindow(TRUE);

	m_pCDK1.SetActiveWindow();

	return TRUE;  // return TRUE  unless you set the focus to a control
};


BEGIN_MESSAGE_MAP(CCDKeyDlg, CDialog)
//	ON_EN_MAXTEXT(IDC_CDK1, OnEnMaxtextCDK)
//	ON_EN_MAXTEXT(IDC_CDK2, OnEnMaxtextCDK)
//	ON_EN_MAXTEXT(IDC_CDK3, OnEnMaxtextCDK)
//	ON_EN_MAXTEXT(IDC_CDK4, OnEnMaxtextCDK)
	ON_EN_CHANGE(IDC_CDK1, OnEnChangeCDK)
	ON_EN_CHANGE(IDC_CDK2, OnEnChangeCDK)
	ON_EN_CHANGE(IDC_CDK3, OnEnChangeCDK)
	ON_EN_CHANGE(IDC_CDK4, OnEnChangeCDK)
END_MESSAGE_MAP()


// CCDKeyDlg message handlers

void CCDKeyDlg::OnEnChangeCDK()
{
	// TODO:  Add your control notification handler code here
	
	CEdit* pActiveWindow = (CEdit*)GetFocus();
	if (pActiveWindow->GetWindowTextLength() < 4) return;
	
	int KeyLength = 0;
	for (int i=0; i<4; i++)
	{
		if (pActiveWindow == m_pCDKA[i])
		{
			int newI = (i+1)%4;
			m_pCDKA[newI]->SetFocus();
		};
		KeyLength += m_pCDKA[i]->GetWindowTextLength();
	}

	if (KeyLength == 16)
	{
		m_pOk.EnableWindow(TRUE);
		m_pOk.SetFocus();
	};
}


void CCDKeyDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	int offset = 0;
	for (int i=0; i<4; i++)
	{
		CString tmp;
		m_pCDKA[i]->GetWindowText(tmp);
		strncpy(m_pNewCDKey+offset, tmp, 4);
		offset += 4;
		m_pNewCDKey[offset] = 0;
		strcat(m_pNewCDKey, "-");
		offset++;
	};
	m_pNewCDKey[offset-1] = 0;

	CDialog::OnOK();
}

INT_PTR CCDKeyDlg::DoModal(char* NewCDKey)
{
	// TODO: Add your specialized code here and/or call the base class
	m_pNewCDKey = NewCDKey;

	return CDialog::DoModal();
}
