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

#include "MgcCont2DMinBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Box2 Mgc::MinBox (int iQuantity, const Vector2* akPoint)
{
    // The input points are V[0] through V[N-1] and are assumed to be the
    // vertices of a convex polygon that are counterclockwise ordered.  The
    // input points must not contain three consecutive collinear points.

    // Unit-length edge directions of convex polygon.  These could be
    // precomputed and passed to this routine if the application requires it.
    int iQuantityM1 = iQuantity -1;
    Vector2* akEdge = new Vector2[iQuantity];
    bool* abVisited = new bool[iQuantity];
    int i;
    for (i = 0; i < iQuantityM1; i++)
    {
        akEdge[i] = akPoint[i+1] - akPoint[i];
        akEdge[i].Unitize();
        abVisited[i] = false;
    }
    akEdge[iQuantityM1] = akPoint[0] - akPoint[iQuantityM1];
    akEdge[iQuantityM1].Unitize();
    abVisited[iQuantityM1] = false;

    // Find the smallest axis-aligned box containing the points.  Keep track
    // of the extremum indices, L (left), R (right), B (bottom), and T (top)
    // so that the following constraints are met:
    //   V[L].x <= V[i].x for all i and V[(L+1)%N].x > V[L].x
    //   V[R].x >= V[i].x for all i and V[(R+1)%N].x < V[R].x
    //   V[B].y <= V[i].y for all i and V[(B+1)%N].y > V[B].y
    //   V[T].y >= V[i].y for all i and V[(T+1)%N].y < V[R].y
    Real fXMin = akPoint[0].x, fXMax = fXMin;
    Real fYMin = akPoint[0].y, fYMax = fYMin;
    int iLIndex = 0, iRIndex = 0, iBIndex = 0, iTIndex = 0;
    for (i = 1; i < iQuantity; i++)
    {
        if ( akPoint[i].x <= fXMin )
        {
            fXMin = akPoint[i].x;
            iLIndex = i;
        }
        else if ( akPoint[i].x >= fXMax )
        {
            fXMax = akPoint[i].x;
            iRIndex = i;
        }

        if ( akPoint[i].y <= fYMin )
        {
            fYMin = akPoint[i].y;
            iBIndex = i;
        }
        else if ( akPoint[i].y >= fYMax )
        {
            fYMax = akPoint[i].y;
            iTIndex = i;
        }
    }

    // wrap-around tests to ensure the constraints mentioned above
    if ( akPoint[0].x <= fXMin )
    {
        fXMin = akPoint[0].x;
        iLIndex = 0;
    }
    else if ( akPoint[0].x >= fXMax )
    {
        fXMax = akPoint[0].x;
        iRIndex = 0;
    }

    if ( akPoint[0].y <= fYMin )
    {
        fYMin = akPoint[0].y;
        iBIndex = 0;
    }
    else if ( akPoint[0].y >= fYMax )
    {
        fYMax = akPoint[0].y;
        iTIndex = 0;
    }

    // dimensions of axis-aligned box (extents store width and height for now)
    Box2 kBox;
    kBox.Center().x = 0.5f*(fXMin + fXMax);
    kBox.Center().y = 0.5f*(fYMin + fYMax);
    kBox.Axis(0) = Vector2::UNIT_X;
    kBox.Axis(1) = Vector2::UNIT_Y;
    kBox.Extent(0) = 0.5f*(fXMax - fXMin);
    kBox.Extent(1) = 0.5f*(fYMax - fYMin);
    Real fMinAreaDiv4 = kBox.Extent(0)*kBox.Extent(1);

    // rotating calipers algorithm
    enum { F_NONE, F_LEFT, F_RIGHT, F_BOTTOM, F_TOP };
    Vector2 kU = Vector2::UNIT_X, kV = Vector2::UNIT_Y;

    bool bDone = false;
    while ( !bDone )
    {
        // determine edge that forms smallest angle with current box edges
        int iFlag = F_NONE;
        Real fMaxDot = 0.0f;

        Real fDot = kU.Dot(akEdge[iBIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_BOTTOM;
        }

        fDot = kV.Dot(akEdge[iRIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_RIGHT;
        }

        fDot = -kU.Dot(akEdge[iTIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_TOP;
        }

        fDot = -kV.Dot(akEdge[iLIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_LEFT;
        }

        switch ( iFlag )
        {
        case F_BOTTOM:
            if ( abVisited[iBIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute box axes with E[B] as an edge
                kU = akEdge[iBIndex];
                kV = -kU.Cross();

                // mark edge visited and rotate the calipers
                abVisited[iBIndex] = true;
                if ( ++iBIndex == iQuantity )
                    iBIndex = 0;
            }
            break;
        case F_RIGHT:
            if ( abVisited[iRIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute dimensions of box with E[R] as an edge
                kV = akEdge[iRIndex];
                kU = kV.Cross();

                // mark edge visited and rotate the calipers
                abVisited[iRIndex] = true;
                if ( ++iRIndex == iQuantity )
                    iRIndex = 0;
            }
            break;
        case F_TOP:
            if ( abVisited[iTIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute dimensions of box with E[T] as an edge
                kU = -akEdge[iTIndex];
                kV = -kU.Cross();

                // mark edge visited and rotate the calipers
                abVisited[iTIndex] = true;
                if ( ++iTIndex == iQuantity )
                    iTIndex = 0;
            }
            break;
        case F_LEFT:
            if ( abVisited[iLIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute dimensions of box with E[L] as an edge
                kV = -akEdge[iLIndex];
                kU = kV.Cross();

                // mark edge visited and rotate the calipers
                abVisited[iLIndex] = true;
                if ( ++iLIndex == iQuantity )
                    iLIndex = 0;
            }
            break;
        case F_NONE:
            // polygon is a rectangle
            bDone = true;
            break;
        }

        Real fExtent0 = 0.5f*(kU.Dot(akPoint[iRIndex]-akPoint[iLIndex]));
        Real fExtent1 = 0.5f*(kV.Dot(akPoint[iTIndex]-akPoint[iBIndex]));
        Real fAreaDiv4 = fExtent0*fExtent1;
        if ( fAreaDiv4 < fMinAreaDiv4 )
        {
            fMinAreaDiv4 = fAreaDiv4;
            kBox.Axis(0) = kU;
            kBox.Axis(1) = kV;
            kBox.Extent(0) = fExtent0;
            kBox.Extent(1) = fExtent1;

            // compute box center
            Vector2 kTmp = 0.5f*(akPoint[iTIndex] + akPoint[iBIndex]) -
                akPoint[iLIndex];
            kBox.Center() = akPoint[iLIndex] + fExtent0*kBox.Axis(0) +
                (kBox.Axis(1).Dot(kTmp))*kBox.Axis(1);
        }
    }

    delete[] abVisited;
    delete[] akEdge;
    return kBox;
}
//----------------------------------------------------------------------------
Box2 Mgc::MinBoxOrderNSqr (int iQuantity, const Vector2* akPoint)
{
    Real fMinAreaDiv4 = Math::MAX_REAL;
    Box2 kBox;

    for (int i1 = 0, i0 = iQuantity-1; i1 < iQuantity; i0 = i1, i1++)
    {
        Vector2 kU0 = akPoint[i1] - akPoint[i0];
        kU0.Unitize();
        Vector2 kU1 = -kU0.Cross();
        Real fS0 = 0.0f, fT0 = 0.0f, fS1 = 0.0f, fT1 = 0.0f;
        for (int j = 1; j < iQuantity; j++)
        {
            Vector2 kDiff = akPoint[j] - akPoint[0];
            Real fTest = kU0.Dot(kDiff);
            if ( fTest < fS0 )
                fS0 = fTest;
            else if ( fTest > fS1 )
                fS1 = fTest;

            fTest = kU1.Dot(kDiff);
            if ( fTest < fT0 )
                fT0 = fTest;
            else if ( fTest > fT1 )
                fT1 = fTest;
        }

        Real fExtent0 = 0.5f*(fS1 - fS0);
        Real fExtent1 = 0.5f*(fT1 - fT0);
        Real fAreaDiv4 = fExtent0*fExtent1;
        if ( fAreaDiv4 < fMinAreaDiv4 )
        {
            fMinAreaDiv4 = fAreaDiv4;
            kBox.Axis(0) = kU0;
            kBox.Axis(1) = kU1;
            kBox.Extent(0) = fExtent0;
            kBox.Extent(1) = fExtent1;
            kBox.Center() = akPoint[0]+0.5f*(fS0+fS1)*kU0+0.5f*(fT0+fT1)*kU1;
        }
    }

    return kBox;
}
//----------------------------------------------------------------------------


