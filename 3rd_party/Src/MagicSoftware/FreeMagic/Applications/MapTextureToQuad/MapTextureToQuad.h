
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

#ifndef MAPTEXTURETOQUAD_H
#define MAPTEXTURETOQUAD_H

#include "FWinApplication.h"
#include "MgcQuadToQuadTransforms.h"

class MapTextureToQuad : public FWinApplication
{
public:
    MapTextureToQuad (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual ~MapTextureToQuad ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long);

    // allows user to drag vertices of convex quadrilateral
    virtual bool OnLButtonDown (int iXPos, int iYPos, unsigned int uiKeys);
    virtual bool OnLButtonUp (int iXPos, int iYPos, unsigned int uiKeys);
    virtual bool OnMouseMove (int iXPos, int iYPos, unsigned int uiKeys);

protected:
    // the bitmap image to perspectively draw onto the convex quadrilateral.
    HBITMAP m_hSrcBitmap;
    unsigned char* m_aucSrcBits;
    int m_iXSize, m_iYSize;

    // the target bitmap image
    HDC m_hTrgWindowDC, m_hTrgMemoryDC;
    HBITMAP m_hTrgBitmap;
    unsigned char* m_aucTrgBits;

    // The four vertices of the convex quadrilateral in counterclockwise
    // order:  Q00 = V[0], Q10 = V[1], Q11 = V[2], Q01 = V[3].
    void CreateMapping ();
    Vector2* m_akVertex;
    HmQuadToSqr* m_pkMap;

    // for drag of quadrilateral vertices
    void SelectVertex (const Vector2& rkPos);
    void UpdateQuadrilateral (const Vector2& rkPos);
    HDC m_hDragDC;
    bool m_bButtonDown;
    int m_iSelected;
};

#endif


