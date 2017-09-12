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

#ifndef INTERSECTCIRCLEBOX_H
#define INTERSECTCIRCLEBOX_H

#include "FWinApplication.h"
#include "MgcIntr2DCirBox.h"


class IntersectCircleBox : public FWinApplication
{
public:
    IntersectCircleBox (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual bool OnInitialize ();
    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long);
    virtual bool OnLButtonDown (int iXPos, int iYPos, unsigned int);
    virtual bool OnLButtonUp (int iXPos, int iYPos, unsigned int);
    virtual bool OnMouseMove (int iXPos, int iYPos, unsigned int);

protected:
    void DrawCircle (HDC hDC);
    void DrawRectangle (HDC hDC);
    void DrawIntersection (HDC hDC);

    // moving circle
    Vector2 m_kC, m_kV;
    Real m_fR;

    // rectangle
    Box2 m_kBox;

    // intersection
    int m_iType;
    Real m_fTFirst;
    Vector2 m_kIntr;

    HDC m_hDragDC;
    bool m_bMouseDown;
};

#endif


