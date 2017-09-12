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

#ifndef VOLUMERENDER_H
#define VOLUMERENDER_H

#include "RayTrace.h"
#include "FWinApplication.h"

class VolumeRender : public FWinApplication
{
public:
    VolumeRender (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual ~VolumeRender ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();

    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long);
    virtual bool OnLButtonDown (int iXPos, int iYPos, unsigned int);
    virtual bool OnLButtonUp (int iXPos, int iYPos, unsigned int);
    virtual bool OnMouseMove (int iXPos, int iYPos, unsigned int);

protected:
    void Draw ();

    HDC m_hWindowDC, m_hMemoryDC;
    HBITMAP m_hImage;
    int m_iBound;
    unsigned short* m_ausBits;

    RayTrace* m_pkRT;
    float m_fX0, m_fY0, m_fX1, m_fY1, m_fHBound, m_fGamma;
    bool m_bButtonDown;
};

#endif


