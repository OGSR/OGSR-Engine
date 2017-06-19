// SVServerOpt.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "Version_Define.h"
#include "SVServerOptDlg.h"
#include ".\svserveroptdlg.h"
#include "PortsDlg.h"


// SVServerOpt dialog

IMPLEMENT_DYNAMIC(SVServerOptDlg, CSubDlg)
SVServerOptDlg::SVServerOptDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(SVServerOptDlg::IDD, pParent)
{
}

SVServerOptDlg::~SVServerOptDlg()
{
}

void SVServerOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOSTNAME, m_pHostName);
	DDX_Control(pDX, IDC_PASSWORD, m_pPassword);
	DDX_Control(pDX, IDC_MAXPLAYERS, m_pMaxPlayers);
	DDX_Control(pDX, IDC_SPECTATORONLY, m_pSpectrMode);
	DDX_Control(pDX, IDC_SPECTRTIME, m_pSpectrSwitchTime);
	DDX_Control(pDX, IDC_DEDICATED, m_pDedicated);
	DDX_Control(pDX, IDC_PUBLIC, m_pPublic);
	DDX_Control(pDX, IDC_CDKEY, m_pCheckCDKey);
	//	DDX_Control(pDX, IDC_MAPROTATION, m_pMapRotation);
	//	DDX_Control(pDX, IDC_MAPROTFILE, m_pMapRotationFile);
	DDX_Control(pDX, IDC_VOTINGALLOWED, m_pVotingAllowed);
	DDX_Control(pDX, IDC_SERVERPORT, m_pSVPort);
	DDX_Control(pDX, IDC_GAMESPYPORT, m_pGSPort);
	DDX_Control(pDX, IDC_CLIENTPORT, m_pCLPort);
}

BOOL SVServerOptDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();
	//-------------------------------------------------
	char CompName[1024];
	DWORD CompNameSize = 1024;
	GetComputerName(CompName, &CompNameSize);
	//-------------------------------------------------	
	CompName[MAX_SERVERNAME_LEN] = 0;
	m_pHostName.SetWindowText(CompName);
	m_pHostName.LimitText(MAX_SERVERNAME_LEN);
	
	m_pPassword.SetWindowText("");
	m_pPassword.LimitText(MAX_SERVERPASSW_LEN);


	m_pSVPort.SetWindowText("5445");
	m_pSVPort.LimitText(MAX_PORTNUMBER_LEN);
	m_pCLPort.SetWindowText("5446");
	m_pCLPort.LimitText(MAX_PORTNUMBER_LEN);
	m_pGSPort.SetWindowText("5447");
	m_pGSPort.LimitText(MAX_PORTNUMBER_LEN);

	m_pMaxPlayers.SetWindowText("32");
	m_pSpectrMode.SetCheck(0);
	m_pSpectrSwitchTime.SetWindowText("20"); 
	m_pSpectrSwitchTime.EnableWindow(FALSE);

	m_pDedicated.SetCheck(0);
	m_pPublic.SetCheck(0);
	m_pCheckCDKey.SetCheck(0);
	m_pCheckCDKey.EnableWindow(FALSE);

//	m_pMapRotation.SetCheck(0);
//	m_pMapRotation.EnableWindow(TRUE);
//	m_pMapRotationFile.SetWindowText("maprot_list");
//	m_pMapRotationFile.EnableWindow(FALSE);

	m_pVotingAllowed.EnableWindow(TRUE);
	m_pVotingAllowed.SetCheck(1);

	return TRUE;  // return TRUE  unless you set the focus to a control
};
BEGIN_MESSAGE_MAP(SVServerOptDlg, CSubDlg)
	ON_BN_CLICKED(IDC_SPECTATORONLY, OnBnClickedSpectatoronly)
	ON_BN_CLICKED(IDC_PUBLIC, OnBnClickedPublic)
	ON_BN_CLICKED(IDC_MAPROTATION, OnBnClickedMapRotation)
	ON_BN_CLICKED(IDC_SV_PORTBUTTON, OnBnClickedSvPortButton)
	ON_BN_CLICKED(IDC_CL_PORTBUTTON, OnBnClickedClPortbutton)
	ON_BN_CLICKED(IDC_GS_PORTBUTTON, OnBnClickedGsPortbutton)
END_MESSAGE_MAP()


// SVServerOpt message handlers

void SVServerOptDlg::OnBnClickedSpectatoronly()
{
	// TODO: Add your control notification handler code here
	if (m_pSpectrMode.GetCheck())
	{
		m_pSpectrSwitchTime.EnableWindow(TRUE);
	}
	else
	{
		m_pSpectrSwitchTime.EnableWindow(FALSE);
	}
}

void SVServerOptDlg::OnBnClickedPublic()
{
	// TODO: Add your control notification handler code here
	if (m_pPublic.GetCheck())
	{
		m_pCheckCDKey.EnableWindow(FALSE);
		m_pCheckCDKey.SetCheck(1);
	}
	else
	{
		m_pCheckCDKey.EnableWindow(FALSE);
		m_pCheckCDKey.SetCheck(0);
	}
}

void SVServerOptDlg::OnBnClickedMapRotation()
{
	// TODO: Add your control notification handler code here
//	if (m_pMapRotation.GetCheck())
//	{
//		m_pMapRotationFile.EnableWindow(TRUE);
//	}
//	else
//	{
//		m_pMapRotationFile.EnableWindow(FALSE);
//	};
}

void	SVServerOptDlg::SetPort(CEdit* pPortItem, int NumPorts, int* UsedPorts)
{
	if (!pPortItem) return;
	int ResPort = 0;
	// TODO: Add your control notification handler code here
	CPortsDlg dlg;	
	INT_PTR nResponse = dlg.DoModal(NumPorts, UsedPorts, &ResPort);
	if (nResponse == IDCANCEL) return;
	if (nResponse == IDOK)
	{
		char tmp[1024];
		pPortItem->SetWindowText(ltoa(ResPort, tmp, 10));
	};
};

void SVServerOptDlg::OnBnClickedSvPortButton()
{
	int UsedPorts[2];
	char tmp[1024];
	m_pCLPort.GetWindowText(tmp, 1024);
	UsedPorts[0] = atol(tmp);
	m_pGSPort.GetWindowText(tmp, 1024);
	UsedPorts[1] = atol(tmp);

	SetPort(&m_pSVPort, 2, UsedPorts);
}

void SVServerOptDlg::OnBnClickedClPortbutton()
{
	int UsedPorts[2];
	char tmp[1024];
	m_pSVPort.GetWindowText(tmp, 1024);
	UsedPorts[0] = atol(tmp);
	m_pGSPort.GetWindowText(tmp, 1024);
	UsedPorts[1] = atol(tmp);

	SetPort(&m_pCLPort, 2, UsedPorts);
}

void SVServerOptDlg::OnBnClickedGsPortbutton()
{
	int UsedPorts[2];
	char tmp[1024];
	m_pSVPort.GetWindowText(tmp, 1024);
	UsedPorts[0] = atol(tmp);
	m_pCLPort.GetWindowText(tmp, 1024);
	UsedPorts[1] = atol(tmp);

	SetPort(&m_pGSPort, 2, UsedPorts);
}
