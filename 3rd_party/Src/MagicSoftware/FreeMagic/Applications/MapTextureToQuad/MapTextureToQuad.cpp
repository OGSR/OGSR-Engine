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

#include "MapTextureToQuad.h"
#include "MgcCont2DPointInPolygon.h"

MapTextureToQuad g_kTheApp("MapTextureToQuad",256,256);

//----------------------------------------------------------------------------
MapTextureToQuad::MapTextureToQuad (char* acCaption, int iWidth,
    int iHeight, unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
    m_pkMap = NULL;
    m_hSrcBitmap = 0;
    m_aucSrcBits = NULL;

    m_hTrgWindowDC = 0;
    m_hTrgMemoryDC = 0;
    m_hTrgBitmap = 0;
    m_aucTrgBits = NULL;

    m_iXSize = 0;
    m_iYSize = 0;
    m_akVertex = NULL;

    m_bButtonDown = false;
    m_iSelected = -1;
}
//----------------------------------------------------------------------------
MapTextureToQuad::~MapTextureToQuad ()
{
}
//----------------------------------------------------------------------------
void MapTextureToQuad::CreateMapping ()
{
    // Create the new perspective mapping from the *target* quadrilateral to
    // the *source* square bitmap.  The mapping is in this direction to avoid
    // holes in the drawn quadrilateral.
    delete m_pkMap;
    m_pkMap = new HmQuadToSqr(m_akVertex[0],m_akVertex[1],m_akVertex[2],
        m_akVertex[3]);

    // compute axis-aligned bounding box
    int iXMin = GetWidth(), iXMax = 0, iYMin = GetWidth(), iYMax = 0;
    for (int i = 0; i < 4; i++)
    {
        if ( iXMin > (int)m_akVertex[i].x )
            iXMin = (int)m_akVertex[i].x;

        if ( iXMax < (int)m_akVertex[i].x )
            iXMax = (int)m_akVertex[i].x;

        if ( iYMin > (int)m_akVertex[i].y )
            iYMin = (int)m_akVertex[i].y;

        if ( iYMax < (int)m_akVertex[i].y )
            iYMax = (int)m_akVertex[i].y;
    }

    // draw perspective mapping of image (...inefficient drawing...)
    memset(m_aucTrgBits,0xFF,3*GetWidth()*GetWidth());
    for (int iTrgY = iYMin; iTrgY <= iYMax; iTrgY++)
    {
        Vector2 kQuad;
        kQuad.y = (Real)iTrgY;

        int iXStart = iXMin;
        while ( iXStart <= iXMax )
        {
            kQuad.x = (Real)iXStart;
            if ( PointInConvex4(m_akVertex,kQuad) )
                break;
            iXStart++;
        }

        int iXFinal = iXMax;
        while ( iXFinal >= iXMin )
        {
            kQuad.x = (Real)iXFinal;
            if ( PointInConvex4(m_akVertex,kQuad) )
                break;
            iXFinal--;
        }

        for (int iTrgX = iXStart; iTrgX <= iXFinal; iTrgX++)
        {
            // transform point to unit square
            kQuad.x = (Real)iTrgX;
            Vector2 kSquare = m_pkMap->Transform(kQuad);

            // convert to bitmap coordinates (using clamping)
            int iSrcX = (int)((m_iXSize-1)*kSquare.x);
            if ( iSrcX < 0 )
                iSrcX = 0;
            else if ( iSrcX >= m_iXSize )
                iSrcX = m_iXSize-1;

            int iSrcY = (int)((m_iYSize-1)*kSquare.y);
            if ( iSrcY < 0 )
                iSrcY = 0;
            else if ( iSrcY >= m_iYSize )
                iSrcY = m_iYSize-1;

            int iSrcIndex = 3*(iSrcX+m_iXSize*iSrcY);
            unsigned char* aucSrcRGB = m_aucSrcBits + iSrcIndex;
            int iTrgIndex = 3*(iTrgX + GetWidth()*iTrgY);
            unsigned char* aucTrgRGB = m_aucTrgBits + iTrgIndex;
            for (int i = 0; i < 3; i++)
                aucTrgRGB[i] = aucSrcRGB[i];
        }
    }

    InvalidateRect(GetWindowHandle(),NULL,FALSE);
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnInitialize ()
{
    // source bitmap
    m_hSrcBitmap = (HBITMAP) LoadImage(NULL,"texture.bmp",IMAGE_BITMAP,0,0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    assert( m_hSrcBitmap );

    DIBSECTION dibSection;
    GetObject(m_hSrcBitmap,sizeof(DIBSECTION),&dibSection);
    assert( dibSection.dsBm.bmBitsPixel == 24
        &&  dibSection.dsBm.bmWidth < GetWidth()
        &&  dibSection.dsBm.bmHeight < GetHeight() );

    m_iXSize = dibSection.dsBm.bmWidth;
    m_iYSize = dibSection.dsBm.bmHeight;
    m_aucSrcBits = (unsigned char*) dibSection.dsBm.bmBits;

    // target bitmap
    m_hTrgWindowDC = GetDC(GetWindowHandle());
    m_hTrgMemoryDC = CreateCompatibleDC(m_hTrgWindowDC);

    char* acBMI = new char[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD*)];
    BITMAPINFOHEADER& rkBMIH = *(BITMAPINFOHEADER*)acBMI;
    rkBMIH.biSize = sizeof(BITMAPINFOHEADER);
    rkBMIH.biWidth = GetWidth();
    rkBMIH.biHeight = -GetWidth();
    rkBMIH.biPlanes = 1;
    rkBMIH.biBitCount = 24;
    rkBMIH.biCompression = BI_RGB;
    rkBMIH.biSizeImage = GetWidth()*GetWidth();
    rkBMIH.biXPelsPerMeter = 0;
    rkBMIH.biYPelsPerMeter = 0;
    rkBMIH.biClrUsed = 0;
    rkBMIH.biClrImportant = 0;

    m_hTrgBitmap = CreateDIBSection(m_hTrgMemoryDC,(CONST BITMAPINFO*)acBMI,
        DIB_RGB_COLORS,(void**)&m_aucTrgBits,NULL,0);

    delete[] acBMI;

    SelectObject(m_hTrgMemoryDC,m_hTrgBitmap);

    // The quadrilateral to which the image is perspectively mapped.  The
    // default is the original image rectangle (the initial mapping is the
    // identity).
    m_akVertex = new Vector2[4];
    m_akVertex[0] = Vector2(0.0f,0.0f);
    m_akVertex[1] = Vector2(m_iXSize-1.0f,0.0f);
    m_akVertex[2] = Vector2(m_iXSize-1.0f,m_iYSize-1.0f);
    m_akVertex[3] = Vector2(0.0f,m_iYSize-1.0f);

    Vector2 kOffset(0.5f*(GetWidth()-m_iXSize),0.5f*(GetHeight()-m_iYSize));
    for (int i = 0; i < 4; i++)
        m_akVertex[i] += kOffset;

    CreateMapping();
    return true;
}
//----------------------------------------------------------------------------
void MapTextureToQuad::OnTerminate ()
{
    DeleteObject(m_hTrgBitmap);
    DeleteDC(m_hTrgMemoryDC);
    ReleaseDC(GetWindowHandle(),m_hTrgWindowDC);

    DeleteObject(m_hSrcBitmap);
    delete m_pkMap;
    delete[] m_akVertex;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnPaint (HDC hDC)
{
    if ( m_aucSrcBits )
    {
        BitBlt(m_hTrgWindowDC,0,0,GetWidth(),GetWidth(),m_hTrgMemoryDC,0,0,
            SRCCOPY);
    }

    return true;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnChar (char cCharCode, long)
{
    switch ( cCharCode )
    {
    case 'q':
    case 'Q':
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(),WM_DESTROY,0,0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnLButtonDown (int iXPos, int iYPos, unsigned int)
{
    if ( !m_bButtonDown )
    {
        m_bButtonDown = true;
        SetCapture(GetWindowHandle());
        m_hDragDC = GetDC(GetWindowHandle());

        SelectVertex(Vector2((Real)iXPos,(Real)iYPos));
    }

    return true;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnLButtonUp (int iXPos, int iYPos, unsigned int)
{
    if ( m_bButtonDown )
    {
        m_bButtonDown = false;
        ReleaseCapture();
        ReleaseDC(GetWindowHandle(),m_hDragDC);

        if ( m_iSelected >= 0
        &&   0 <= iXPos && iXPos < GetWidth()
        &&   0 <= iYPos && iYPos < GetHeight() )
        {
            UpdateQuadrilateral(Vector2((Real)iXPos,(Real)iYPos));
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MapTextureToQuad::OnMouseMove (int iXPos, int iYPos, unsigned int)
{
    if ( m_bButtonDown )
    {
        if ( m_iSelected >= 0
        &&   0 <= iXPos && iXPos < GetWidth()
        &&   0 <= iYPos && iYPos < GetHeight() )
        {
            UpdateQuadrilateral(Vector2((Real)iXPos,(Real)iYPos));
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void MapTextureToQuad::SelectVertex (const Vector2& rkPos)
{
    // identify vertex within 5 pixels of mouse click
    const Real fPixels = 5.0f;
    m_iSelected = -1;
    for (int i = 0; i < 4; i++)
    {
        Vector2 kDiff = rkPos - m_akVertex[i];
        if ( kDiff.Length() <= fPixels )
        {
            m_iSelected = i;
            break;
        }
    }
}
//----------------------------------------------------------------------------
void MapTextureToQuad::UpdateQuadrilateral (const Vector2& rkPos)
{
    // quadrilateral must remain convex
    int iPrev = (m_iSelected > 0 ? m_iSelected - 1 : 3);
    int iNext = (m_iSelected < 3 ? m_iSelected + 1 : 0);
    Vector2 kDiff1 = rkPos - m_akVertex[iPrev];
    Vector2 kDiff2 = m_akVertex[iNext] - rkPos;
    Real fDet = kDiff1.x*kDiff2.y - kDiff1.y*kDiff2.x;
    if ( fDet > 0.0f )
    {
        m_akVertex[m_iSelected] = rkPos;
        CreateMapping();
    }
}
//----------------------------------------------------------------------------
