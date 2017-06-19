// Stalker_netDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Stalker_net.h"
#include "Stalker_netDlg.h"
//#include ".\stalker_netdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Version_Define.h"
#include ".\stalker_netdlg.h"

// CStalker_netDlg dialog



CStalker_netDlg::CStalker_netDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CStalker_netDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(ICON_ID/*IDR_MAINFRAME*/);
	m_pServerDlg = NULL;
	m_pClientDlg = NULL;
	m_pCDKeyDlg = NULL;	
}

void CStalker_netDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_pTabCtrl);
	DDX_Control(pDX, IDC_CATCHINPUT, m_pCatchInput);
	DDX_Control(pDX, IDC_PLAYERNAME, m_pPlayerName);
	DDX_Control(pDX, IDC_BUILD, m_pBuild);
	DDX_Control(pDX, IDC_CDKEYBTN, m_pCDKeyBtn);
	DDX_Control(pDX, IDC_NOPREFETCH, m_pPrefetch);
	DDX_Control(pDX, IDC_R2, m_pR2);
	DDX_Control(pDX, IDC_DISTORT, m_pDistort);
	DDX_Control(pDX, IDC_LOGSPATH, m_pLogsPath);
}

BEGIN_MESSAGE_MAP(CStalker_netDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
	ON_BN_CLICKED(IDC_CDKEYBTN, CallToEnterCDKey)
//ON_WM_KEYDOWN()
ON_WM_CLOSE()
//ON_BN_CLICKED(IDC_BUILD2, OnBnClickedBuild2)
ON_BN_CLICKED(IDC_LOGSPATH, OnBnClickedLogsPath)
END_MESSAGE_MAP()


// CStalker_netDlg message handlers

BOOL CStalker_netDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	SetWindowText(WND_NAME);

	// TODO: Add extra initialization here
	//Load Map List
//	LoadMapList();

	m_pServerDlg = new CServerDlg();
	if (m_pServerDlg)
	{
		BOOL ret = m_pServerDlg->Create(IDD_SERVERDLG,this);
		if(!ret)   //Create failed.
			AfxMessageBox("Error creating Server Dialog");
		m_pServerDlg->ShowWindow(SW_SHOW);
		m_pServerDlg->SetParent(&m_pTabCtrl);
		m_pServerDlg->SetWindowPos(NULL, 3, 23, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	}
	else
		AfxMessageBox("Error Creating Server Dialog Object");
	//-------------------------------------------------------
	m_pClientDlg = new CClientDlg();
	if (m_pClientDlg)
	{
		BOOL ret = m_pClientDlg->Create(IDD_CLIENTDLG,this);
		if(!ret)   //Create failed.
			AfxMessageBox("Error creating Client Dialog");
		m_pClientDlg->ShowWindow(SW_HIDE);
		m_pClientDlg->SetParent(&m_pTabCtrl);
		m_pClientDlg->SetWindowPos(NULL, 3, 23, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}
	else
		AfxMessageBox("Error Creating Client Object");
	//-------------------------------------------------------
	/*
	m_pCDKeyDlg = new CCDKeyDlg();
	if (m_pCDKeyDlg)
	{
		BOOL ret = m_pCDKeyDlg->Create(IDD_CDKEYDLG,this);
		if(!ret)   //Create failed.
			AfxMessageBox("Error creating Client Dialog");
		m_pCDKeyDlg->ShowWindow(SW_HIDE);
	}
	else
		AfxMessageBox("Error Creating Client Object");
		*/
	//------------- TabCtrl ------------------
	TCITEM tcItem;
	tcItem.mask = TCIF_TEXT;

	tcItem.pszText = _T("Server");
	m_pTabCtrl.InsertItem(0, &tcItem);

#ifndef NDEBUG
	tcItem.pszText = _T("Client");
	m_pTabCtrl.InsertItem(1, &tcItem);
#endif

	m_pTabCtrl.SetCurSel(0);
	//---------------------------------------
	m_pCatchInput.SetCheck(0);

	m_pPlayerName.SetWindowText("");	
	//---------------------------------------
	char CompName[1024];
	DWORD CompNameSize = 1024;
	GetComputerName(CompName, &CompNameSize);
	
	CompName[MAX_PLAYERNAME_LEN] = 0;
	m_pPlayerName.SetWindowText(CompName);
	m_pPlayerName.LimitText(MAX_PLAYERNAME_LEN);

	m_pBuild.SetCheck(0);
	m_pBuild.EnableWindow(FALSE);
	m_pBuild.ShowWindow(SW_HIDE);

	m_pPrefetch.SetCheck(1);
	m_pPrefetch.EnableWindow(FALSE);
	m_pPrefetch.ShowWindow(SW_HIDE);

//	if (!stricmp(CompName, "MAD_MAX_1") || !stricmp(CompName, "MAD_MAX_2"))
	{
		m_pBuild.EnableWindow( TRUE);
		m_pBuild.ShowWindow(SW_SHOWNA);
	}
	m_pPrefetch.EnableWindow( TRUE);
	m_pPrefetch.ShowWindow(SW_SHOWNA);

	m_pR2.EnableWindow( TRUE);
	m_pR2.ShowWindow(SW_SHOWNA);

	m_pDistort.EnableWindow( TRUE);
	m_pDistort.ShowWindow(SW_SHOWNA);
	m_pDistort.SetCheck(1);
	//---------------------------------------
	m_bCloseAllowed = false;
	//-----------------------------------------------
	if (!CHECK_CDKEY) m_pCDKeyBtn.EnableWindow(FALSE);
	else m_pCDKeyBtn.EnableWindow(TRUE);

	CreateCDKeyEntry();

	char CDKeyStr[1024];
	GetCDKey(CDKeyStr);
	m_pCDKeyBtn.SetWindowText(CDKeyStr);

	m_pLogsPath.SetWindowText("");
	//-----------------------------------------------
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStalker_netDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CStalker_netDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CStalker_netDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int iItem = m_pTabCtrl.GetCurSel();

	switch (iItem)
	{
	case 0:
		{
			m_pServerDlg->ShowWindow(SW_SHOW);
			m_pClientDlg->ShowWindow(SW_HIDE);
		}break;
	case 1:
		{
			m_pServerDlg->ShowWindow(SW_HIDE);
			m_pClientDlg->ShowWindow(SW_SHOW);
		}break;
	};
}


void CStalker_netDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	int x=0;
	x=x;

}

//void CStalker_netDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
//}

void CStalker_netDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bCloseAllowed) return;
	CDialog::OnCancel();
}

