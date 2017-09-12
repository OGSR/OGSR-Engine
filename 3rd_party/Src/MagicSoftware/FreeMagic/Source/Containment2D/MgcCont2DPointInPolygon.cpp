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

#include "MgcCont2DPointInPolygon.h"
using namespace Mgc;

//----------------------------------------------------------------------------
// general polygons
//----------------------------------------------------------------------------
bool Mgc::PointInPolygon (int iQuantity, const Vector2* akV,
    const Vector2& rkP)
{
    bool bInside = false;
    for (int i = 0, j = iQuantity-1; i < iQuantity; j = i++)
    {
        const Vector2& rkU0 = akV[i];
        const Vector2& rkU1 = akV[j];
        Real fRHS, fLHS;

        if ( rkP.y < rkU1.y )  // U1 above ray
        {
            if ( rkU0.y <= rkP.y )  // U0 on or below ray
            {
                fLHS = (rkP.y-rkU0.y)*(rkU1.x-rkU0.x);
                fRHS = (rkP.x-rkU0.x)*(rkU1.y-rkU0.y);
                if ( fLHS > fRHS )
                    bInside = !bInside;
            }
        }
        else if ( rkP.y < rkU0.y )  // U1 on or below ray, U0 above ray
        {
            fLHS = (rkP.y-rkU0.y)*(rkU1.x-rkU0.x);
            fRHS = (rkP.x-rkU0.x)*(rkU1.y-rkU0.y);
            if ( fLHS < fRHS )
                bInside = !bInside;
        }
    }
    return bInside;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// convex polygons
//----------------------------------------------------------------------------
bool Mgc::PointInConvexOrderN (int iQuantity, const Vector2* akV,
    const Vector2& rkP)
{
    for (int i1 = 0, i0 = iQuantity-1; i1 < iQuantity; i0 = i1++)
    {
        Real fNx = akV[i1].y - akV[i0].y;
        Real fNy = akV[i0].x - akV[i1].x;
        Real fDx = rkP.x - akV[i0].x;
        Real fDy = rkP.y - akV[i0].y;
        if ( fNx*fDx + fNy*fDy > 0.0f )
            return false;
    }

    return true;
}
//----------------------------------------------------------------------------
static bool SubContainsPoint (int iQuantity, const Vector2* akV,
    const Vector2& rkP, int i0, int i1)
{
    Real fNx, fNy, fDx, fDy;

    int iDiff = i1 - i0;
    if ( iDiff == 1 || (iDiff < 0 && iDiff+iQuantity == 1) )
    {
        fNx = akV[i1].y - akV[i0].y;
        fNy = akV[i0].x - akV[i1].x;
        fDx = rkP.x - akV[i0].x;
        fDy = rkP.y - akV[i0].y;
        return fNx*fDx + fNy*fDy <= 0.0f;
    }

    // bisect the index range
    int iMid;
    if ( i0 < i1 )
    {
        iMid = (i0 + i1) >> 1;
    }
    else
    {
        iMid = ((i0 + i1 + iQuantity) >> 1);
        if ( iMid >= iQuantity )
            iMid -= iQuantity;
    }

    // determine which side of the splitting line contains the point
    fNx = akV[iMid].y - akV[i0].y;
    fNy = akV[i0].x - akV[iMid].x;
    fDx = rkP.x - akV[i0].x;
    fDy = rkP.y - akV[i0].y;
    if ( fNx*fDx + fNy*fDy > 0.0f )
    {
        // P potentially in <V(i0),V(i0+1),...,V(mid-1),V(mid)>
        return SubContainsPoint(iQuantity,akV,rkP,i0,iMid);
    }
    else
    {
        // P potentially in <V(mid),V(mid+1),...,V(i1-1),V(i1)>
        return SubContainsPoint(iQuantity,akV,rkP,iMid,i1);
    }
}
//----------------------------------------------------------------------------
bool Mgc::PointInConvexOrderLogN (int iVQuantity, const Vector2* akV,
    const Vector2& rkP)
{
    return SubContainsPoint(iVQuantity,akV,rkP,0,0);
}
//----------------------------------------------------------------------------
bool Mgc::PointInConvex4 (const Vector2* akV, const Vector2& rkP)
{
    Real fNx = akV[2].y - akV[0].y;
    Real fNy = akV[0].x - akV[2].x;
    Real fDx = rkP.x - akV[0].x;
    Real fDy = rkP.y - akV[0].y;

    if ( fNx*fDx + fNy*fDy > 0.0f )
    {
        // P potentially in <V0,V1,V2>
        fNx = akV[1].y - akV[0].y;
        fNy = akV[0].x - akV[1].x;
        if ( fNx*fDx + fNy*fDy > 0.0f )
            return false;

        fNx = akV[2].y - akV[1].y;
        fNy = akV[1].x - akV[2].x;
        fDx = rkP.x - akV[1].x;
        fDy = rkP.y - akV[1].y;
        if ( fNx*fDx + fNy*fDy > 0.0f )
            return false;
    }
    else
    {
        // P potentially in <V0,V2,V3>
        fNx = akV[0].y - akV[3].y;
        fNy = akV[3].x - akV[0].x;
        if ( fNx*fDx + fNy*fDy > 0.0f )
            return false;

        fNx = akV[3].y - akV[2].y;
        fNy = akV[2].x - akV[3].x;
        fDx = rkP.x - akV[3].x;
        fDy = rkP.y - akV[3].y;
        if ( fNx*fDx + fNy*fDy > 0.0f )
            return false;
    }
    return true;
}
//----------------------------------------------------------------------------


