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

#include "MgcIntr2DCirBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
// All functions return -1 if initially intersecting, 0 if no intersection,
// +1 if intersects at some positive time.
//----------------------------------------------------------------------------
static int TestVertexRegion (Real fCx, Real fCy, Real fR, Real fVx, Real fVy,
    Real fEx, Real fEy, Real& rfTFirst, Real& rfIx, Real& rfIy)
{
    Real fDx = fCx + fEx;
    Real fDy = fCy + fEy;
    Real fRSqr = fR*fR;
    Real fDiff = fDx*fDx + fDy*fDy - fRSqr;
    if ( fDiff <= 0.0f )
    {
        // circle is already intersecting the box
        rfTFirst = 0.0f;
        return -1;
    }

    Real fDot = fVx*fDx + fVy*fDy;
    if ( fDot >= 0.0f )
    {
        // circle not moving towards box
        return 0;
    }

    Real fKross = fVx*fDy - fVy*fDx;
    Real fVSqr, fInv;

    if ( fKross >= 0.0f )
    {
        // potential contact on left edge
        if ( fKross <= fR*fVy )
        {
            // lower left corner is first point of contact
            rfIx = -fEx;
            rfIy = -fEy;
            fVSqr = fVx*fVx + fVy*fVy;
            fInv = Math::InvSqrt(Math::FAbs(fDot*fDot-fVSqr*fDiff));
            rfTFirst = fDiff*fInv/(1.0f-fDot*fInv);
            return 1;
        }

        if ( fVx <= 0.0f )
        {
            // passed corner, moving away from box
            return 0;
        }

        fVSqr = fVx*fVx + fVy*fVy;
        fDy = fCy - fEy;
        fKross = fVx*fDy - fVy*fDx;
        if ( fKross >= 0.0f && fKross*fKross > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if ( fKross <= fR*fVy )
        {
            // first contact on left edge of box
            rfTFirst = -(fDx+fR)/fVx;
            rfIy = fCy + rfTFirst*fVy;
        }
        else
        {
            // first contact at upper left corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math::InvSqrt(Math::FAbs(fDot*fDot-fVSqr*fDiff));
            rfTFirst = fDiff*fInv/(1.0f-fDot*fInv);
            rfIy = fEy;
        }
    }
    else
    {
        // potential contact on bottom edge
        if ( -fKross <= fR*fVx )
        {
            // lower left corner is first point of contact
            rfIx = -fEx;
            rfIy = -fEy;
            fVSqr = fVx*fVx + fVy*fVy;
            fInv = Math::InvSqrt(Math::FAbs(fDot*fDot-fVSqr*fDiff));
            rfTFirst = fDiff*fInv/(1.0f-fDot*fInv);
            return 1;
        }

        if ( fVy <= 0.0f )
        {
            // passed corner, moving away from box
            return 0;
        }

        fVSqr = fVx*fVx + fVy*fVy;
        fDx = fCx - fEx;
        fKross = fVx*fDy - fVy*fDx;
        if ( -fKross >= 0.0f && fKross*fKross > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with y = ymin.
        rfIy = -fEy;

        if ( -fKross <= fR*fVx )
        {
            // first contact on bottom edge of box
            rfTFirst = -(fDy+fR)/fVy;
            rfIx = fCx + rfTFirst*fVx;
        }
        else
        {
            // first contact at lower right corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math::InvSqrt(Math::FAbs(fDot*fDot-fVSqr*fDiff));
            rfTFirst = fDiff*fInv/(1.0f-fDot*fInv);
            rfIx = fEx;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------
static int TestEdgeRegion (Real fCx, Real fCy, Real fR, Real fVx, Real fVy,
    Real fEx, Real fEy, Real& rfTFirst, Real& rfIx, Real& rfIy)
{
    Real fDx = fCx + fEx;
    Real fXSignedDist = fDx + fR;
    if ( fXSignedDist >= 0.0f )
    {
        // circle is already intersecting the box
        rfTFirst = 0.0f;
        return -1;
    }

    if ( fVx <= 0.0f )
    {
        // circle not moving towards box
        return 0;
    }

    Real fRSqr = fR*fR;
    Real fVSqr = fVx*fVx + fVy*fVy;
    Real fDy, fDot, fKross, fDiff, fInv;

    if ( fVy >= 0.0f )
    {
        fDy = fCy - fEy;
        fKross = fVx*fDy - fVy*fDx;
        if ( fKross >= 0.0f && fKross*fKross > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if ( fKross <= fR*fVy )
        {
            // first contact on left edge of box
            rfTFirst = -fXSignedDist/fVx;
            rfIy = fCy + rfTFirst*fVy;
        }
        else
        {
            // first contact at corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math::InvSqrt(Math::FAbs(fDot*fDot-fVSqr*fDiff));
            rfTFirst = fDiff*fInv/(1.0f-fDot*fInv);
            rfIy = fEy;
        }
    }
    else
    {
        fDy = fCy + fEy;
        fKross = fVx*fDy - fVy*fDx;
        if ( fKross <= 0.0f && fKross*fKross > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if ( fKross >= fR*fVy )
        {
            // first contact on left edge of box
            rfTFirst = -fXSignedDist/fVx;
            rfIy = fCy + rfTFirst*fVy;
        }
        else
        {
            // first contact at corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math::InvSqrt(Math::FAbs(fDot*fDot-fVSqr*fDiff));
            rfTFirst = fDiff*fInv/(1.0f-fDot*fInv);
            rfIy = -fEy;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------
int Mgc::FindIntersection (const Vector2& rkC, Real fR, const Vector2& rkV,
    const Box2& rkBox, Real& rfTFirst, Vector2& rkIntr)
{
    // Convert circle center to box coordinates.
    Vector2 kDiff = rkC - rkBox.Center();
    Real fCx = kDiff.Dot(rkBox.Axis(0));
    Real fCy = kDiff.Dot(rkBox.Axis(1));
    Real fVx = rkV.Dot(rkBox.Axis(0));
    Real fVy = rkV.Dot(rkBox.Axis(1));
    Real fEx = rkBox.Extent(0);
    Real fEy = rkBox.Extent(1);
    Real fIx, fIy;

    int iType = 0;

    if ( fCx < -fEx )
    {
        if ( fCy < -fEy )
        {
            // region Rmm
            iType = TestVertexRegion(fCx,fCy,fR,fVx,fVy,fEx,fEy,
                rfTFirst,fIx,fIy);
        }
        else if ( fCy <= fEy )
        {
            // region Rmz
            iType = TestEdgeRegion(fCx,fCy,fR,fVx,fVy,fEx,fEy,
                rfTFirst,fIx,fIy);
        }
        else
        {
            // region Rmp
            iType = TestVertexRegion(fCx,-fCy,fR,fVx,-fVy,fEx,fEy,
                rfTFirst,fIx,fIy);
            fIy = -fIy;
        }
    }
    else if ( fCx <= fEx )
    {
        if ( fCy < -fEy )
        {
            // region Rzm
            iType = TestEdgeRegion(fCy,fCx,fR,fVy,fVx,fEy,fEx,
                rfTFirst,fIy,fIx);
        }
        else if ( fCy <= fEy )
        {
            // region Rzz: circle is already intersecting the box
            rfTFirst = 0.0f;
            return -1;
        }
        else
        {
            // region Rzp
            iType = TestEdgeRegion(-fCy,fCx,fR,-fVy,fVx,fEy,fEx,
                rfTFirst,fIy,fIx);
            fIy = -fIy;
        }
    }
    else
    {
        if ( fCy < -fEy )
        {
            // region Rpm
            iType = TestVertexRegion(-fCx,fCy,fR,-fVx,fVy,fEx,fEy,
                rfTFirst,fIx,fIy);
            fIx = -fIx;
        }
        else if ( fCy <= fEy )
        {
            // region Rpz
            iType = TestEdgeRegion(-fCx,fCy,fR,-fVx,fVy,fEx,fEy,
                rfTFirst,fIx,fIy);
            fIx = -fIx;
        }
        else
        {
            // region Rpp
            iType = TestVertexRegion(-fCx,-fCy,fR,-fVx,-fVy,fEx,fEy,
                rfTFirst,fIx,fIy);
            fIx = -fIx;
            fIy = -fIy;
        }
    }

    if ( iType == 1 )
        rkIntr = rkBox.Center() + fIx*rkBox.Axis(0) + fIy*rkBox.Axis(1);

    return iType;
}
//----------------------------------------------------------------------------
