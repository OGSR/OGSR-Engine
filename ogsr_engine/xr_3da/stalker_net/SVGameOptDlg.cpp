// SVGameOptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "SVGameOptDlg.h"
#include "GameTypes.h"
#include ".\svgameoptdlg.h"


// SVGameOptDlg dialog

IMPLEMENT_DYNAMIC(SVGameOptDlg, CSubDlg)
SVGameOptDlg::SVGameOptDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(SVGameOptDlg::IDD, pParent)
{
}

SVGameOptDlg::~SVGameOptDlg()
{
}

void SVGameOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_FRAGLIMIT, m_pFragLimit);
	DDX_Control(pDX, IDC_TIMELIMIT, m_pTimeLimit);
	DDX_Control(pDX, IDC_SLIDER1, m_pFriedlyFire);
	DDX_Control(pDX, IDC_FFAMOUNT, m_pFFAmount);
	DDX_Control(pDX, IDC_ARTEFACTSNUM, m_pArtefactsNum);
	DDX_Control(pDX, IDC_FL, m_pStaticFL);
	DDX_Control(pDX, IDC_TL, m_pStaticTL);
	DDX_Control(pDX, IDC_FF, m_pStaticFF);
	DDX_Control(pDX, IDC_AN, m_pStaticAN);
	DDX_Control(pDX, IDC_ARTEFACTDELAY, m_pArtefactDelay);
	DDX_Control(pDX, IDC_ARD, m_pStaticAD);
	DDX_Control(pDX, IDC_AST, m_pStaticAST);
	DDX_Control(pDX, IDC_ARTEFACTSTAY, m_pArtefactStay);
	DDX_Control(pDX, IDC_NOANOMALIES, m_pNoAnomalies);
	DDX_Control(pDX, IDC_AUTOTEAMBALANCE, m_pAutoTeamBalance);
	DDX_Control(pDX, IDC_FRIENDLYINDICATORS, m_pFriendlyIndicators);
	DDX_Control(pDX, IDC_FRIENDLYNAMES, m_pFriendlyNames);
	DDX_Control(pDX, IDC_ANMSETLENGTH, m_pAnomalySetTime);
	DDX_Control(pDX, IDC_WARMUPTIME, m_pWarmUpTime);
	DDX_Control(pDX, IDC_WARMUPSTATIC, m_pStaticWU);
	DDX_Control(pDX, IDC_AUTOTEAMSWAP, m_pAutoTeamSwap);
}


BEGIN_MESSAGE_MAP(SVGameOptDlg, CSubDlg)
	ON_WM_HSCROLL()
//	ON_BN_CLICKED(IDC_DEDICATED2, OnBnClickedDedicated2)
//ON_WM_VSCROLL()
ON_BN_CLICKED(IDC_NOANOMALIES, OnBnClickedNoAnomalies)
END_MESSAGE_MAP()

BOOL SVGameOptDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();
	//-----------------------------------------
	m_pFriedlyFire.EnableWindow(FALSE);	m_pStaticFF.EnableWindow(FALSE);
	m_pFFAmount.EnableWindow(FALSE);
	m_pFFAmount.SetWindowText("100%");

	int NumTicks = m_pFriedlyFire.GetNumTics();
	m_pFriedlyFire.SetRangeMax(200);
	m_pFriedlyFire.SetRangeMin(0);
	m_pFriedlyFire.ClearTics(TRUE);
	//	m_pFriedlyFire.SetTicFreq(25);
	m_pFriedlyFire.SetTic(0);
	m_pFriedlyFire.SetTic(50);
	m_pFriedlyFire.SetTic(100);
	m_pFriedlyFire.SetTic(150);
	m_pFriedlyFire.SetTic(200);
	m_pFriedlyFire.SetPageSize(25);

	m_pFriedlyFire.SetPos(100);

	//-----------------------------------------
	m_pArtefactsNum.EnableWindow(FALSE);	m_pStaticAN.EnableWindow(FALSE);
	for (int i=1; i<9; i++)
	{
		int ItemData = i*5;
		char Text[1024];
		sprintf(Text, "%2d", ItemData);

		m_pArtefactsNum.AddString(Text);
		m_pArtefactsNum.SetItemData(i-1, (DWORD_PTR)ItemData);
	};
	m_pArtefactsNum.SetCurSel(1);
	//-----------------------------------------
	m_pFragLimit.SetWindowText("0");		m_pStaticFL.EnableWindow(TRUE);
	m_pTimeLimit.SetWindowText("0");		m_pStaticTL.EnableWindow(TRUE);
	m_pArtefactDelay.SetWindowText("30");	m_pStaticAD.EnableWindow(FALSE);
	m_pArtefactDelay.EnableWindow(FALSE);
	//-----------------------------------------
	m_pArtefactStay.SetWindowText("3");
	//-----------------------------------------
	m_pNoAnomalies.SetCheck(0);
	m_pAnomalySetTime.SetWindowText("5");
	OnBnClickedNoAnomalies();
	//-----------------------------------------
	m_pWarmUpTime.SetWindowText("60");		m_pStaticWU.EnableWindow(TRUE);
	/*
	RECT R, PR;
	GetWindowRect(&R);
	GetParent()->GetClientRect(&PR);
	SetScrollRange(SB_VERT, 0, PR.bottom - (R.bottom - R.top), TRUE);
	*/
	m_pFriendlyIndicators.EnableWindow(FALSE);
	m_pFriendlyIndicators.SetCheck(0);
	m_pFriendlyNames.EnableWindow(FALSE);
	m_pFriendlyNames.SetCheck(0);
	return TRUE;
}

