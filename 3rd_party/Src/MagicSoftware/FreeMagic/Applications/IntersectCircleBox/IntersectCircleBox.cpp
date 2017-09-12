// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include "IntersectCircleBox.h"

const int g_iSize = 256;
IntersectCircleBox g_kTheApp("IntersectCircleBox",g_iSize,g_iSize);

//----------------------------------------------------------------------------
IntersectCircleBox::IntersectCircleBox (char* acCaption, int iWidth,
    int iHeight, unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
    m_hDragDC = 0;
    m_bMouseDown = false;
}
//----------------------------------------------------------------------------
bool IntersectCircleBox::OnInitialize ()
{
    m_kBox.Center() = Vector2(150.0f,125.0f);
    m_kBox.Axis(0) = Vector2::UNIT_X;
    m_kBox.Axis(1) = Vector2::UNIT_Y;
    m_kBox.Extent(0) = 50.0f;
    m_kBox.Extent(1) = 25.0f;

    m_kC.x = 20.0f;
    m_kC.y = 135.0f;
    m_fR = 10.0f;

    m_kV.x = 1.0f;
    m_kV.y = 0.0f;

    m_iType = FindIntersection(m_kC,m_fR,m_kV,m_kBox,m_fTFirst,m_kIntr);

    InvalidateRect(GetWindowHandle(),NULL,TRUE);
    return true;
}
//----------------------------------------------------------------------------
void IntersectCircleBox::DrawCircle (HDC hDC)
{
    // draw circle
    HPEN hPen = CreatePen(PS_SOLID,1,RGB(0,0,255));
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);

    int iXC = (int)m_kC.x;
    int iYC = (int)m_kC.y;
    int iR = (int)m_fR;
    Arc(hDC,iXC-iR,iYC+iR,iXC+iR,iYC-iR,iXC+iR,iYC,iXC+iR,iYC);

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);

    // draw rays of motion
    hPen = CreatePen(PS_SOLID,1,RGB(192,0,0));
    hOldPen = SelectObject(hDC,hPen);
    float fT = (float)g_iSize;
    Vector2 kVPerp = m_kV.Cross();

    Vector2 kStart = m_kC, kFinal = m_kC + fT*m_kV;
    int iX0 = (int)kStart.x, iY0 = (int)kStart.y;
    int iX1 = (int)kFinal.x, iY1 = (int)kFinal.y;
    MoveToEx(hDC,iX0,iY0,NULL);
    LineTo(hDC,iX1,iY1);

    kStart = m_kC + m_fR*kVPerp;
    kFinal = kStart + fT*m_kV;
    iX0 = (int)kStart.x;
    iY0 = (int)kStart.y;
    iX1 = (int)kFinal.x;
    iY1 = (int)kFinal.y;
    MoveToEx(hDC,iX0,iY0,NULL);
    LineTo(hDC,iX1,iY1);

    kStart = m_kC - m_fR*kVPerp;
    kFinal = kStart + fT*m_kV;
    iX0 = (int)kStart.x;
    iY0 = (int)kStart.y;
    iX1 = (int)kFinal.x;
    iY1 = (int)kFinal.y;
    MoveToEx(hDC,iX0,iY0,NULL);
    LineTo(hDC,iX1,iY1);

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
void IntersectCircleBox::DrawRectangle (HDC hDC)
{
    int iXMin = (int)(m_kBox.Center().x - m_kBox.Extent(0));
    int iXMax = (int)(m_kBox.Center().x + m_kBox.Extent(0));
    int iYMin = (int)(m_kBox.Center().y - m_kBox.Extent(1));
    int iYMax = (int)(m_kBox.Center().y + m_kBox.Extent(1));

    HPEN hPen = CreatePen(PS_SOLID,1,RGB(192,192,192));
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);

    MoveToEx(hDC,iXMin,0,NULL);
    LineTo(hDC,iXMin,g_iSize-1);
    MoveToEx(hDC,iXMax,0,NULL);
    LineTo(hDC,iXMax,g_iSize-1);
    MoveToEx(hDC,0,iYMin,NULL);
    LineTo(hDC,g_iSize-1,iYMin);
    MoveToEx(hDC,0,iYMax,NULL);
    LineTo(hDC,g_iSize-1,iYMax);

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);

    hPen = CreatePen(PS_SOLID,1,RGB(0,0,0));
    hOldPen = SelectObject(hDC,hPen);

    MoveToEx(hDC,iXMin,iYMin,NULL);
    LineTo(hDC,iXMax,iYMin);
    LineTo(hDC,iXMax,iYMax);
    LineTo(hDC,iXMin,iYMax);
    LineTo(hDC,iXMin,iYMin);

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
void IntersectCircleBox::DrawIntersection (HDC hDC)
{
    HPEN hPen = CreatePen(PS_SOLID,1,RGB(255,0,255));
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);

    Vector2 kMoved = m_kC + m_fTFirst*m_kV;
    int iXC = (int)kMoved.x;
    int iYC = (int)kMoved.y;
    int iR = (int)m_fR;
    Arc(hDC,iXC-iR,iYC+iR,iXC+iR,iYC-iR,iXC+iR,iYC,iXC+iR,iYC);

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);

    int iX = (int)m_kIntr.x;
    int iY = (int)m_kIntr.y;
    SetPixel(hDC,iX-1,iY-1,RGB(0,255,0));
    SetPixel(hDC,iX-1,iY  ,RGB(0,255,0));
    SetPixel(hDC,iX-1,iY+1,RGB(0,255,0));
    SetPixel(hDC,iX  ,iY-1,RGB(0,255,0));
    SetPixel(hDC,iX  ,iY  ,RGB(0,255,0));
    SetPixel(hDC,iX  ,iY+1,RGB(0,255,0));
    SetPixel(hDC,iX+1,iY-1,RGB(0,255,0));
    SetPixel(hDC,iX+1,iY  ,RGB(0,255,0));
    SetPixel(hDC,iX+1,iY+1,RGB(0,255,0));
}
//----------------------------------------------------------------------------
bool IntersectCircleBox::OnPaint (HDC hDC)
{
    DrawCircle(hDC);
    DrawRectangle(hDC);
    if ( m_iType == 1 )
        DrawIntersection(hDC);
    return true;
}
//----------------------------------------------------------------------------
bool IntersectCircleBox::OnChar (char cCharCode, long)
{
    switch ( cCharCode )
    {
    // exit program
    case 'q':
    case 'Q':
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(),WM_DESTROY,0,0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool IntersectCircleBox::OnLButtonDown (int iXPos, int iYPos, unsigned int)
{
    if ( !m_bMouseDown )
    {
        m_bMouseDown = true;
        SetCapture(GetWindowHandle());
        m_hDragDC = GetDC(GetWindowHandle());

        float fDx = iXPos - m_kC.x;
        float fDy = iYPos - m_kC.y;

        if ( fDx*fDx +fDy*fDy <= m_fR*m_fR )
        {
            // mouse click inside circle, move circle
            m_kC.x = (float)iXPos;
            m_kC.y = (float)iYPos;
        }
        else
        {
            // mouse click outside circle, change velocity
            m_kV.x = fDx;
            m_kV.y = fDy;
            m_kV.Unitize();
        }

        m_iType = FindIntersection(m_kC,m_fR,m_kV,m_kBox,m_fTFirst,m_kIntr);
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
    }
    return true;
}
//----------------------------------------------------------------------------
bool IntersectCircleBox::OnLButtonUp (int iXPos, int iYPos, unsigned int)
{
    if ( m_bMouseDown )
    {
        m_bMouseDown = false;
        ReleaseCapture();
        ReleaseDC(GetWindowHandle(),m_hDragDC);
    }

    return false;
}
//----------------------------------------------------------------------------
bool IntersectCircleBox::OnMouseMove (int iXPos, int iYPos, unsigned int)
{
    if ( m_bMouseDown )
    {
        float fDx = iXPos - m_kC.x;
        float fDy = iYPos - m_kC.y;

        if ( fDx*fDx +fDy*fDy <= m_fR*m_fR )
        {
            // mouse click inside circle, move circle
            m_kC.x = (float)iXPos;
            m_kC.y = (float)iYPos;
        }
        else
        {
            // mouse click outside circle, change velocity
            m_kV.x = fDx;
            m_kV.y = fDy;
            m_kV.Unitize();
        }

        m_iType = FindIntersection(m_kC,m_fR,m_kV,m_kBox,m_fTFirst,m_kIntr);
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
    }

    return false;
}
//----------------------------------------------------------------------------
