// Stalker_net.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "Stalker_netDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStalker_netApp

BEGIN_MESSAGE_MAP(CStalker_netApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CStalker_netApp construction

CStalker_netApp::CStalker_netApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}
CStalker_netApp::~CStalker_netApp()
{
	Core._destroy();
}

// The one and only CStalker_netApp object

CStalker_netApp theApp;


// CStalker_netApp initialization

BOOL CStalker_netApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	Core._initialize("Stalker_net", NULL, TRUE);
	
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
//	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CStalker_netDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

