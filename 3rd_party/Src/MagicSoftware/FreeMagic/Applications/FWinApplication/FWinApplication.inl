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

//----------------------------------------------------------------------------
inline bool FWinApplication::OnCreate (LPCREATESTRUCT)
{
    return true;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnDestroy ()
{
    return true;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnEraseBkgnd (HDC)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnMove (int, int)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnPaint (HDC hDC)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnCommand (WORD, WORD, HWND)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnSysChar (char, long)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnSysKeyDown (int, long)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnSysKeyUp (int, long)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnChar (char, long)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnKeyDown (int, long)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnKeyUp (int, long)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnLButtonDown (int, int, unsigned int)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnLButtonUp (int, int, unsigned int)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnRButtonDown (int, int, unsigned int)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnRButtonUp (int, int, unsigned int)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnMouseMove (int, int, unsigned int)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnDefault (WPARAM, LPARAM)
{
    return false;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnPrecreate ()
{
    return true;
}
//----------------------------------------------------------------------------
inline bool FWinApplication::OnInitialize ()
{
    return true;
}
//----------------------------------------------------------------------------
inline void FWinApplication::OnIdle ()
{
}
//----------------------------------------------------------------------------
inline void FWinApplication::OnTerminate ()
{
}
//----------------------------------------------------------------------------
inline FWinApplication* FWinApplication::GetApplication ()
{
    return ms_pkApplication;
}
//----------------------------------------------------------------------------
inline char* FWinApplication::GetCaption ()
{
    return ms_acCaption;
}
//----------------------------------------------------------------------------
inline int FWinApplication::GetWidth ()
{
    return ms_iWidth;
}
//----------------------------------------------------------------------------
inline int FWinApplication::GetHeight ()
{
    return ms_iHeight;
}
//----------------------------------------------------------------------------
inline unsigned int FWinApplication::GetMenuID ()
{
    return ms_uiMenuID;
}
//----------------------------------------------------------------------------
inline unsigned int FWinApplication::GetStatusPaneQuantity ()
{
    return ms_uiStatusPaneQuantity;
}
//----------------------------------------------------------------------------
inline HWND FWinApplication::GetWindowHandle ()
{
    return ms_hWnd;
}
//----------------------------------------------------------------------------
inline HWND FWinApplication::GetStatusWindowHandle ()
{
    return ms_hStatusWnd;
}
//----------------------------------------------------------------------------
inline Command* FWinApplication::GetCommand ()
{
    return ms_pkCommand;
}
//----------------------------------------------------------------------------
inline void FWinApplication::SetCommand (Command* pkCommand)
{
    ms_pkCommand = pkCommand;
}
//----------------------------------------------------------------------------
inline void FWinApplication::SetWindowHandle (HWND hWnd)
{
    ms_hWnd = hWnd;
}
//----------------------------------------------------------------------------
inline void FWinApplication::SetStatusWindowHandle (HWND hStatusWnd)
{
    ms_hStatusWnd = hStatusWnd;
}
//----------------------------------------------------------------------------
inline void FWinApplication::SetWidth (int iWidth)
{
    ms_iWidth = iWidth;
}
//----------------------------------------------------------------------------
inline void FWinApplication::SetHeight (int iHeight)
{
    ms_iHeight = iHeight;
}
//----------------------------------------------------------------------------


