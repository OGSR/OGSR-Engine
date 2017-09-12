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

#include "MgcCont2DBox.h"
#include "MgcAppr2DGaussPointsFit.h"
using namespace Mgc;

//----------------------------------------------------------------------------
void Mgc::ContAlignedBox (int iQuantity, const Vector2* akPoint,
    Vector2& rkMin, Vector2& rkMax)
{
    rkMin = akPoint[0];
    rkMax = rkMin;

    for (int i = 1; i < iQuantity; i++)
    {
        if ( akPoint[i].x < rkMin.x )
            rkMin.x = akPoint[i].x;
        else if ( akPoint[i].x > rkMax.x )
            rkMax.x = akPoint[i].x;

        if ( akPoint[i].y < rkMin.y )
            rkMin.y = akPoint[i].y;
        else if ( akPoint[i].y > rkMax.y )
            rkMax.y = akPoint[i].y;
    }
}
//----------------------------------------------------------------------------
Box2 Mgc::ContOrientedBox (int iQuantity, const Vector2* akPoint)
{
    Box2 kBox;

    GaussPointsFit(iQuantity,akPoint,kBox.Center(),kBox.Axes(),
        kBox.Extents());

    // Let C be the box center and let U0 and U1 be the box axes.  Each input
    // point is of the form X = C + y0*U0 + y1*U1.  The following code
    // computes min(y0), max(y0), min(y1), and max(y1).  The box center is
    // then adjusted to be
    //   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1

    Vector2 kDiff = akPoint[0] - kBox.Center();
    Real fY0Min = kDiff.Dot(kBox.Axis(0)), fY0Max = fY0Min;
    Real fY1Min = kDiff.Dot(kBox.Axis(1)), fY1Max = fY1Min;

    for (int i = 1; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kBox.Center();

        Real fY0 = kDiff.Dot(kBox.Axis(0));
        if ( fY0 < fY0Min )
            fY0Min = fY0;
        else if ( fY0 > fY0Max )
            fY0Max = fY0;

        Real fY1 = kDiff.Dot(kBox.Axis(1));
        if ( fY1 < fY1Min )
            fY1Min = fY1;
        else if ( fY1 > fY1Max )
            fY1Max = fY1;
    }

    kBox.Center() += (0.5f*(fY0Min+fY0Max))*kBox.Axis(0) +
        (0.5f*(fY1Min+fY1Max))*kBox.Axis(1);

    kBox.Extent(0) = 0.5f*(fY0Max - fY0Min);
    kBox.Extent(1) = 0.5f*(fY1Max - fY1Min);

    return kBox;
}
//----------------------------------------------------------------------------
bool Mgc::ContOrientedBox (int iQuantity, const Vector2* akPoint,
    const bool* abValid, Box2& rkBox)
{
    if ( !GaussPointsFit(iQuantity,akPoint,abValid,rkBox.Center(),
         rkBox.Axes(),rkBox.Extents()) )
    {
        return false;
    }

    // Let C be the box center and let U0 and U1 be the box axes.  Each input
    // point is of the form X = C + y0*U0 + y1*U1.  The following code
    // computes min(y0), max(y0), min(y1), and max(y1).  The box center is
    // then adjusted to be
    //   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1

    // get first valid vertex
    Vector2 kDiff;
    Real fY0Min, fY0Max, fY1Min, fY1Max;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kDiff = akPoint[i] - rkBox.Center();
            fY0Min = kDiff.Dot(rkBox.Axis(0));
            fY0Max = fY0Min;
            fY1Min = kDiff.Dot(rkBox.Axis(1));
            fY1Max = fY1Min;
            break;
        }
    }

    for (i++; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kDiff = akPoint[i] - rkBox.Center();

            Real fY0 = kDiff.Dot(rkBox.Axis(0));
            if ( fY0 < fY0Min )
                fY0Min = fY0;
            else if ( fY0 > fY0Max )
                fY0Max = fY0;

            Real fY1 = kDiff.Dot(rkBox.Axis(1));
            if ( fY1 < fY1Min )
                fY1Min = fY1;
            else if ( fY1 > fY1Max )
                fY1Max = fY1;
        }
    }

    rkBox.Center() += (0.5f*(fY0Min+fY0Max))*rkBox.Axis(0)
        + (0.5f*(fY1Min+fY1Max))*rkBox.Axis(1);

    rkBox.Extent(0) = 0.5f*(fY0Max - fY0Min);
    rkBox.Extent(1) = 0.5f*(fY1Max - fY1Min);

    return true;
}
//----------------------------------------------------------------------------


