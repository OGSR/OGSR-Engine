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

#include "TestTriTri.h"
#include "MgcIntr2DTriTri.h"

COLORREF TestTriTri::BLACK = RGB(0,0,0);
COLORREF TestTriTri::GRAY = RGB(128,128,128);
COLORREF TestTriTri::RED = RGB(255,0,0);
COLORREF TestTriTri::BLUE = RGB(0,0,255);
COLORREF TestTriTri::PURPLE = RGB(255,0,255);
COLORREF TestTriTri::LIGHT_RED = RGB(128,0,0);
COLORREF TestTriTri::LIGHT_BLUE = RGB(0,0,128);
COLORREF TestTriTri::LIGHT_PURPLE = RGB(128,0,128);

const int g_iSize = 256;
TestTriTri g_kTheApp("TestTriTri",g_iSize,g_iSize,0,1);

//----------------------------------------------------------------------------
TestTriTri::TestTriTri (char* acCaption, int iWidth, int iHeight,
    unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
    m_hDragDC = 0;
    m_bMouseDown = false;
    m_iTriangle = -1;
    m_iSelect = -1;
    m_iQuantity = 0;
    m_eType = TT_TEST;

    // create initial triangles
    int iW = GetWidth(), iH = GetHeight();
    m_aiX0[0] = iW/8;    m_aiY0[0] = iH/8;
    m_aiX0[1] = 3*iW/8;  m_aiY0[1] = iH/8;
    m_aiX0[2] = iW/8;    m_aiY0[2] = 3*iH/8;
    m_aiX1[0] = 7*iW/8;  m_aiY1[0] = 7*iH/8;
    m_aiX1[1] = 5*iW/8;  m_aiY1[1] = 7*iH/8;
    m_aiX1[2] = 7*iW/8;  m_aiY1[2] = 5*iH/8;

    m_uiAngle0 = 0;
    m_uiAngle1 = 0;
    m_uiSpeed0 = 0;
    m_uiSpeed1 = 0;

    GetIntersecting();
}
//----------------------------------------------------------------------------
bool TestTriTri::OnInitialize ()
{
    WriteStatus();
    return true;
}
//----------------------------------------------------------------------------
bool TestTriTri::OnPaint (HDC hDC)
{
    char acMsg[64];

    switch ( m_eType )
    {
    case TT_TEST:
        strcpy(acMsg,"TEST: ");
        DrawTriangle(hDC,m_aiX0,m_aiY0,RED);
        DrawTriangle(hDC,m_aiX1,m_aiY1,BLUE);
        break;
    case TT_FIND:
        strcpy(acMsg,"FIND: ");
        DrawTriangle(hDC,m_aiX0,m_aiY0,RED);
        DrawTriangle(hDC,m_aiX1,m_aiY1,BLUE);
        DrawIntersection(hDC);
        break;
    case TT_TEST_VEL:
        strcpy(acMsg,"TEST VEL: ");
        DrawTriangle(hDC,m_aiX0,m_aiY0,RED);
        DrawTriangle(hDC,m_aiX1,m_aiY1,BLUE);
        DrawMovedTriangle(hDC,m_aiX0,m_aiY0,m_uiSpeed0,m_uiAngle0,LIGHT_RED);
        DrawMovedTriangle(hDC,m_aiX1,m_aiY1,m_uiSpeed1,m_uiAngle1,LIGHT_BLUE);
        break;
    case TT_FIND_VEL:
        strcpy(acMsg,"FIND VEL: ");
        DrawTriangle(hDC,m_aiX0,m_aiY0,RED);
        DrawTriangle(hDC,m_aiX1,m_aiY1,BLUE);
        DrawMovedTriangle(hDC,m_aiX0,m_aiY0,m_uiSpeed0,m_uiAngle0,LIGHT_RED);
        DrawMovedTriangle(hDC,m_aiX1,m_aiY1,m_uiSpeed1,m_uiAngle1,LIGHT_BLUE);
        DrawIntersection(hDC);
        break;
    }

    strcat(acMsg,"Intersecting = ");
    strcat(acMsg,(m_bIntersecting ? "YES" : "NO"));
    TextOut(hDC,0,0,acMsg,strlen(acMsg));
 
    return true;
}
//----------------------------------------------------------------------------
bool TestTriTri::OnChar (char cCharCode, long)
{
    switch ( cCharCode )
    {
    // select next intersection type
    case 't':
        m_eType = Type((m_eType+1) % TT_MAX);
        GetIntersecting();
        return true;

    // rotate velocity vector of active triangle
    case '+':
    case '=':
        if ( m_iTriangle == 0 )
            m_uiAngle0 = (m_uiAngle0 + 1) % MAX_ANGLE;
        else if ( m_iTriangle == 1 )
            m_uiAngle1 = (m_uiAngle1 + 1) % MAX_ANGLE;
        GetIntersecting();
        WriteStatus();
        return true;

    // rotate velocity vector of active triangle
    case '-':
    case '_':
        if ( m_iTriangle == 0 )
        {
            if ( m_uiAngle0 > 0 )
                m_uiAngle0--;
            else
                m_uiAngle0 = MAX_ANGLE - 1;
        }
        else if ( m_iTriangle == 1 )
        {
            if ( m_uiAngle1 > 0 )
                m_uiAngle1--;
            else
                m_uiAngle1 = MAX_ANGLE - 1;
        }
        GetIntersecting();
        WriteStatus();
        return true;

    // increase speed of active triangle
    case '>':
    case '.':
        if ( m_iTriangle == 0 )
        {
            if ( m_uiSpeed0 < MAX_SPEED )
                m_uiSpeed0++;
        }
        else if ( m_iTriangle == 1 )
        {
            if ( m_uiSpeed1 < MAX_SPEED )
                m_uiSpeed1++;
        }
        GetIntersecting();
        WriteStatus();
        return true;

    // decrease speed of active triangle
    case '<':
    case ',':
        if ( m_iTriangle == 0 )
        {
            if ( m_uiSpeed0 > 0 )
                m_uiSpeed0--;
        }
        else if ( m_iTriangle == 1 )
        {
            if ( m_uiSpeed1 > 0 )
                m_uiSpeed1--;
        }
        GetIntersecting();
        WriteStatus();
        return true;

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
bool TestTriTri::OnLButtonDown (int iXPos, int iYPos, unsigned int)
{
    if ( !m_bMouseDown )
    {
        m_bMouseDown = true;
        SetCapture(ms_hWnd);
        m_hDragDC = GetDC(ms_hWnd);

        m_iTriangle = -1;
        m_iSelect = -1;

        if ( 0 <= iXPos && iXPos < GetWidth()
        &&   0 <= iYPos && iYPos < GetHeight() )
        {
            COLORREF kColor = GetPixel(m_hDragDC,iXPos,iYPos);
            iYPos = Flip(iYPos);

            int i;
            for (i = 0; i < 3; i++)
            {
                if ( MouseNearVertex(iXPos,iYPos,m_aiX0[i],m_aiY0[i]) )
                {
                    m_iTriangle = 0;
                    m_iSelect = i;
                    return true;
                }
            }

            for (i = 0; i < 3; i++)
            {
                if ( MouseNearVertex(iXPos,iYPos,m_aiX1[i],m_aiY1[i]) )
                {
                    m_iTriangle = 1;
                    m_iSelect = i;
                    return true;
                }
            }

            if ( MouseInTriangle(iXPos,iYPos,m_aiX0,m_aiY0) )
            {
                m_iTriangle = 0;
                m_iXMouseStart = iXPos;
                m_iYMouseStart = iYPos;
                return true;
            }

            if ( MouseInTriangle(iXPos,iYPos,m_aiX1,m_aiY1) )
            {
                m_iTriangle = 1;
                m_iXMouseStart = iXPos;
                m_iYMouseStart = iYPos;
                return true;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool TestTriTri::OnLButtonUp (int iXPos, int iYPos, unsigned int)
{
    if ( m_bMouseDown )
    {
        m_bMouseDown = false;
        ReleaseCapture();
        ReleaseDC(ms_hWnd,m_hDragDC);

        iYPos = Flip(iYPos);

        AdjustTriangle(iXPos,iYPos);
    }

    return false;
}
//----------------------------------------------------------------------------
bool TestTriTri::OnMouseMove (int iXPos, int iYPos, unsigned int)
{
    // allow mouse dragging for continuous update/display of pixel values
    if ( m_bMouseDown )
    {
        if ( 0 <= iXPos && iXPos < GetWidth()
        &&   0 <= iYPos && iYPos < GetHeight() )
        {
            iYPos = Flip(iYPos);
            AdjustTriangle(iXPos,iYPos);
            m_iXMouseStart = iXPos;
            m_iYMouseStart = iYPos;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void TestTriTri::WriteStatus ()
{
    char acMsg[256];
    sprintf(acMsg,"tri = %d , spd0 = %u  ang0 = %u  spd1 = %u  ang1 = %u",
        m_iTriangle, m_uiSpeed0, m_uiAngle0, m_uiSpeed1, m_uiAngle1);

    SendMessage(GetStatusWindowHandle(),SB_SETTEXT,(WPARAM)0,
        (LPARAM)acMsg);
    SendMessage(GetStatusWindowHandle(),WM_PAINT,0,0); 
}
//----------------------------------------------------------------------------
Vector2 TestTriTri::GetVelocity (unsigned int uiSpeed, unsigned int uiAngle)
{
    Real fAngle = Math::TWO_PI*uiAngle/MAX_ANGLE;
    Real fCos = Math::Cos(fAngle);
    Real fSin = Math::Sin(fAngle);
    return Vector2(uiSpeed*fCos,uiSpeed*fSin);
}
//----------------------------------------------------------------------------
void TestTriTri::GetIntersecting ()
{
    Vector2 akV0[3], akV1[3];
    for (int i = 0; i < 3; i++)
    {
        akV0[i].x = (Real) m_aiX0[i];
        akV0[i].y = (Real) m_aiY0[i];
        akV1[i].x = (Real) m_aiX1[i];
        akV1[i].y = (Real) m_aiY1[i];
    }

    Vector2 kW0, kW1;
    Real fTLast, fTMax = 1.0;
    m_iQuantity = 0;

    switch ( m_eType )
    {
    case TT_TEST:
        m_bIntersecting = TestIntersection(akV0,akV1);
        break;
    case TT_FIND:
        m_bIntersecting = FindIntersection(akV0,akV1,m_iQuantity,
            m_akVertex);
        break;
    case TT_TEST_VEL:
        kW0 = GetVelocity(m_uiSpeed0,m_uiAngle0);
        kW1 = GetVelocity(m_uiSpeed1,m_uiAngle1);
        m_bIntersecting = TestIntersection(fTMax,akV0,kW0,akV1,kW1,
            m_fTFirst,fTLast);
        break;
    case TT_FIND_VEL:
        kW0 = GetVelocity(m_uiSpeed0,m_uiAngle0);
        kW1 = GetVelocity(m_uiSpeed1,m_uiAngle1);
        m_bIntersecting = FindIntersection(fTMax,akV0,kW0,akV1,kW1,
            m_fTFirst,fTLast,m_iQuantity,m_akVertex);
        break;
    }

    InvalidateRect(GetWindowHandle(),NULL,TRUE);
}
//----------------------------------------------------------------------------
void TestTriTri::DrawTriangle (HDC hDC, int aiX[3], int aiY[3],
    COLORREF kColor)
{
    // draw triangle boundary
    HPEN hPen = CreatePen(PS_SOLID,1,kColor);
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);
    MoveToEx(hDC,aiX[0],Flip(aiY[0]),NULL);
    LineTo(hDC,aiX[1],Flip(aiY[1]));
    LineTo(hDC,aiX[2],Flip(aiY[2]));
    LineTo(hDC,aiX[0],Flip(aiY[0]));
    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
void TestTriTri::DrawMovedTriangle (HDC hDC, int aiX[3], int aiY[3],
    unsigned int uiSpeed, unsigned int uiAngle, COLORREF kColor)
{
    Vector2 kW = GetVelocity(uiSpeed,uiAngle);
    if ( m_bIntersecting )
        kW *= m_fTFirst;

    int aiMoveX[3] =
    {
        (int)(aiX[0] + kW.x),
        (int)(aiX[1] + kW.x),
        (int)(aiX[2] + kW.x)
    };

    int aiMoveY[3] =
    {
        (int)(aiY[0] + kW.y),
        (int)(aiY[1] + kW.y),
        (int)(aiY[2] + kW.y)
    };

    HPEN hPen = CreatePen(PS_SOLID,1,kColor);
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);

    // draw triangle boundary
    MoveToEx(hDC,aiMoveX[0],Flip(aiMoveY[0]),NULL);
    LineTo(hDC,aiMoveX[1],Flip(aiMoveY[1]));
    LineTo(hDC,aiMoveX[2],Flip(aiMoveY[2]));
    LineTo(hDC,aiMoveX[0],Flip(aiMoveY[0]));

    // connect start/final triangle vertices
    for (int i = 0; i < 3; i++)
    {
        MoveToEx(hDC,aiX[i],Flip(aiY[i]),NULL);
        LineTo(hDC,aiMoveX[i],Flip(aiMoveY[i]));
    }

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
void TestTriTri::DrawIntersection (HDC hDC)
{
    if ( m_iQuantity == 0 )
        return;

    HPEN hPen = CreatePen(PS_SOLID,2,PURPLE);
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);
    MoveToEx(hDC,(int)m_akVertex[0].x,Flip((int)m_akVertex[0].y),NULL);
    LineTo(hDC,(int)m_akVertex[0].x,Flip((int)m_akVertex[0].y));
    for (int i = 1; i < m_iQuantity; i++)
        LineTo(hDC,(int)m_akVertex[i].x,Flip((int)m_akVertex[i].y));
    LineTo(hDC,(int)m_akVertex[0].x,Flip((int)m_akVertex[0].y));
    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
bool TestTriTri::MouseNearVertex (int iXPos, int iYPos, int iXVer, int iYVer)
{
    const int iThick = 3;
    return abs(iXPos - iXVer) <= iThick && abs(iYPos - iYVer) <= iThick;
}
//----------------------------------------------------------------------------
bool TestTriTri::MouseInTriangle (int iXPos, int iYPos, int aiX[3],
    int aiY[3])
{
    int iE0x = aiX[1] - aiX[0], iE0y = aiY[1] - aiY[0];
    int iE1x = aiX[2] - aiX[0], iE1y = aiY[2] - aiY[0];
    int iDx = iXPos - aiX[0], iDy = iYPos - aiY[0];
    int iE00 = iE0x*iE0x + iE0y*iE0y;
    int iE01 = iE0x*iE1x + iE0y*iE1y;
    int iE11 = iE1x*iE1x + iE1y*iE1y;
    int iR0 = iE0x*iDx + iE0y*iDy;
    int iR1 = iE1x*iDx + iE1y*iDy;
    int iS0 = iE11*iR0 - iE01*iR1;
    int iS1 = iE00*iR1 - iE01*iR0;
    int iS2 = iE00*iE11 - iE01*iE01;
    return 0 <= iS0 && 0 <= iS1 && iS0+iS1 <= iS2;
}
//----------------------------------------------------------------------------
void TestTriTri::AdjustTriangle (int iXPos, int iYPos)
{
    int i, iDx, iDy;

    if ( m_iTriangle == 0 )
    {
        if ( m_iSelect == -1 )
        {
            iDx = iXPos - m_iXMouseStart;
            iDy = iYPos - m_iYMouseStart;
            for (i = 0; i < 3; i++)
            {
                m_aiX0[i] += iDx;
                m_aiY0[i] += iDy;
            }
        }
        else
        {
            m_aiX0[m_iSelect] = iXPos;
            m_aiY0[m_iSelect] = iYPos;
        }
    }
    else if ( m_iTriangle == 1 )
    {
        if ( m_iSelect == -1 )
        {
            iDx = iXPos - m_iXMouseStart;
            iDy = iYPos - m_iYMouseStart;
            for (i = 0; i < 3; i++)
            {
                m_aiX1[i] += iDx;
                m_aiY1[i] += iDy;
            }
        }
        else
        {
            m_aiX1[m_iSelect] = iXPos;
            m_aiY1[m_iSelect] = iYPos;
        }
    }

    GetIntersecting();
    WriteStatus();
}
//----------------------------------------------------------------------------


