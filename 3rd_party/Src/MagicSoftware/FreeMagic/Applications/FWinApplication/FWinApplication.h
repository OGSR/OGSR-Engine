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

#ifndef FWINAPPLICATION_H
#define FWINAPPLICATION_H

#include <windows.h>
#include <commctrl.h>
#include "MgcCommand.h"
using namespace Mgc;


class FWinApplication
{
public:
    virtual ~FWinApplication ();

    // WinProc callbacks. Return true if event processed or if you want
    // Windows to further process the event.  Return false otherwise.

    // creation and destruction
    virtual bool OnCreate (LPCREATESTRUCT lpCS);
    virtual bool OnDestroy ();

    // command processing
    virtual bool OnCommand (WORD wNotifyCode, WORD wID, HWND hCtrl);

    // window resizing and painting
    virtual bool OnPaint (HDC hDC);
    virtual bool OnEraseBkgnd (HDC hDC);
    virtual bool OnMove (int iXPos, int iYPos);
    virtual bool OnSize (int iWidth, int iHeight, unsigned int uiSizeType);

    // keyboard
    virtual bool OnSysChar (char cCharCode, long lKeyData);
    virtual bool OnSysKeyDown (int iVirtKey, long lKeyData);
    virtual bool OnSysKeyUp (int iVirtKey, long lKeyData);
    virtual bool OnChar (char cCharCode, long lKeyData);
    virtual bool OnKeyDown (int iVirtKey, long lKeyData);
    virtual bool OnKeyUp (int iVirtKey, long lKeyData);

    // mouse
    virtual bool OnLButtonDown (int iXPos, int iYPos, unsigned int uiKeys);
    virtual bool OnLButtonUp (int iXPos, int iYPos, unsigned int uiKeys);
    virtual bool OnRButtonDown (int iXPos, int iYPos, unsigned int uiKeys);
    virtual bool OnRButtonUp (int iXPos, int iYPos, unsigned int uiKeys);
    virtual bool OnMouseMove (int iXPos, int iYPos, unsigned int uiKeys);

    // default processing
    virtual bool OnDefault (WPARAM wParam, LPARAM lParam);

    // The OnPrecreate function is called by WinMain after the command
    // line is processed for parsing, but before the main window creation.
    // This is a good place to adjust the application window width and
    // height.
    virtual bool OnPrecreate ();

    // The OnInitialize function is called by WinMain after ShowWindow and
    // UpdateWindow, but before the idle loop.  The window handles ms_hWnd
    // and ms_hStatusWnd are valid at this point, so any derived class may
    // safely use the handles.  The routine should be used for allocating
    // resources and initializing any data values.
    virtual bool OnInitialize ();

    // Called when the message pump is idle.
    virtual void OnIdle ();

    // This is called by WinMain after the idle loop exits.  The application
    // should free up its resources in this routine.
    virtual void OnTerminate ();

    // access by WinMain
    static FWinApplication* GetApplication ();
    static char* GetCaption ();
    static int GetWidth ();
    static int GetHeight ();
    static unsigned int GetMenuID ();
    static unsigned int GetStatusPaneQuantity ();
    Command* GetCommand ();
    static HWND GetWindowHandle ();
    static HWND GetStatusWindowHandle ();
    static void SetCommand (Command* pkCommand);
    static void SetWindowHandle (HWND hWnd);
    static void SetStatusWindowHandle (HWND hStatusWnd);
    static void SetWidth (int iWidth);
    static void SetHeight (int iHeight);

protected:
    // abstract base class
    FWinApplication (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    static char* ms_acCaption;
    static int ms_iWidth;
    static int ms_iHeight;
    static unsigned int ms_uiMenuID;
    static unsigned int ms_uiStatusPaneQuantity;

    static HWND ms_hWnd;
    static HWND ms_hStatusWnd;
    static FWinApplication* ms_pkApplication;
    static Command* ms_pkCommand;
};

#include "FWinApplication.inl"

#endif


