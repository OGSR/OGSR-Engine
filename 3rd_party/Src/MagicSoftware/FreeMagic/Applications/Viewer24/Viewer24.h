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

#ifndef VIEWER24_H
#define VIEWER24_H

#include "FWinApplication.h"

class Viewer24 : public FWinApplication
{
public:
    Viewer24 (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual ~Viewer24 ();

    virtual bool OnPrecreate ();
    virtual bool OnInitialize ();
    virtual void OnTerminate ();

    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long);
    virtual bool OnKeyDown (int iVirtKey, long);
    virtual bool OnLButtonDown (int iXPos, int iYPos, unsigned int);
    virtual bool OnLButtonUp (int iXPos, int iYPos, unsigned int);
    virtual bool OnMouseMove (int iXPos, int iYPos, unsigned int);

protected:
    void WritePixelString ();

    HBITMAP m_hImage;
    HDC m_hDragDC;
    bool m_bMouseDown;

    int m_iDimensions, m_iQuantity, m_iSliceQuantity;
    int* m_aiBound;
    float* m_afData;
    float m_fMin, m_fMax, m_fRange;
    unsigned int* m_auiData;
    int m_iZ;
    char m_acPixelStr[256];
    unsigned int* m_auiBits;
    char* m_acBMI;
    char m_acFilename[512];
};

#endif


