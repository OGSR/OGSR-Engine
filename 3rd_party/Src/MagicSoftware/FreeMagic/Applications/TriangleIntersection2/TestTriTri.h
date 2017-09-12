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

#ifndef TESTTRITRI_H
#define TESTTRITRI_H

#include "FWinApplication.h"
#include "MgcVector2.h"


class TestTriTri : public FWinApplication
{
public:
    TestTriTri (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual bool OnInitialize ();
    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long);
    virtual bool OnLButtonDown (int iXPos, int iYPos, unsigned int);
    virtual bool OnLButtonUp (int iXPos, int iYPos, unsigned int);
    virtual bool OnMouseMove (int iXPos, int iYPos, unsigned int);

protected:
    void WriteStatus ();

    Vector2 GetVelocity (unsigned int uiSpeed, unsigned int uiAngle);
    void GetIntersecting ();

    int Flip (int iY) { return GetHeight() - 1 - iY; }
    void DrawTriangle (HDC hDC, int aiX[3], int aiY[3], COLORREF kColor);
    void DrawMovedTriangle (HDC hDC, int aiX[3], int aiY[3],
        unsigned int uiSpeed, unsigned int uiAngle, COLORREF kColor);
    void DrawIntersection (HDC hDC);

    bool MouseNearVertex (int iXPos, int iYPos, int iXVer, int iYVer);
    bool MouseInTriangle (int iXPos, int iYPos, int aiX[3], int aiY[3]);
    void AdjustTriangle (int iXPos, int iYPos);

    static COLORREF BLACK, GRAY, RED, BLUE, PURPLE, LIGHT_RED, LIGHT_BLUE,
        LIGHT_PURPLE;

    HDC m_hDragDC;
    bool m_bMouseDown, m_bIntersecting;
    int m_iTriangle, m_iSelect, m_iXMouseStart, m_iYMouseStart;

    enum Type { TT_TEST, TT_FIND, TT_TEST_VEL, TT_FIND_VEL, TT_MAX };
    Type m_eType;

    // two triangles
    int m_aiX0[3], m_aiY0[3], m_aiX1[3], m_aiY1[3];

    // velocity information
    enum { MAX_SPEED = 64, MAX_ANGLE = 32 };
    unsigned int m_uiSpeed0, m_uiSpeed1;
    unsigned int m_uiAngle0, m_uiAngle1;
    Real m_fTFirst;

    // intersection of triangles
    int m_iQuantity;
    Vector2 m_akVertex[6];
};

#endif


