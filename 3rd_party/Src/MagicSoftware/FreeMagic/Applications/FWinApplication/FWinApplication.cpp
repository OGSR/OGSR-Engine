// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include <cassert>
#include "FWinApplication.h"

#pragma comment(lib,"comctl32.lib")

char* FWinApplication::ms_acCaption = 0;
int FWinApplication::ms_iWidth = 0;
int FWinApplication::ms_iHeight = 0;
unsigned int FWinApplication::ms_uiMenuID = 0;
unsigned int FWinApplication::ms_uiStatusPaneQuantity = 0;
HWND FWinApplication::ms_hWnd = 0;
HWND FWinApplication::ms_hStatusWnd = 0;
FWinApplication* FWinApplication::ms_pkApplication = NULL;
Command* FWinApplication::ms_pkCommand = NULL;

static HINSTANCE gs_hInstance = 0;
static char gs_acWindowClassName[] = "FWinApplication";
static DWORD gs_dwWindowStyle =
    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

//----------------------------------------------------------------------------
FWinApplication::FWinApplication (char* acCaption, int iWidth, int iHeight,
    unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
{
    assert( ms_pkApplication == 0 );
    ms_pkApplication = this;

    ms_acCaption = acCaption;
    ms_iWidth = iWidth;
    ms_iHeight = iHeight;
    ms_uiMenuID = uiMenuID;
    ms_uiStatusPaneQuantity = uiStatusPaneQuantity;
}
//----------------------------------------------------------------------------
FWinApplication::~FWinApplication ()
{
    assert( ms_pkApplication != 0 );
    ms_pkApplication = 0;

    ms_hWnd = 0;
    ms_hStatusWnd = 0;

    delete ms_pkCommand;
    ms_pkCommand = 0;
}
//----------------------------------------------------------------------------
bool FWinApplication::OnSize (int iWidth, int iHeight,
    unsigned int uiSizeType)
{
    FWinApplication* pkTheApp = FWinApplication::GetApplication();
    if ( !pkTheApp )
        return false;

    pkTheApp->SetWidth(iWidth);
    pkTheApp->SetHeight(iHeight);

    unsigned int uiStatusPaneQuantity = pkTheApp->GetStatusPaneQuantity();
    if ( uiStatusPaneQuantity )
    {
        // resize the status window
        HWND hStatusWnd = pkTheApp->GetStatusWindowHandle();

        WPARAM wParam = uiSizeType;
        LPARAM lParam = MAKELPARAM(iWidth,iHeight);
        SendMessage(hStatusWnd,WM_SIZE,wParam,lParam);

        // specify number of panes and repaint
        RECT kRect;
        GetClientRect(pkTheApp->GetWindowHandle(),&kRect);
        int* aiPanes = new int[uiStatusPaneQuantity];
        for (unsigned int uiP = 0; uiP < uiStatusPaneQuantity-1; uiP++)
            aiPanes[uiP] = (uiP+1)*kRect.right/uiStatusPaneQuantity;
        aiPanes[uiStatusPaneQuantity-1] = kRect.right;

        wParam = WPARAM(uiStatusPaneQuantity);
        lParam = LPARAM(aiPanes);
        SendMessage(hStatusWnd,SB_SETPARTS,wParam,lParam);

        SendMessage(hStatusWnd,WM_PAINT,0,0);

        delete[] aiPanes;
    }

    return true;
}
//----------------------------------------------------------------------------
LRESULT CALLBACK WinProc (HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
    FWinApplication* pkTheApp = FWinApplication::GetApplication();
    if ( !pkTheApp )
        return DefWindowProc(hWnd,uiMsg,wParam,lParam);

    switch ( uiMsg ) 
    {
        case WM_CREATE:
        {
            LPCREATESTRUCT lpCS = LPCREATESTRUCT(lParam);
            if ( pkTheApp->OnCreate(lpCS) )
                return 0;
            else
                return -1;
        }
        case WM_DESTROY:
        {
            pkTheApp->OnDestroy();
            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT kPS;
            HDC hDC = BeginPaint(hWnd,&kPS);
            if ( pkTheApp->OnPaint(hDC) )
            {
                EndPaint(hWnd,&kPS);
                return 0;
            }
            EndPaint(hWnd,&kPS);
            break;
        }
        case WM_ERASEBKGND:
        {
            HDC hDC = HDC(hWnd);
            if ( pkTheApp->OnEraseBkgnd(hDC) )
                return 0;
            break;
        }
        case WM_MOVE:
        {
            int xPos = int(LOWORD(lParam));
            int yPos = int(HIWORD(lParam));
            if ( pkTheApp->OnMove(xPos,yPos) )
                return 0;
            break;
        }
        case WM_SIZE:
        {
            int iWidth = int(LOWORD(lParam));
            int iHeight = int(HIWORD(lParam));
            unsigned int uiSizeType = (unsigned int)(wParam);
            if ( pkTheApp->OnSize(iWidth,iHeight,uiSizeType) )
                return 0;
            break;
        }
        case WM_COMMAND:
        {
            WORD wNotifyCode = HIWORD(wParam);
            WORD wID = LOWORD(wParam);
            HWND hwndCtl = HWND(lParam);
            if ( pkTheApp->OnCommand(wNotifyCode,wID,hwndCtl) )
                return 0;
            break;
        }
        case WM_SYSCHAR:
        {
            char cCharCode = char(wParam);
            long lKeyCode = long(lParam);
            if ( pkTheApp->OnSysChar(cCharCode,lKeyCode) )
                return 0;
            break;
        }
        case WM_SYSKEYDOWN:
        {
            int iVirtKey = int(wParam);
            long lKeyCode = long(lParam);
            if ( pkTheApp->OnSysKeyDown(iVirtKey,lKeyCode) )
                return 0;
            break;
        }
        case WM_SYSKEYUP:
        {
            int iVirtKey = int(wParam);
            long lKeyCode = long(lParam);
            if ( pkTheApp->OnSysKeyUp(iVirtKey,lKeyCode) )
                return 0;
            break;
        }
        case WM_CHAR:
        {
            char cCharCode = char(wParam);
            long lKeyData = long(lParam);
            if ( pkTheApp->OnChar(cCharCode,lKeyData) )
                return 0;
            break;
        }
        case WM_KEYDOWN:
        {
            int iVirtKey = int(wParam);
            long lKeyData = long(lParam);
            if ( pkTheApp->OnKeyDown(iVirtKey,lKeyData) )
                return 0;
            break;
        }
        case WM_KEYUP:
        {
            int iVirtKey = int(wParam);
            long lKeyData = long(lParam);
            if ( pkTheApp->OnKeyUp(iVirtKey,lKeyData) )
                return 0;
            break;
        }
        case WM_LBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            unsigned int uiKeys = (unsigned int)(wParam);
            if ( pkTheApp->OnLButtonDown(iXPos,iYPos,uiKeys) )
                return 0;
            break;
        }
        case WM_LBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            unsigned int uiKeys = (unsigned int)(wParam);
            if ( pkTheApp->OnLButtonUp(iXPos,iYPos,uiKeys) )
                return 0;
            break;
        }
        case WM_RBUTTONDOWN:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            unsigned int uiKeys = (unsigned int)(wParam);
            if ( pkTheApp->OnRButtonDown(iXPos,iYPos,uiKeys) )
                return 0;
            break;
        }
        case WM_RBUTTONUP:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            unsigned int uiKeys = (unsigned int)(wParam);
            if ( pkTheApp->OnRButtonUp(iXPos,iYPos,uiKeys) )
                return 0;
            break;
        }
        case WM_MOUSEMOVE:
        {
            int iXPos = int(LOWORD(lParam));
            int iYPos = int(HIWORD(lParam));
            unsigned int uiKeys = (unsigned int)(wParam);
            if ( pkTheApp->OnMouseMove(iXPos,iYPos,uiKeys) )
                return 0;
            break;
        }
        default:
        {
            if ( pkTheApp->OnDefault(wParam,lParam) )
                return 0;
            break;
        }
    }

    return DefWindowProc(hWnd,uiMsg,wParam,lParam);
}
//----------------------------------------------------------------------------
static char* ProcessArgument (char* acArgument)
{
    int iLength = strlen(acArgument);

    // strip off quotes if command line was built from double-clicking a file
    char* acProcessed = new char[iLength+1];
	if ( acArgument[0] == '\"' )
    {
		strcpy(acProcessed,acArgument+1);  // remove leading quote
		if ( acArgument[iLength-1] == '\"' )
			acProcessed[iLength-2] = '\0';  // remove trailing quote
	}
	else
    {
		strcpy(acProcessed,acArgument);
    }

    return acProcessed;
}
//----------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR acArgument, int)
{
    FWinApplication* pkTheApp = FWinApplication::GetApplication();
    assert( pkTheApp );
    if ( !pkTheApp )
        return -1;

    gs_hInstance = hInstance;

    if ( acArgument && strlen(acArgument) > 0 )
    {
        char* acProcessed = ProcessArgument(acArgument);
        pkTheApp->SetCommand(new Command(acProcessed));
        delete[] acProcessed;
    }

    if ( !pkTheApp->OnPrecreate() )
        return -2;

    // register the window class
    WNDCLASS kWC;
    kWC.style         = CS_HREDRAW | CS_VREDRAW;
    kWC.lpfnWndProc   = WinProc;
    kWC.cbClsExtra    = 0;
    kWC.cbWndExtra    = 0;
    kWC.hInstance     = hInstance;
    kWC.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
    kWC.hCursor       = LoadCursor(NULL,IDC_ARROW);
    kWC.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    kWC.lpszClassName = gs_acWindowClassName;
    if ( pkTheApp->GetMenuID() )
        kWC.lpszMenuName  = MAKEINTRESOURCE(pkTheApp->GetMenuID());
    else
        kWC.lpszMenuName = 0;

    RegisterClass(&kWC);

    // require the window to have the specified client area
    RECT kRect = { 0, 0, pkTheApp->GetWidth()-1, pkTheApp->GetHeight()-1 };
    AdjustWindowRect(&kRect,gs_dwWindowStyle,pkTheApp->GetMenuID());

    // create the application window
    int iXPos = CW_USEDEFAULT;
    int iYPos = CW_USEDEFAULT;
    int iXSize = kRect.right - kRect.left + 1;
    int iYSize = kRect.bottom - kRect.top + 1;
    HWND hWnd = CreateWindow(gs_acWindowClassName,pkTheApp->GetCaption(),
        gs_dwWindowStyle,iXPos,iYPos,iXSize,iYSize,0,0,hInstance,0);

    pkTheApp->SetWindowHandle(hWnd);

    // create status window
    if ( pkTheApp->GetStatusPaneQuantity() )
    {
        InitCommonControls();

        HWND hStatusWnd = CreateWindow(STATUSCLASSNAME,"",
            WS_CHILD | WS_VISIBLE,0,0,0,0,hWnd,0,hInstance,0);

        pkTheApp->SetStatusWindowHandle(hStatusWnd);
    }

    // display the window
    ShowWindow(hWnd,SW_SHOW);
    UpdateWindow(hWnd);

    // allow the application to initialize before starting the message pump
    if ( !pkTheApp->OnInitialize() )
        return -3;

    MSG kMsg;
    while ( true )
    {
        if ( PeekMessage(&kMsg,0,0,0,PM_REMOVE) )
        {
            if ( kMsg.message == WM_QUIT )
                break;

            HACCEL hAccel = 0;
            if ( !TranslateAccelerator(hWnd,hAccel,&kMsg) )
            {
                TranslateMessage(&kMsg);
                DispatchMessage(&kMsg);
            }
        }
        else
        {
            pkTheApp->OnIdle();
        }
    }

    pkTheApp->OnTerminate();

    UnregisterClass(gs_acWindowClassName,hInstance);

    return 0;
}
//----------------------------------------------------------------------------


