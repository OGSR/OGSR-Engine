#include "StdAfx.h"
#include "GameSpy_Full.h"

#include "GameSpy_Available.h"
#include "GameSpy_Patching.h"
#include "GameSpy_HTTP.h"
#include "GameSpy_Browser.h"

#include "../MainMenu.h"
#include "../object_broker.h"


CGameSpy_Full::CGameSpy_Full()	
{
	m_pGSA	= NULL;
	m_pGS_Patching = NULL;
	m_pGS_HTTP = NULL;
	m_pGS_SB = NULL;

	m_hGameSpyDLL	= NULL;
	m_bServicesAlreadyChecked	= false;

	LoadGameSpy();
	//---------------------------------------
	m_pGSA = xr_new<CGameSpy_Available>(m_hGameSpyDLL);
	//-----------------------------------------------------
	shared_str resultstr;
	m_bServicesAlreadyChecked = m_pGSA->CheckAvailableServices(resultstr);
	//-----------------------------------------------------
	m_pGS_Patching = xr_new<CGameSpy_Patching>(m_hGameSpyDLL);
	m_pGS_HTTP  = xr_new<CGameSpy_HTTP>(m_hGameSpyDLL);
	m_pGS_SB = xr_new<CGameSpy_Browser>(m_hGameSpyDLL);
}

CGameSpy_Full::~CGameSpy_Full()
{
	delete_data(m_pGSA);
	delete_data(m_pGS_Patching);
	delete_data(m_pGS_HTTP);
	delete_data(m_pGS_SB);

	if (m_hGameSpyDLL)
	{
		FreeLibrary(m_hGameSpyDLL);
		m_hGameSpyDLL = NULL;
	}
}

void	CGameSpy_Full::LoadGameSpy()
{
	LPCSTR			g_name	= "xrGameSpy.dll";
	Log				("Loading DLL:",g_name);
	m_hGameSpyDLL			= LoadLibrary	(g_name);
	if (0==m_hGameSpyDLL)	R_CHK			(GetLastError());
	R_ASSERT2		(m_hGameSpyDLL,"GameSpy DLL raised exception during loading or there is no game DLL at all");

	HMODULE	hGameSpyDLL = m_hGameSpyDLL;
	GAMESPY_LOAD_FN(xrGS_GetGameVersion);
}

void	CGameSpy_Full::Update	()
{
	if (!m_bServicesAlreadyChecked)
	{
		m_bServicesAlreadyChecked = true;
		MainMenu()->SetErrorDialog(CMainMenu::ErrGSServiceFailed);
	}
	m_pGS_HTTP->Think();
	m_pGS_SB->Update();
};

const	char*	CGameSpy_Full::GetGameVersion	(const	char*result)
{
	return xrGS_GetGameVersion(result);
};