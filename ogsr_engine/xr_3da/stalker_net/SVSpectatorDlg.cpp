// SVSpectatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "SVSpectatorDlg.h"

#include "ServerDlg.h"
#include ".\SVSpectatorDlg.h"
#include <stdio.h>
// SVSpectatorDlg dialog

IMPLEMENT_DYNAMIC(SVSpectatorDlg, CSubDlg)
SVSpectatorDlg::SVSpectatorDlg(CWnd* pParent /*=NULL*/)
	: CSubDlg(SVSpectatorDlg::IDD, pParent)
{
}

SVSpectatorDlg::~SVSpectatorDlg()
{
}

void SVSpectatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CSubDlg::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_SP_TEAMONLY, m_SP_TeamOnly);
	DDX_Control(pDX, IDC_SP_FFM, m_SP_FFM);
	DDX_Control(pDX, IDC_SP_FEM, m_SP_FEM);
	DDX_Control(pDX, IDC_SP_LAM, m_SP_LAM);
	DDX_Control(pDX, IDC_SP_FLM, m_SP_FLM);
}


BEGIN_MESSAGE_MAP(SVSpectatorDlg, CSubDlg)	
END_MESSAGE_MAP()

BOOL SVSpectatorDlg::OnInitDialog()
{
	CSubDlg::OnInitDialog();

	//-----------------------------------------
	m_SP_TeamOnly.SetCheck(1);

	m_SP_TeamOnly.SetCheck(1);
	m_SP_FFM.SetCheck(0);
	m_SP_FEM.SetCheck(1);
	m_SP_LAM.SetCheck(1);
	m_SP_FLM.SetCheck(1);
	//-----------------------------------------

	return TRUE;  // return TRUE  unless you set the focus to a control
};

void	SVSpectatorDlg::OnGameTypeSwitch(byte NewGameType)
{
	GAME_TYPE GameType = (GAME_TYPE)NewGameType;
	switch (GameType)
	{
	case GAME_DEATHMATCH :
		{
			m_SP_TeamOnly.EnableWindow(FALSE);
			m_SP_FFM.EnableWindow(TRUE);
			m_SP_FEM.EnableWindow(TRUE);
			m_SP_LAM.EnableWindow(TRUE);
			m_SP_FLM.EnableWindow(TRUE);
		}break;
	case GAME_TEAMDEATHMATCH :
		{
			m_SP_TeamOnly.EnableWindow(TRUE);
			m_SP_FFM.EnableWindow(TRUE);
			m_SP_FEM.EnableWindow(TRUE);
			m_SP_LAM.EnableWindow(TRUE);
			m_SP_FLM.EnableWindow(TRUE);
		}break;
	case GAME_ARTEFACTHUNT:
		{
			m_SP_TeamOnly.EnableWindow(TRUE);
			m_SP_FFM.EnableWindow(TRUE);
			m_SP_FEM.EnableWindow(TRUE);
			m_SP_LAM.EnableWindow(TRUE);
			m_SP_FLM.EnableWindow(TRUE);
		}break;
	};
};

LPCSTR SVSpectatorDlg::GetSpectatorModesStr	(LPSTR result, const u32 &result_size)
{
	int res = 0;
	res |= (m_SP_FFM.GetCheck()) ? 1<<0 : 0;
	res |= (m_SP_FEM.GetCheck()) ? 1<<1 : 0;
	res |= (m_SP_LAM.GetCheck()) ? 1<<2 : 0;
	res |= (m_SP_FLM.GetCheck()) ? 1<<3 : 0;
	res |= (m_SP_TeamOnly.GetCheck()) ? 1<<4 : 0;

	sprintf(result,"/spectrmds=%d", res);
//	sprintf_s(result,result_size,"/spectrmds=%d", res);
	return (result);
};