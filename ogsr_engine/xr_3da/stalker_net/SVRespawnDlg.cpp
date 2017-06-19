// SVRespawnDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "SVRespawnDlg.h"
#include ".\svrespawndlg.h"

#include "GameTypes.h"
// SVRespawnDlg dialog

IMPLEMENT_DYNAMIC(SVRespawnDlg, CSubDlg)
SVRespawnDlg::SVRespawnDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(SVRespawnDlg::IDD, pParent)
{
}

SVRespawnDlg::~SVRespawnDlg()
{
}

void SVRespawnDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REINFORCEMENT, m_pReinforcement);
	DDX_Control(pDX, IDC_ARTEFACTCAPTURED, m_pACaptured);
	DDX_Control(pDX, IDC_REINFORCEMENTTIME, m_pReinforcementTime);
	DDX_Control(pDX, IDC_PROINTFREEZETIME, m_pRPFreezeTime);
	DDX_Control(pDX, IDC_DAMAGEBLOCKLIMIT, m_pDmBlockLimit);
	DDX_Control(pDX, IDC_DMGBLKIND, m_pDmgBlkInd);
	DDX_Control(pDX, IDC_FORCERESPAWNTIME, m_pForceRespawnTime);
	DDX_Control(pDX, IDC_FORCERESPAWN, m_pForceRespawn);
}


BEGIN_MESSAGE_MAP(SVRespawnDlg, CSubDlg)
	ON_BN_CLICKED(IDC_REINFORCEMENT, OnBnClickedReinforcement)
	ON_BN_CLICKED(IDC_ARTEFACTCAPTURED, OnBnClickedArtefactCaptured)
	ON_EN_CHANGE(IDC_REINFORCEMENTTIME, OnEnChangeReinforcementtime)
	ON_BN_CLICKED(IDC_FORCERESPAWN, OnBnClickedForceRespawn)
END_MESSAGE_MAP()

BOOL SVRespawnDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();

	m_pReinforcement.EnableWindow(TRUE);
	m_pACaptured.EnableWindow(TRUE);
	
	m_pReinforcement.SetCheck(1);
	m_pACaptured.SetCheck(0);
	//-----------------------------------------
	m_pReinforcementTime.EnableWindow(FALSE);
	m_pReinforcementTime.SetWindowText("10");
	//-----------------------------------------
	m_pRPFreezeTime.EnableWindow(TRUE);
	m_pRPFreezeTime.SetWindowText("5");
	//-----------------------------------------
	m_pDmgBlkInd.SetCheck(1);
	m_pDmBlockLimit.SetWindowText("0");
	//-----------------------------------------
	m_pForceRespawn.EnableWindow(TRUE);
	m_pForceRespawn.SetCheck(0);
	m_pForceRespawnTime.EnableWindow(TRUE);
	m_pForceRespawnTime.SetWindowText("0");

	return TRUE;  // return TRUE  unless you set the focus to a control
};
// SVRespawnDlg message handlers
void SVRespawnDlg::OnBnClickedForceRespawn()
{
	// TODO: Add your control notification handler code here
	m_pForceRespawn.SetCheck(1);
	m_pReinforcement.SetCheck(0);
	m_pACaptured.SetCheck(0);
	//-----------------------------------------
}

void SVRespawnDlg::OnBnClickedReinforcement()
{
	// TODO: Add your control notification handler code here
	m_pForceRespawn.SetCheck(0);
	m_pReinforcement.SetCheck(1);
	m_pACaptured.SetCheck(0);
	//-----------------------------------------
}

void SVRespawnDlg::OnBnClickedArtefactCaptured()
{
	// TODO: Add your control notification handler code here
	m_pForceRespawn.SetCheck(0);
	m_pReinforcement.SetCheck(0);
	m_pACaptured.SetCheck(1);
}

void SVRespawnDlg::OnEnChangeReinforcementtime()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}

void SVRespawnDlg::OnGameTypeSwitch(byte NewGameType)
{
	GAME_TYPE GameType = (GAME_TYPE)NewGameType;
	switch (GameType)
	{
	case GAME_DEATHMATCH :
		{
			m_pForceRespawn.SetCheck(1);
			m_pReinforcement.SetCheck(0);
			m_pACaptured.SetCheck(0);

			m_pForceRespawn.EnableWindow(TRUE);
			m_pReinforcement.EnableWindow(FALSE);
			m_pACaptured.EnableWindow(FALSE);
			//-----------------------------------------
			m_pReinforcementTime.EnableWindow(FALSE);
			m_pReinforcementTime.SetWindowText("0");
			m_pForceRespawnTime.EnableWindow(TRUE);			
		}break;
	case GAME_TEAMDEATHMATCH :
		{
			m_pForceRespawn.SetCheck(1);
			m_pReinforcement.SetCheck(0);
			m_pACaptured.SetCheck(0);

			m_pForceRespawn.EnableWindow(TRUE);
			m_pReinforcement.EnableWindow(FALSE);
			m_pACaptured.EnableWindow(FALSE);
			//-----------------------------------------
			m_pReinforcementTime.EnableWindow(FALSE);
			m_pReinforcementTime.SetWindowText("0");
			m_pForceRespawnTime.EnableWindow(TRUE);			
		}break;
	case GAME_ARTEFACTHUNT:
		{
			m_pForceRespawn.EnableWindow(TRUE);
			m_pReinforcement.EnableWindow(TRUE);
			m_pACaptured.EnableWindow(TRUE);
			//-----------------------------------------
			m_pReinforcementTime.EnableWindow(TRUE);
			m_pForceRespawnTime.EnableWindow(TRUE);
		}break;
	};
}