void CStalker_netDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	return;
	
	CDialog::OnOK();
}

void CStalker_netDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_bCloseAllowed = true;
	CDialog::OnClose();
}
#define REGISTRY_BASE	HKEY_LOCAL_MACHINE
#define REGISTRY_PATH	"Software\\GSC Game World\\STALKER-SHOC\\"
#define REGISTRY_VALUE_GSCDKEY	"InstallCDKEY"
#define REGISTRY_VALUE_VERSION	"InstallVers"

void	CStalker_netDlg::GetCDKey(char* CDKeyStr)
{
	HKEY KeyCDKey = 0;
	long res = RegOpenKeyEx(REGISTRY_BASE, 
		REGISTRY_PATH, 0, KEY_READ, &KeyCDKey);

	DWORD KeyValueSize = 128;
	DWORD KeyValueType = REG_SZ;
	if (res == ERROR_SUCCESS && KeyCDKey != 0)
	{
		res = RegQueryValueEx(KeyCDKey, REGISTRY_VALUE_GSCDKEY, NULL, &KeyValueType, (LPBYTE)CDKeyStr, &KeyValueSize);
	};

	if (res == ERROR_PATH_NOT_FOUND ||
		res == ERROR_FILE_NOT_FOUND ||
		KeyValueSize == 0)
	{
		sprintf(CDKeyStr, "- No CD Key -" );
	};
};

