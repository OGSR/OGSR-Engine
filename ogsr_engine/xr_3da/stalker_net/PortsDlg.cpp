// PortsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "PortsDlg.h"
#include "Version_Define.h"
#include ".\portsdlg.h"

// CPortsDlg dialog

IMPLEMENT_DYNAMIC(CPortsDlg, CDialog)
CPortsDlg::CPortsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPortsDlg::IDD, pParent)
{
}

CPortsDlg::~CPortsDlg()
{
}

void CPortsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PORTS, m_pPortsList);
}

BOOL CPortsDlg::OnInitDialog()
{
	BOOL res = CDialog::OnInitDialog();
	FillPortsList();
	return res;
}

BEGIN_MESSAGE_MAP(CPortsDlg, CDialog)
	ON_LBN_DBLCLK(IDC_PORTS, OnLbnDblclkPorts)
	ON_LBN_SELCHANGE(IDC_PORTS, OnLbnSelchangePorts)
END_MESSAGE_MAP()

void	CPortsDlg::FillPortsList()
{
	m_pPortsList.ResetContent();
	
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	

	for (int i=START_PORT; i<= END_PORT; i++)
	{
		for (int p=0; p<m_iNumPorts; p++)
		{
			if (m_pUsedPorts[p] == i) break;
		}
		if (p<m_iNumPorts) continue;

		service.sin_port = htons(i);
//-----------------------------------------------------------------------
		SOCKET tmpsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (tmpsocket == INVALID_SOCKET)
		{
			continue;
		}	
		
		int res = bind( tmpsocket, 
			(SOCKADDR*) &service, 
			sizeof(service));
		if (res != 0)
		{
			closesocket(tmpsocket);
			continue;
		}
		closesocket(tmpsocket);
//-----------------------------------------------------------------------
		tmpsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tmpsocket == INVALID_SOCKET)
		{
			continue;
		}	

		res = bind( tmpsocket, 
			(SOCKADDR*) &service, 
			sizeof(service));
		if (res != 0)
		{
			closesocket(tmpsocket);
			continue;
		}
		closesocket(tmpsocket);
//-----------------------------------------------------------------------
		char PortStr[1024];
		int ItemID = m_pPortsList.AddString(ltoa(i, PortStr, 10));
		m_pPortsList.SetItemData(ItemID, DWORD(i));
	}
	m_pPortsList.SetCurSel(0);
	*m_pRes = int(m_pPortsList.GetItemData(m_pPortsList.GetCurSel()));
	
};
// CPortsDlg message handlers

INT_PTR CPortsDlg::DoModal(  int NumPorts, int* pUsedPorts, int* pRes  )
{	
	m_pRes = pRes;
	*m_pRes = -1;

	m_iNumPorts = NumPorts;
	m_pUsedPorts = pUsedPorts;
	return CDialog::DoModal();
};
void CPortsDlg::OnLbnDblclkPorts()
{
	// TODO: Add your control notification handler code here
	*m_pRes = int(m_pPortsList.GetItemData(m_pPortsList.GetCurSel()));
	EndDialog(IDOK);
}

void CPortsDlg::OnLbnSelchangePorts()
{
	// TODO: Add your control notification handler code here
	*m_pRes = int(m_pPortsList.GetItemData(m_pPortsList.GetCurSel()));
}
