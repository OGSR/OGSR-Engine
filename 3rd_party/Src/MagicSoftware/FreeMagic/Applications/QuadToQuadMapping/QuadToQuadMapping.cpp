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

#include "MgcQuadToQuadTransforms.h"
#include "MgcImages.h"
using namespace Mgc;

const unsigned char OFF = 255;
const unsigned char ON = 0;

//----------------------------------------------------------------------------
void DoHmSqrToQuad (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Mgc::Vector2& rkP01)
{
    HmSqrToQuad kMap(rkP00,rkP10,rkP11,rkP01);

    float fXMin, fXMax, fXRange, fYMin, fYMax, fYRange;
    fXMax = fXMin = rkP00.x;
    fYMax = fYMin = rkP00.y;

    if ( rkP10.x < fXMin ) fXMin = rkP10.x;
    if ( rkP10.x > fXMax ) fXMax = rkP10.x;
    if ( rkP10.y < fYMin ) fYMin = rkP10.y;
    if ( rkP10.y > fYMax ) fYMax = rkP10.y;

    if ( rkP11.x < fXMin ) fXMin = rkP11.x;
    if ( rkP11.x > fXMax ) fXMax = rkP11.x;
    if ( rkP11.y < fYMin ) fYMin = rkP11.y;
    if ( rkP11.y > fYMax ) fYMax = rkP11.y;

    if ( rkP01.x < fXMin ) fXMin = rkP01.x;
    if ( rkP01.x > fXMax ) fXMax = rkP01.x;
    if ( rkP01.y < fYMin ) fYMin = rkP01.y;
    if ( rkP01.y > fYMax ) fYMax = rkP01.y;

    fXRange = fXMax-fXMin;
    fYRange = fYMax-fYMin;

    int iXBase, iYBase;
    if ( fXRange <= fYRange )
    {
        iYBase = 256;
        iXBase = (int)(256.0f*fXRange/fYRange);
        if ( iXBase % 2 )
            iXBase++;
    }
    else
    {
        iXBase = 256;
        iYBase = (int)(256.0f*fYRange/fXRange);
        if ( iYBase % 2 )
            iYBase++;
    }

    ImageUChar2D kQuad(iXBase,iYBase);
    kQuad = OFF;

    int iX, iY, iXMap, iYMap;
    Vector2 kInput, kOutput;

    // transform columns of square
    for (iX = 0; iX < 256; iX += 16)
    {
        kInput.x = iX/255.0f;
        for (iY = 0; iY < 256; iY++)
        {
            kInput.y = iY/255.0f;
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
            iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
            kQuad(iXMap,iYMap) = ON;
        }
    }

    // transform last column
    kInput.x = 1.0f;
    for (iY = 0; iY < 256; iY++)
    {
        kInput.y = iY/255.0f;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;
    }

    // transform rows of square
    for (iY = 0; iY < 256; iY += 16)
    {
        kInput.y = iY/255.0f;
        for (iX = 0; iX < 256; iX++)
        {
            kInput.x = iX/255.0f;
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
            iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
            kQuad(iXMap,iYMap) = ON;
        }
    }

    // transform last row
    kInput.y = 1.0f;
    for (iX = 0; iX < 256; iX++)
    {
        kInput.x = iX/255.0f;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;
    }

    // transform diagonals of square
    for (iX = 0; iX < 256; iX++)
    {
        kInput.x = iX/255.0f;
        kInput.y = kInput.x;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;

        kInput.y = 1.0f-kInput.x;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;
    }

    kQuad.Save("HmSqrToQuad.im");
}
//----------------------------------------------------------------------------
void DoBiSqrToQuad (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    BiSqrToQuad kMap(rkP00,rkP10,rkP11,rkP01);

    float fXMin, fXMax, fXRange, fYMin, fYMax, fYRange;
    fXMax = fXMin = rkP00.x;
    fYMax = fYMin = rkP00.y;

    if ( rkP10.x < fXMin ) fXMin = rkP10.x;
    if ( rkP10.x > fXMax ) fXMax = rkP10.x;
    if ( rkP10.y < fYMin ) fYMin = rkP10.y;
    if ( rkP10.y > fYMax ) fYMax = rkP10.y;

    if ( rkP11.x < fXMin ) fXMin = rkP11.x;
    if ( rkP11.x > fXMax ) fXMax = rkP11.x;
    if ( rkP11.y < fYMin ) fYMin = rkP11.y;
    if ( rkP11.y > fYMax ) fYMax = rkP11.y;

    if ( rkP01.x < fXMin ) fXMin = rkP01.x;
    if ( rkP01.x > fXMax ) fXMax = rkP01.x;
    if ( rkP01.y < fYMin ) fYMin = rkP01.y;
    if ( rkP01.y > fYMax ) fYMax = rkP01.y;

    fXRange = fXMax-fXMin;
    fYRange = fYMax-fYMin;

    int iXBase, iYBase;
    if ( fXRange <= fYRange )
    {
        iYBase = 256;
        iXBase = (int)(256.0f*fXRange/fYRange);
        if ( iXBase % 2 )
            iXBase++;
    }
    else
    {
        iXBase = 256;
        iYBase = (int)(256.0f*fYRange/fXRange);
        if ( iYBase % 2 )
            iYBase++;
    }

    ImageUChar2D kQuad(iXBase,iYBase);
    kQuad = OFF;

    int iX, iY, iXMap, iYMap;
    Vector2 kInput, kOutput;

    // transform columns of square
    for (iX = 0; iX < 256; iX += 16)
    {
        kInput.x = iX/255.0f;
        for (iY = 0; iY < 256; iY++)
        {
            kInput.y = iY/255.0f;
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
            iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
            kQuad(iXMap,iYMap) = ON;
        }
    }

    // transform last column
    kInput.x = 1.0f;
    for (iY = 0; iY < 256; iY++)
    {
        kInput.y = iY/255.0f;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;
    }

    // transform rows of square
    for (iY = 0; iY < 256; iY += 16)
    {
        kInput.y = iY/255.0f;
        for (iX = 0; iX < 256; iX++)
        {
            kInput.x = iX/255.0f;
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
            iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
            kQuad(iXMap,iYMap) = ON;
        }
    }

    // transform last row
    kInput.y = 1.0f;
    for (iX = 0; iX < 256; iX++)
    {
        kInput.x = iX/255.0f;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;
    }

    // transform diagonals of square
    for (iX = 0; iX < 256; iX++)
    {
        kInput.x = iX/255.0f;
        kInput.y = kInput.x;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;

        kInput.y = 1.0f-kInput.x;
        kOutput = kMap.Transform(kInput);
        iXMap = int((iXBase-1)*(kOutput.x-fXMin)/fXRange);
        iYMap = int((iYBase-1)*(kOutput.y-fYMin)/fYRange);
        kQuad(iXMap,iYMap) = ON;
    }

    kQuad.Save("BiSqrToQuad.im");
}
//----------------------------------------------------------------------------
void DoHmQuadToSqr (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    HmQuadToSqr kMap(rkP00,rkP10,rkP11,rkP01);

    float fXMin, fXMax, fXRange, fYMin, fYMax, fYRange;
    fXMax = fXMin = rkP00.x;
    fYMax = fYMin = rkP00.y;

    if ( rkP10.x < fXMin ) fXMin = rkP10.x;
    if ( rkP10.x > fXMax ) fXMax = rkP10.x;
    if ( rkP10.y < fYMin ) fYMin = rkP10.y;
    if ( rkP10.y > fYMax ) fYMax = rkP10.y;

    if ( rkP11.x < fXMin ) fXMin = rkP11.x;
    if ( rkP11.x > fXMax ) fXMax = rkP11.x;
    if ( rkP11.y < fYMin ) fYMin = rkP11.y;
    if ( rkP11.y > fYMax ) fYMax = rkP11.y;

    if ( rkP01.x < fXMin ) fXMin = rkP01.x;
    if ( rkP01.x > fXMax ) fXMax = rkP01.x;
    if ( rkP01.y < fYMin ) fYMin = rkP01.y;
    if ( rkP01.y > fYMax ) fYMax = rkP01.y;

    fXRange = fXMax-fXMin;
    fYRange = fYMax-fYMin;

    int iXBase = 256, iYBase = 256;
    ImageUChar2D kSquare(iXBase,iYBase);
    kSquare = OFF;

    int iX, iY, iXMap, iYMap;
    Vector2 kInput, kOutput;

    // transform columns
    const int iYSample = 1000;
    for (iX = 0; iX < 16; iX++)
    {
        kInput.x = fXMin+fXRange*iX/15.0f;
        for (iY = 0; iY < iYSample; iY++)
        {
            kInput.y = fYMin+fYRange*iY/(iYSample-1.0f);
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*kOutput.x);
            iYMap = int((iYBase-1)*kOutput.y);
            if ( 0 <= iXMap && iXMap < iXBase
            &&   0 <= iYMap && iYMap < iYBase )
            {
                kSquare(iXMap,iYMap) = ON;
            }
        }
    }

    // transform rows
    const int iXSample = 1000;
    for (iY = 0; iY < 16; iY++)
    {
        kInput.y = fYMin+fYRange*iY/15.0f;
        for (iX = 0; iX < iXSample; iX++)
        {
            kInput.x = fXMin+fXRange*iX/(iXSample-1.0f);
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*kOutput.x);
            iYMap = int((iYBase-1)*kOutput.y);
            if ( 0 <= iXMap && iXMap < iXBase
            &&   0 <= iYMap && iYMap < iYBase )
            {
                kSquare(iXMap,iYMap) = ON;
            }
        }
    }

    kSquare.Save("HmQuadToSqr.im");
}
//----------------------------------------------------------------------------
void DoBiQuadToSqr (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    BiQuadToSqr kMap(rkP00,rkP10,rkP11,rkP01);

    float fXMin, fXMax, fXRange, fYMin, fYMax, fYRange;
    fXMax = fXMin = rkP00.x;
    fYMax = fYMin = rkP00.y;

    if ( rkP10.x < fXMin ) fXMin = rkP10.x;
    if ( rkP10.x > fXMax ) fXMax = rkP10.x;
    if ( rkP10.y < fYMin ) fYMin = rkP10.y;
    if ( rkP10.y > fYMax ) fYMax = rkP10.y;

    if ( rkP11.x < fXMin ) fXMin = rkP11.x;
    if ( rkP11.x > fXMax ) fXMax = rkP11.x;
    if ( rkP11.y < fYMin ) fYMin = rkP11.y;
    if ( rkP11.y > fYMax ) fYMax = rkP11.y;

    if ( rkP01.x < fXMin ) fXMin = rkP01.x;
    if ( rkP01.x > fXMax ) fXMax = rkP01.x;
    if ( rkP01.y < fYMin ) fYMin = rkP01.y;
    if ( rkP01.y > fYMax ) fYMax = rkP01.y;

    fXRange = fXMax-fXMin;
    fYRange = fYMax-fYMin;

    int iXBase = 256, iYBase = 256;
    ImageUChar2D kSquare(iXBase,iYBase);
    kSquare = OFF;

    int iX, iY, iXMap, iYMap;
    Vector2 kInput, kOutput;

    // transform columns
    const int iYSample = 1000;
    for (iX = 0; iX < 16; iX++)
    {
        kInput.x = fXMin+fXRange*iX/15.0f;
        for (iY = 0; iY < iYSample; iY++)
        {
            kInput.y = fYMin+fYRange*iY/(iYSample-1.0f);
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*kOutput.x);
            iYMap = int((iYBase-1)*kOutput.y);
            if ( 0 <= iXMap && iXMap < iXBase
            &&   0 <= iYMap && iYMap < iYBase )
            {
                kSquare(iXMap,iYMap) = ON;
            }
        }
    }

    // transform rows
    const int iXSample = 1000;
    for (iY = 0; iY < 16; iY++)
    {
        kInput.y = fYMin+fYRange*iY/15.0f;
        for (iX = 0; iX < iXSample; iX++)
        {
            kInput.x = fXMin+fXRange*iX/(iXSample-1.0f);
            kOutput = kMap.Transform(kInput);
            iXMap = int((iXBase-1)*kOutput.x);
            iYMap = int((iYBase-1)*kOutput.y);
            if ( 0 <= iXMap && iXMap < iXBase
            &&   0 <= iYMap && iYMap < iYBase )
            {
                kSquare(iXMap,iYMap) = ON;
            }
        }
    }

    kSquare.Save("BiQuadToSqr.im");
}
//----------------------------------------------------------------------------
int main ()
{
    Vector2 kP00(1.0f,1.0f);
    Vector2 kP10(2.0f,1.0f);
    Vector2 kP11(4.0f,3.0f);
    Vector2 kP01(1.0f,2.0f);

    DoHmSqrToQuad(kP00,kP10,kP11,kP01);
    DoBiSqrToQuad(kP00,kP10,kP11,kP01);
    DoHmQuadToSqr(kP00,kP10,kP11,kP01);
    DoBiQuadToSqr(kP00,kP10,kP11,kP01);

    return 0;
}
//----------------------------------------------------------------------------