void	CStalker_netDlg::CreateCDKeyEntry()
{
	return;

	HKEY KeyCDKey;

	long res = RegOpenKeyEx(REGISTRY_BASE, 
		REGISTRY_CDKEY_STR, 0, KEY_ALL_ACCESS, &KeyCDKey);

	switch (res)
	{
	case ERROR_FILE_NOT_FOUND:
		{
			HKEY hKey;
			res = RegOpenKeyEx(REGISTRY_BASE, 
				REGISTRY_PATH, 0, KEY_ALL_ACCESS, &hKey);
			if (res != ERROR_SUCCESS) return;
			DWORD xres;
			res = RegCreateKeyEx(hKey, REGISTRY_PATH, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS,
				NULL, &KeyCDKey, &xres);

			if (hKey) RegCloseKey(hKey);
		}break;
	};
	//-----------------------------------------------------------------------------------
	if (!KeyCDKey) return;
	char KeyValue[1024] = "";
	DWORD KeyValueSize = 1023;
	DWORD KeyValueType = REG_SZ;
	res = RegQueryValueEx(KeyCDKey, REGISTRY_VALUE_GSCDKEY, NULL, &KeyValueType, (LPBYTE)KeyValue, &KeyValueSize);

	switch (res)
	{
	case ERROR_FILE_NOT_FOUND:
		{
			res = RegSetValueEx(KeyCDKey, REGISTRY_VALUE_GSCDKEY, NULL, KeyValueType, (LPBYTE)KeyValue, 0);
		}break;
	};	

	if (KeyCDKey) RegCloseKey(KeyCDKey);
};

void	CStalker_netDlg::CallToEnterCDKey()
{
	char NewCDKey[1024];

	CCDKeyDlg dlg;
	INT_PTR nResponse = dlg.DoModal(NewCDKey);
	if (nResponse == IDOK)
	{
		HKEY KeyCDKey = 0;

		long res = RegOpenKeyEx(HKEY_CURRENT_USER, 
			REGISTRY_PATH, 0, KEY_ALL_ACCESS, &KeyCDKey);

		if (res == ERROR_SUCCESS && KeyCDKey != 0)
		{
			DWORD KeyValueSize = (DWORD) xr_strlen(NewCDKey);
			DWORD KeyValueType = REG_SZ;
			res = RegSetValueEx(KeyCDKey, REGISTRY_VALUE_GSCDKEY, NULL, KeyValueType, (LPBYTE)NewCDKey, KeyValueSize);
		}

		if (KeyCDKey) RegCloseKey(KeyCDKey);

		char CDKeyStr[1024];
		GetCDKey(CDKeyStr);
		m_pCDKeyBtn.SetWindowText(CDKeyStr);
	}
	else if (nResponse == IDCANCEL)
	{
	};
};

//void CStalker_netDlg::OnBnClickedBuild2()
//{
//	// TODO: Add your control notification handler code here
//}

void CStalker_netDlg::OnBnClickedLogsPath()
{
	char NewPath[MAX_PATH] = "";
	m_pLogsPath.GetWindowText(NewPath, MAX_PATH);
	/*
	ITEMIDLIST InitialDir;
	ULONG tmp;
	SHParseDisplayName(NewPath, NULL, &InitialDir, 0, &tmp);
*/
	CoInitialize( NULL);//, COINIT_APARTMENTTHREADED );
	BROWSEINFO BIS; ZeroMemory(&BIS, sizeof(BIS));
	BIS.hwndOwner = NULL;
	BIS.pszDisplayName = NewPath;
	BIS.lpszTitle = "Select root folder to store logs";
	BIS.ulFlags = 0;

	LPITEMIDLIST res = SHBrowseForFolder(&BIS);
	if (res == NULL) return;	
	if (!SHGetPathFromIDList(res, NewPath)) return;
	strcat(NewPath, "\\");
	m_pLogsPath.SetWindowText(NewPath);
}
