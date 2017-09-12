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

#include "MgcIntr2DLinCir.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static bool Find (const Vector2& rkP, const Vector2& rkD, const Vector2& rkC,
    Real fR, int& riRootCount, Real afT[2])
{
    // Intersection of a the line P+t*D and the circle |X-C| = R.  The t
    // value is a root to the quadratic equation:
    //   0 = |t*D+P-C|^2 - R^2
    //     = |D|^2*t^2 + 2*D.Dot(P-C)*t + |P-C|^2-R^2
    //     = a2*t^2 + 2*a1*t + a0
    // If two roots are returned, the order is T[0] < T[1].  Hopefully the
    // application will be kind and provide line directions D that are not so
    // small that a2 is nearly zero and potentially creates numerical
    // problems.

    Vector2 kDiff = rkP - rkC;
    Real fA0 = kDiff.SquaredLength() - fR*fR;
    Real fA1 = rkD.Dot(kDiff);
    Real fA2 = rkD.SquaredLength();
    Real fDiscr = fA1*fA1 - fA0*fA2;
    if ( fDiscr > 0.0f )
    {
        riRootCount = 2;
        Real fInvA2 = 1.0f/fA2;
        fDiscr = Math::Sqrt(fDiscr);
        afT[0] = (-fA1 - fDiscr)*fInvA2;
        afT[1] = (-fA1 + fDiscr)*fInvA2;
    }
    else if ( fDiscr < 0.0f )
    {
        riRootCount = 0;
    }
    else  // fDiscr == 0
    {
        riRootCount = 1;
        afT[0] = -fA1/fA2;
    }

    return riRootCount != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line2& rkLine, const Circle2& rkCircle,
    int& riQuantity, Vector2 akPoint[2])
{
    Real afT[2];
    bool bIntersects = Find(rkLine.Origin(),rkLine.Direction(),
        rkCircle.Center(),rkCircle.Radius(),riQuantity,afT);

    if ( bIntersects )
    {
        // construct line-circle points of intersection
        for (int i = 0; i < riQuantity; i++)
            akPoint[i] = rkLine.Origin() + afT[i]*rkLine.Direction();
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line2& rkLine, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2])
{
    int iRootCount;
    Real afT[2];
    bool bIntersects = Find(rkLine.Origin(),rkLine.Direction(),
        rkArc.Center(),rkArc.Radius(),iRootCount,afT);

    riQuantity = 0;
    if ( bIntersects )
    {
        // Construct line-circle points of intersection and test if they are
        // on the arc.
        for (int i = 0; i < iRootCount; i++)
        {
            akPoint[riQuantity] = rkLine.Origin() + afT[i]*rkLine.Direction();
            if ( rkArc.Contains(akPoint[riQuantity]) )
                riQuantity++;
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray2& rkRay, const Circle2& rkCircle,
    int& riQuantity, Vector2 akPoint[2])
{
    Real afT[2];
    bool bIntersects = Find(rkRay.Origin(),rkRay.Direction(),
        rkCircle.Center(),rkCircle.Radius(),riQuantity,afT);

    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on ray
        if ( riQuantity == 1 )
        {
            if ( afT[0] < 0.0f )
                riQuantity = 0;
        }
        else
        {
            if ( afT[1] < 0.0f )
            {
                riQuantity = 0;
            }
            else if ( afT[0] < 0.0f )
            {
                riQuantity = 1;
                afT[0] = afT[1];
            }
        }

        // construct ray-circle points of intersection
        for (int i = 0; i < riQuantity; i++)
            akPoint[i] = rkRay.Origin() + afT[i]*rkRay.Direction();
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray2& rkRay, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2])
{
    int iRootCount;
    Real afT[2];
    bool bIntersects = Find(rkRay.Origin(),rkRay.Direction(),rkArc.Center(),
        rkArc.Radius(),iRootCount,afT);

    riQuantity = 0;
    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on ray
        if ( iRootCount == 1 )
        {
            if ( afT[0] < 0.0f )
                iRootCount = 0;
        }
        else
        {
            if ( afT[1] < 0.0f )
            {
                iRootCount = 0;
            }
            else if ( afT[0] < 0.0f )
            {
                iRootCount = 1;
                afT[0] = afT[1];
            }
        }

        // Construct ray-circle points of intersection and test if they are
        // on the arc.
        for (int i = 0; i < iRootCount; i++)
        {
            akPoint[riQuantity] = rkRay.Origin() + afT[i]*rkRay.Direction();
            if ( rkArc.Contains(akPoint[riQuantity]) )
                riQuantity++;
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment2& rkSegment,
    const Circle2& rkCircle, int& riQuantity, Vector2 akPoint[2])
{
    Real afT[2];
    bool bIntersects = Find(rkSegment.Origin(),rkSegment.Direction(),
        rkCircle.Center(),rkCircle.Radius(),riQuantity,afT);

    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on segment
        if ( riQuantity == 1 )
        {
            if ( afT[0] < 0.0f || afT[0] > 1.0f )
                riQuantity = 1;
        }
        else
        {
            if ( afT[1] < 0.0f || afT[0] > 1.0f )
            {
                riQuantity = 0;
            }
            else
            {
                if ( afT[1] <= 1.0f )
                {
                    if ( afT[0] < 0.0f )
                    {
                        riQuantity = 1;
                        afT[0] = afT[1];
                    }
                }
                else
                {
                    riQuantity = ( afT[0] >= 0.0f ? 1 : 0 );
                }
            }
        }

        // construct ray-circle points of intersection
        for (int i = 0; i < riQuantity; i++)
            akPoint[i] = rkSegment.Origin() + afT[i]*rkSegment.Direction();
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment2& rkSegment, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2])
{
    int iRootCount;
    Real afT[2];
    bool bIntersects = Find(rkSegment.Origin(),rkSegment.Direction(),
        rkArc.Center(),rkArc.Radius(),iRootCount,afT);

    riQuantity = 0;
    if ( bIntersects )
    {
        // reduce root count if line-circle intersections are not on segment
        if ( iRootCount == 1 )
        {
            if ( afT[0] < 0.0f || afT[0] > 1.0f )
                iRootCount = 1;
        }
        else
        {
            if ( afT[1] < 0.0f || afT[0] > 1.0f )
            {
                iRootCount = 0;
            }
            else
            {
                if ( afT[1] <= 1.0f )
                {
                    if ( afT[0] < 0.0f )
                    {
                        iRootCount = 1;
                        afT[0] = afT[1];
                    }
                }
                else
                {
                    iRootCount = ( afT[0] >= 0.0f ? 1 : 0 );
                }
            }
        }

        // Construct segment-circle points of intersection and test if they
        // are on the arc.
        for (int i = 0; i < iRootCount; i++)
        {
            akPoint[riQuantity] = rkSegment.Origin() +
                afT[i]*rkSegment.Direction();
            if ( rkArc.Contains(akPoint[riQuantity]) )
                riQuantity++;
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------


