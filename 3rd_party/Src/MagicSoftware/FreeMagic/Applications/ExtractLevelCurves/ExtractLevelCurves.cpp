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

#include "ImageInterp2D.h"
using namespace Mgc;

// comment this out for LINEAR
#define USE_BILINEAR

// color subimage of 'head.im'
const int g_iSize = 32, g_iMagnify = 8;
const int g_iXPos = 100, g_iYPos = 100;
const float g_fMultiply = 1.0f/(float)g_iMagnify;
ImageRGB82D g_kColor(g_iMagnify*g_iSize,g_iMagnify*g_iSize);

//----------------------------------------------------------------------------
static void DrawPixel (unsigned int uiColor, int iX, int iY)
{
    if ( 0 <= iX && iX < g_kColor.GetBound(0)
    &&   0 <= iY && iY < g_kColor.GetBound(1) )
    {
        if ( iX == 224 && iY == 78 )
        {
            int i = 0;
        }
        g_kColor(iX,iY) = uiColor;
    }
}
//----------------------------------------------------------------------------
static void DrawLine (unsigned int uiColor, int x0, int y0, int x1, int y1)
{
    // starting point of line
    int x = x0, y = y0;

    // direction of line
    int dx = x1-x0, dy = y1-y0;

    // increment or decrement depending on direction of line
    int sx = (dx > 0 ? 1 : (dx < 0 ? -1 : 0));
    int sy = (dy > 0 ? 1 : (dy < 0 ? -1 : 0));

    // decision parameters for voxel selection
    if ( dx < 0 ) dx = -dx;
    if ( dy < 0 ) dy = -dy;
    int ax = 2*dx, ay = 2*dy;
    int decx, decy;

    // determine largest direction component, single-step related variable
    int max = dx, var = 0;
    if ( dy > max ) { var = 1; }

    // traverse Bresenham line
    switch ( var )
    {
    case 0:  // single-step in x-direction
        for (decy=ay-dx; /**/; x += sx, decy += ay)
        {
            // process pixel
            DrawPixel(uiColor,x,y);

            // take Bresenham step
            if ( x == x1 ) break;
            if ( decy >= 0 ) { decy -= ax; y += sy; }
        }
        break;
    case 1:  // single-step in y-direction
        for (decx=ax-dy; /**/; y += sy, decx += ax)
        {
            // process pixel
            DrawPixel(uiColor,x,y);

            // take Bresenham step
            if ( y == y1 ) break;
            if ( decx >= 0 ) { decx -= ay; x += sx; }
        }
        break;
    }
}
//----------------------------------------------------------------------------
int main ()
{
    // load image for level set extraction
    ImageInterp2D kImage("head.im");

    // get the extremes
    int iMin = kImage[0], iMax = iMin;
    for (int i = 1; i < kImage.GetQuantity(); i++)
    {
        int iValue = kImage[i];
        if ( iValue < iMin )
            iMin = iValue;
        else if ( iValue > iMax )
            iMax = iValue;
    }
    int iRange = iMax - iMin;

    // Generate a color subimage to superimpose level sets.  Process the
    // subimage with upper left corner (100,100) and of size 32x32.
    for (int iY = 0; iY < g_iSize; iY++)
    {
        for (int iX = 0; iX < g_iSize; iX++)
        {
            for (int iDY = 0; iDY < g_iMagnify; iDY++)
            {
                float fY = g_iYPos + iY + g_fMultiply*iDY;
                for (int iDX = 0; iDX < g_iMagnify; iDX++)
                {
                    float fX = g_iXPos + iX + g_fMultiply*iDX;
#ifdef USE_BILINEAR
                    float fInterp = kImage.BilinearInterpolate(fX,fY);
#else
                    float fInterp = kImage.LinearInterpolate(fX,fY);
#endif
                    fInterp = (fInterp - iMin)/(float)iRange;
                    unsigned char ucGrey = (unsigned char)(255.0f*fInterp);
                    g_kColor(g_iMagnify*iX+iDX,g_iMagnify*iY+iDY) =
                        GetColor24(ucGrey,ucGrey,ucGrey);
                }
            }
        }
    }

    // extract the level set
    int iVertexQuantity = 0;
    ImageInterp2D::Vertex* akVertex = NULL;
    int iEdgeQuantity = 0;
    ImageInterp2D::Edge* akEdge = NULL;
#ifdef USE_BILINEAR
    kImage.ExtractLevelSetBilinear(512,iVertexQuantity,akVertex,
        iEdgeQuantity,akEdge);
#else
    kImage.ExtractLevelSetLinear(512,iVertexQuantity,akVertex,
        iEdgeQuantity,akEdge);
#endif

    kImage.MakeUnique(iVertexQuantity,akVertex,iEdgeQuantity,akEdge);

    // draw the edges in the subimage
    float fX, fY;
    for (int iE = 0; iE < iEdgeQuantity; iE++)
    {
        const ImageInterp2D::Edge& rkEdge = akEdge[iE];

        const ImageInterp2D::Vertex& rkV0 = akVertex[rkEdge.m_i0];
        rkV0.GetPair(fX,fY);
        int iX0 = (int)((fX - g_iXPos)*g_iMagnify);
        int iY0 = (int)((fY - g_iYPos)*g_iMagnify);

        const ImageInterp2D::Vertex& rkV1 = akVertex[rkEdge.m_i1];
        rkV1.GetPair(fX,fY);
        int iX1 = (int)((fX - g_iXPos)*g_iMagnify);
        int iY1 = (int)((fY - g_iYPos)*g_iMagnify);

        DrawLine(GetColor24(0,255,0),iX0,iY0,iX1,iY1);
    }

    // draw the vertices in the subimage
    for (int iV = 0; iV < iVertexQuantity; iV++)
    {
        akVertex[iV].GetPair(fX,fY);
        int iX = (int)((fX - g_iXPos)*g_iMagnify);
        int iY = (int)((fY - g_iYPos)*g_iMagnify);

        DrawPixel(GetColor24(255,0,0),iX,iY);
    }

#ifdef USE_BILINEAR
    g_kColor.Save("BilinearZoom.im");
#else
    g_kColor.Save("LinearZoom.im");
#endif

    delete[] akVertex;
    delete[] akEdge;

    return 0;
}
//----------------------------------------------------------------------------


