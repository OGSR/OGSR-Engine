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

#include "MgcImages.h"
#include "ImageTessellator.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void SetPixel (ImageRGB82D& rkImage, int iX, int iY)
{
    if ( 0 <= iX && iX < rkImage.GetBound(0)
    &&   0 <= iY && iY < rkImage.GetBound(1) )
    {
        rkImage(iX,iY) = GetColor24(255,0,0);
    }
}
//----------------------------------------------------------------------------
static void Line2D (ImageRGB82D& rkImage, int iX0, int iY0, int iX1, int iY1)
{
    // starting point of line
    int iX = iX0, iY = iY0;

    // direction of line
    int iDx = iX1-iX0, iDy = iY1-iY0;

    // increment or decrement depending on direction of line
    int iSx = (iDx > 0 ? 1 : (iDx < 0 ? -1 : 0));
    int iSy = (iDy > 0 ? 1 : (iDy < 0 ? -1 : 0));

    // decision parameters for voxel selection
    if ( iDx < 0 ) iDx = -iDx;
    if ( iDy < 0 ) iDy = -iDy;
    int iAx = 2*iDx, iAy = 2*iDy;
    int iDecX, iDecY;

    // determine largest direction component, single-step related variable
    int iMax = iDx, iVar = 0;
    if ( iDy > iMax ) { iVar = 1; }

    // traverse Bresenham line
    switch ( iVar )
    {
    case 0:  // single-step in iX-direction
        for (iDecY = iAy-iDx; /**/; iX += iSx, iDecY += iAy)
        {
            // process pixel
            SetPixel(rkImage,iX,iY);

            // take Bresenham step
            if ( iX == iX1 ) break;
            if ( iDecY >= 0 ) { iDecY -= iAx; iY += iSy; }
        }
        break;
    case 1:  // single-step in iY-direction
        for (iDecX = iAx-iDy; /**/; iY += iSy, iDecX += iAx)
        {
            // process pixel
            SetPixel(rkImage,iX,iY);

            // take Bresenham step
            if ( iY == iY1 ) break;
            if ( iDecX >= 0 ) { iDecX -= iAy; iX += iSx; }
        }
        break;
    }
}
//----------------------------------------------------------------------------
int main ()
{
    // read in 256x256 image
    ImageInt2D kHead("head.im");
    int i, iMaxHead = kHead[0];
    for (i = 1; i < kHead.GetQuantity(); i++)
    {
        if ( kHead[i] > iMaxHead )
            iMaxHead = kHead[i];
    }

    // embed in a 257x257 image
    const int iSize = 257;
    ImageInt2D kImage(iSize,iSize);
    int iX, iY;
    for (iY = 0; iY < 256; iY++)
    {
        for (iX = 0; iX < 256; iX++)
            kImage(iX,iY) = kHead(iX,iY);
    }

    // tessellate image to desired resolution
    ImageTessellator kTessellator(iSize,(const int*)kImage.GetData());
    kTessellator.Tessellate(256);

    // draw as gray-scale image with triangles drawn in red
    const int iMult = 2;
    ImageRGB82D kColor(iMult*256,iMult*256);
    for (iY = 0; iY < kColor.GetBound(1); iY++)
    {
        for (iX = 0; iX < kColor.GetBound(0); iX++)
        {
            unsigned char ucValue = (255*kHead(iX/iMult,iY/iMult))/iMaxHead;
            kColor(iX,iY) = GetColor24(ucValue,ucValue,ucValue);
        }
    }

    int iNumTriangles = kTessellator.GetNumTriangles();
    const int* aiIndex = kTessellator.GetIndices();
    for (i = 0; i < 3*iNumTriangles; /**/)
    {
        int iV0 = aiIndex[i++];
        int iV1 = aiIndex[i++];
        int iV2 = aiIndex[i++];

        int iX0 = iV0 % iSize;
        int iY0 = iV0 / iSize;
        int iX1 = iV1 % iSize;
        int iY1 = iV1 / iSize;
        int iX2 = iV2 % iSize;
        int iY2 = iV2 / iSize;

        Line2D(kColor,iMult*iX0,iMult*iY0,iMult*iX1,iMult*iY1);
        Line2D(kColor,iMult*iX1,iMult*iY1,iMult*iX2,iMult*iY2);
        Line2D(kColor,iMult*iX2,iMult*iY2,iMult*iX0,iMult*iY0);
    }

    kColor.Save("ImageTessellator.im");

    return 0;
}
//----------------------------------------------------------------------------