// SVGameOptDlg message handlers
void SVGameOptDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CSubDlg::OnHScroll(nSBCode, nPos, pScrollBar);

	if (*pScrollBar == m_pFriedlyFire)
	{
		char Text[1024];
		sprintf(Text, "%d%c", m_pFriedlyFire.GetPos(), '%');//);
		m_pFFAmount.SetWindowText(Text);
	};
}

void	SVGameOptDlg::OnGameTypeSwitch(byte NewGameType)
{
	GAME_TYPE GameType = (GAME_TYPE)NewGameType;
	switch (GameType)
	{
	case GAME_DEATHMATCH :
		{
			m_pFragLimit.EnableWindow(TRUE);		m_pStaticFL.EnableWindow(TRUE);
			m_pFriedlyFire.EnableWindow(FALSE);		m_pStaticFF.EnableWindow(FALSE);
			m_pAutoTeamBalance.EnableWindow(FALSE);
			m_pAutoTeamSwap.EnableWindow(FALSE);
			m_pFFAmount.EnableWindow(FALSE);
			m_pArtefactsNum.EnableWindow(FALSE);	m_pStaticAN.EnableWindow(FALSE);
			m_pArtefactDelay.EnableWindow(FALSE);	m_pStaticAD.EnableWindow(FALSE);
			m_pArtefactStay.EnableWindow(FALSE);	m_pStaticAST.EnableWindow(FALSE);
			m_pFriendlyIndicators.EnableWindow(FALSE); m_pFriendlyIndicators.SetCheck(0);
			m_pFriendlyNames.EnableWindow(FALSE);	m_pFriendlyNames.SetCheck(0);
			m_pWarmUpTime.EnableWindow(TRUE);		m_pStaticWU.EnableWindow(TRUE);
		}break;
	case GAME_TEAMDEATHMATCH :
		{
			m_pFragLimit.EnableWindow(TRUE);		m_pStaticFL.EnableWindow(TRUE);
			m_pFriedlyFire.EnableWindow(TRUE);		m_pStaticFF.EnableWindow(TRUE);
			m_pAutoTeamBalance.EnableWindow(TRUE);
			m_pAutoTeamSwap.EnableWindow(TRUE);
			m_pFFAmount.EnableWindow(TRUE);
			m_pArtefactsNum.EnableWindow(FALSE);	m_pStaticAN.EnableWindow(FALSE);
			m_pArtefactDelay.EnableWindow(FALSE);	m_pStaticAD.EnableWindow(FALSE);
			m_pArtefactStay.EnableWindow(FALSE);	m_pStaticAST.EnableWindow(FALSE);
			m_pFriendlyIndicators.EnableWindow(TRUE);
			m_pFriendlyNames.EnableWindow(TRUE);
			m_pWarmUpTime.EnableWindow(TRUE);		m_pStaticWU.EnableWindow(TRUE);
		}break;
	case GAME_ARTEFACTHUNT:
		{
			m_pFragLimit.EnableWindow(FALSE);		m_pStaticFL.EnableWindow(FALSE);
			m_pFriedlyFire.EnableWindow(TRUE);		m_pStaticFF.EnableWindow(TRUE);
			m_pAutoTeamBalance.EnableWindow(TRUE);
			m_pAutoTeamSwap.EnableWindow(TRUE);
			m_pFFAmount.EnableWindow(TRUE);
			m_pArtefactsNum.EnableWindow(TRUE);		m_pStaticAN.EnableWindow(TRUE);
			m_pArtefactDelay.EnableWindow(TRUE);	m_pStaticAD.EnableWindow(TRUE);
			m_pArtefactStay.EnableWindow(TRUE);		m_pStaticAST.EnableWindow(TRUE);
			m_pFriendlyIndicators.EnableWindow(TRUE);
			m_pFriendlyNames.EnableWindow(TRUE);
			m_pWarmUpTime.EnableWindow(TRUE);		m_pStaticWU.EnableWindow(TRUE);
		}break;
	};
};


//void SVGameOptDlg::OnBnClickedDedicated2()
//{
//	// TODO: Add your control notification handler code here
//}

//void SVGameOptDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
//{
//	// TODO: Add your message handler code here and/or call default
//	int OldScrollPos = GetScrollPos(SB_VERT);
//	ScrollWindow(0, GetScrollPos(SB_VERT) - nPos);
//	SetScrollPos(SB_VERT, nPos, TRUE);
//	CSubDlg::OnVScroll(nSBCode, nPos, pScrollBar);
//}

void SVGameOptDlg::OnBnClickedNoAnomalies()
{
	// TODO: Add your control notification handler code here
	if (m_pNoAnomalies.GetCheck())
	{
		m_pAnomalySetTime.EnableWindow(FALSE);
	}
	else
	{
		m_pAnomalySetTime.EnableWindow(TRUE);
	}
}
