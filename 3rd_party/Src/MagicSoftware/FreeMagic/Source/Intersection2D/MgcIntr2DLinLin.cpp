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

#include "MgcIntr2DLinLin.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static bool Find (const Vector2& rkP0, const Vector2& rkD0,
    const Vector2& rkP1, const Vector2& rkD1, Vector2& rkDiff,
    Real& rfD0SqrLen, int& riQuantity, Real afT[2])
{
    // Intersection is a solution to P0+s*D0 = P1+t*D1.  Rewrite as
    // s*D0 - t*D1 = P1 - P0, a 2x2 system of equations.  If D0 = (x0,y0)
    // and D1 = (x1,y1) and P1 - P0 = (c0,c1), then the system is
    // x0*s - x1*t = c0 and y0*s - y1*t = c1.  The error tests are relative
    // to the size of the direction vectors, |Cross(D0,D1)| >= e*|D0|*|D1|
    // rather than absolute tests |Cross(D0,D1)| >= e.  The quantities
    // P1-P0, |D0|^2, and |D1|^2 are returned for use by calling functions.

    Real fDet = rkD1.x*rkD0.y - rkD1.y*rkD0.x;
    rkDiff = rkP1 - rkP0;
    rfD0SqrLen = rkD0.SquaredLength();

    const Real fEpsilon = 1e-06f;
    if ( fDet*fDet > fEpsilon*rfD0SqrLen*rkD1.SquaredLength() )
    {
        // Lines intersect in a single point.  Return both s and t values for
        // use by calling functions.
        Real fInvDet = 1.0f/fDet;
        riQuantity = 1;
        afT[0] = (rkD1.x*rkDiff.y - rkD1.y*rkDiff.x)*fInvDet;
        afT[1] = (rkD0.x*rkDiff.y - rkD0.y*rkDiff.x)*fInvDet;
    }
    else
    {
        // lines are parallel
        fDet = rkD0.x*rkDiff.y - rkD0.y*rkDiff.x;
        if ( fDet*fDet > fEpsilon*rfD0SqrLen*rkDiff.SquaredLength() )
        {
            // lines are disjoint
            riQuantity = 0;
        }
        else
        {
            // lines are the same
            riQuantity = 2;
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line2& rkLine0, const Line2& rkLine1,
    int& riQuantity, Real afT[2])
{
    Vector2 kDiff;
    Real fD0SqrLen;
    bool bIntersects = Find(rkLine0.Origin(),rkLine0.Direction(),
        rkLine1.Origin(),rkLine1.Direction(),kDiff,fD0SqrLen,riQuantity,afT);

    if ( bIntersects )
    {
        if ( riQuantity == 2 )
        {
            afT[0] = -Math::MAX_REAL;
            afT[1] = Math::MAX_REAL;
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line2& rkLine, const Ray2& rkRay,
    int& riQuantity, Real afT[2])
{
    Vector2 kDiff;
    Real fD0SqrLen;
    bool bIntersects = Find(rkLine.Origin(),rkLine.Direction(),
        rkRay.Origin(),rkRay.Direction(),kDiff,fD0SqrLen,riQuantity,afT);

    if ( bIntersects )
    {
        if ( riQuantity == 1 )
        {
            if ( afT[1] < 0.0f )
            {
                // lines intersect, but ray does not intersect line
                riQuantity = 0;
            }
        }
        else
        {
            // ray is contained by line, adjust intersection interval
            if ( rkLine.Direction().Dot(rkRay.Direction()) > 0.0f )
            {
                afT[0] = (kDiff.Dot(rkLine.Direction()))/fD0SqrLen;
                afT[1] = Math::MAX_REAL;
            }
            else
            {
                afT[0] = -Math::MAX_REAL;
                afT[1] = (kDiff.Dot(rkLine.Direction()))/fD0SqrLen;
            }
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line2& rkLine, const Segment2& rkSegment,
    int& riQuantity, Real afT[2])
{
    Vector2 kDiff;
    Real fD0SqrLen;
    bool bIntersects = Find(rkLine.Origin(),rkLine.Direction(),
        rkSegment.Origin(),rkSegment.Direction(),kDiff,fD0SqrLen,riQuantity,
        afT);

    if ( bIntersects )
    {
        if ( riQuantity == 1 )
        {
            if ( afT[1] < 0.0f || afT[1] > 1.0f )
            {
                // lines intersect, but segment does not intersect line
                riQuantity = 0;
            }
        }
        else
        {
            // segment is contained by line, adjust intersection interval
            Real fDot = rkLine.Direction().Dot(rkSegment.Direction());
            Real fInvLen = 1.0f/fD0SqrLen;
            if ( fDot > 0.0f )
            {
                afT[0] = (kDiff.Dot(rkLine.Direction()))*fInvLen;
                afT[1] = afT[0] + fDot*fInvLen;
            }
            else
            {
                afT[1] = (kDiff.Dot(rkLine.Direction()))*fInvLen;
                afT[0] = afT[1] + fDot*fInvLen;
            }
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray2& rkRay0, const Ray2& rkRay1,
    int& riQuantity, Real afT[2])
{
    Vector2 kDiff;
    Real fD0SqrLen;
    bool bIntersects = Find(rkRay0.Origin(),rkRay0.Direction(),
        rkRay1.Origin(),rkRay1.Direction(),kDiff,fD0SqrLen,riQuantity,afT);

    if ( bIntersects )
    {
        if ( riQuantity == 1 )
        {
            if ( afT[0] < 0.0f || afT[1] < 0.0f )
            {
                // lines intersect, but rays do not
                riQuantity = 0;
            }
        }
        else
        {
            // rays are on same line
            Real fDot = kDiff.Dot(rkRay0.Direction());
            if ( rkRay0.Direction().Dot(rkRay1.Direction()) > 0.0f )
            {
                // Rays point in same direction, get semiinfinite interval
                // of intersection.
                afT[0] = ( fDot >= 0.0f ? fDot/fD0SqrLen : 0.0f );
                afT[1] = Math::MAX_REAL;
            }
            else
            {
                // test for overlap of opposite pointing rays
                if ( fDot > 0.0f )
                {
                    afT[0] = 0.0f;
                    afT[1] = fDot/fD0SqrLen;
                }
                else if ( fDot < 0.0f )
                {
                    // no overlap
                    riQuantity = 0;
                }
                else
                {
                    // rays have a common origin
                    riQuantity = 1;
                    afT[0] = 0.0f;
                }
            }
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray2& rkRay, const Segment2& rkSegment,
    int& riQuantity, Real afT[2])
{
    Vector2 kDiff;
    Real fD0SqrLen;
    bool bIntersects = Find(rkRay.Origin(),rkRay.Direction(),
        rkSegment.Origin(),rkSegment.Direction(),kDiff,fD0SqrLen,riQuantity,
        afT);

    if ( bIntersects )
    {
        if ( riQuantity == 1 )
        {
            if ( afT[0] < 0.0f || afT[1] < 0.0f || afT[1] > 1.0f )
            {
                // lines intersect, but ray and segment do not
                riQuantity = 0;
            }
        }
        else
        {
            // ray and segment are on the same line
            Real fDotRS = rkRay.Direction().Dot(rkSegment.Direction());
            Real fDot0, fDot1;
            if ( fDotRS > 0.0f )
            {
                fDot0 = kDiff.Dot(rkRay.Direction());
                fDot1 = fDot0 + fDotRS;
            }
            else
            {
                fDot1 = kDiff.Dot(rkRay.Direction());
                fDot0 = fDot1 + fDotRS;
            }

            // compute intersection of [t0,t1] and [0,+infinity]
            if ( fDot0 >= 0.0f )
            {
                // complete overlap
                Real fInvLen = 1.0f/fD0SqrLen;
                afT[0] = fDot0*fInvLen;
                afT[1] = fDot1*fInvLen;
            }
            else if ( fDot1 > 0.0f )
            {
                // partial overlap
                afT[0] = 0.0f;
                afT[1] = fDot1/fD0SqrLen;
            }
            else if ( fDot1 < 0.0f )
            {
                // no overlap
                riQuantity = 0;
            }
            else
            {
                // overlap at a single end point
                riQuantity = 1;
                afT[0] = 0.0f;
            }
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment2& rkSegment0,
    const Segment2& rkSegment1, int& riQuantity, Real afT[2])
{
    Vector2 kDiff;
    Real fD0SqrLen;
    bool bIntersects = Find(rkSegment0.Origin(),rkSegment0.Direction(),
        rkSegment1.Origin(),rkSegment1.Direction(),kDiff,fD0SqrLen,
        riQuantity,afT);

    if ( bIntersects )
    {
        if ( riQuantity == 1 )
        {
            if ( afT[0] < 0.0f || afT[0] > 1.0f
            ||   afT[1] < 0.0f || afT[1] > 1.0f )
            {
                // lines intersect, but segments do not
                riQuantity = 0;
            }
        }
        else
        {
            // segments are on the same line
            Real fDotRS =
                rkSegment0.Direction().Dot(rkSegment1.Direction());
            Real fDot0, fDot1;
            if ( fDotRS > 0.0f )
            {
                fDot0 = kDiff.Dot(rkSegment0.Direction());
                fDot1 = fDot0 + fDotRS;
            }
            else
            {
                fDot1 = kDiff.Dot(rkSegment0.Direction());
                fDot0 = fDot1 + fDotRS;
            }

            // compute intersection of [t0,t1] and [0,1]
            if ( fDot1 < 0.0f || fDot0 > fD0SqrLen )
            {
                riQuantity = 0;
            }
            else if ( fDot1 > 0.0f )
            {
                if ( fDot0 < fD0SqrLen )
                {
                    Real fInvLen = 1.0f/fD0SqrLen;
                    riQuantity = 2;
                    afT[0] = ( fDot0 < 0.0f ? 0.0f : fDot0*fInvLen );
                    afT[1] = ( fDot1 > fD0SqrLen ? 1.0f : fDot1*fInvLen );
                }
                else  // fT0 == 1
                {
                    riQuantity = 1;
                    afT[0] = 1.0f;
                }
            }
            else  // fT1 == 0
            {
                riQuantity = 1;
                afT[0] = 0.0f;
            }
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------


