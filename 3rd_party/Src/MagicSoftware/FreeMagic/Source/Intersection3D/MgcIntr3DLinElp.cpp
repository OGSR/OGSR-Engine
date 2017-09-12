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

#include "MgcIntr3DLinElp.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Segment3& rkSegment,
    const Ellipsoid& rkEllipsoid)
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkSegment.Origin() - rkEllipsoid.Center();
    Vector3 kMatDir = rkEllipsoid.A()*rkSegment.Direction();
    Vector3 kMatDiff = rkEllipsoid.A()*kDiff;
    Real fA = rkSegment.Direction().Dot(kMatDir);
    Real fB = rkSegment.Direction().Dot(kMatDiff);
    Real fC = kDiff.Dot(kMatDiff) - 1.0f;

    // no intersection if Q(t) has no real roots
    Real fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
        return false;

    // test if line origin is inside ellipsoid
    if ( fC <= 0.0f )
        return true;

    // At this point fC > 0 and Q(t) has real roots.  No intersection if
    // Q'(0) >= 0.
    if ( fB >= 0.0f )
        return false;

    // Need to determine if Q(t) has real roots on [0,1].  Effectively is
    // a test for sign changes of Sturm polynomials.
    Real fSum = fA + fB;
    if ( fSum >= 0.0f )
        return true;

    return fSum + fB + fC <= 0.0f;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Ray3& rkRay, const Ellipsoid& rkEllipsoid)
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkRay.Origin() - rkEllipsoid.Center();
    Vector3 kMatDir = rkEllipsoid.A()*rkRay.Direction();
    Vector3 kMatDiff = rkEllipsoid.A()*kDiff;
    Real fA = rkRay.Direction().Dot(kMatDir);  // fA > 0 is necessary
    Real fB = rkRay.Direction().Dot(kMatDiff);
    Real fC = kDiff.Dot(kMatDiff) - 1.0f;

    // no intersection if Q(t) has no real roots
    Real fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
        return false;

    // test if ray origin is inside ellipsoid
    if ( fC <= 0.0f )
        return true;

    // At this point, fC > 0 and Q(t) has real roots.  Intersection occurs
    // if Q'(0) < 0.
    return fB < 0.0f;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Line3& rkLine, const Ellipsoid& rkEllipsoid)
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkLine.Origin() - rkEllipsoid.Center();
    Vector3 kMatDir = rkEllipsoid.A()*rkLine.Direction();
    Vector3 kMatDiff = rkEllipsoid.A()*kDiff;
    Real fA = rkLine.Direction().Dot(kMatDir);
    Real fB = rkLine.Direction().Dot(kMatDiff);
    Real fC = kDiff.Dot(kMatDiff) - 1.0f;

    // intersection occurs if Q(t) has real roots
    Real fDiscr = fB*fB - fA*fC;
    return fDiscr >= 0.0f;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment3& rkSegment,
    const Ellipsoid& rkEllipsoid, int& riQuantity, Vector3 akPoint[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkSegment.Origin() - rkEllipsoid.Center();
    Vector3 kMatDir = rkEllipsoid.A()*rkSegment.Direction();
    Vector3 kMatDiff = rkEllipsoid.A()*kDiff;
    Real fA = rkSegment.Direction().Dot(kMatDir);
    Real fB = rkSegment.Direction().Dot(kMatDiff);
    Real fC = kDiff.Dot(kMatDiff) - 1.0f;

    // no intersection if Q(t) has no real roots
    Real afT[2];
    Real fDiscr = fB*fB - fA*fC;

    if ( fDiscr < 0.0f )
    {
        riQuantity = 0;
        return false;
    }
    else if ( fDiscr > 0.0f )
    {
        Real fRoot = Math::Sqrt(fDiscr);
        Real fInvA = 1.0f/fA;
        afT[0] = (-fB - fRoot)*fInvA;
        afT[1] = (-fB + fRoot)*fInvA;

        // assert: t0 < t1 since A > 0

        if ( afT[0] > 1.0f || afT[1] < 0.0f )
        {
            riQuantity = 0;
            return false;
        }
        else if ( afT[0] >= 0.0f )
        {
            if ( afT[1] > 1.0f )
            {
                riQuantity = 1;
                akPoint[0] = rkSegment.Origin()+afT[0]*rkSegment.Direction();
                return true;
            }
            else
            {
                riQuantity = 2;
                akPoint[0] = rkSegment.Origin()+afT[0]*rkSegment.Direction();
                akPoint[1] = rkSegment.Origin()+afT[1]*rkSegment.Direction();
                return true;
            }
        }
        else  // afT[1] >= 0
        {
            riQuantity = 1;
            akPoint[0] = rkSegment.Origin()+afT[1]*rkSegment.Direction();
            return true;
        }
    }
    else
    {
        afT[0] = -fB/fA;
        if ( 0.0f <= afT[0] && afT[0] <= 1.0f )
        {
            riQuantity = 1;
            akPoint[0] = rkSegment.Origin()+afT[0]*rkSegment.Direction();
            return true;
        }
        else
        {
            riQuantity = 0;
            return false;
        }
    }
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray3& rkRay, const Ellipsoid& rkEllipsoid,
    int& riQuantity, Vector3 akPoint[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkRay.Origin() - rkEllipsoid.Center();
    Vector3 kMatDir = rkEllipsoid.A()*rkRay.Direction();
    Vector3 kMatDiff = rkEllipsoid.A()*kDiff;
    Real fA = rkRay.Direction().Dot(kMatDir);  // fA > 0 is necessary
    Real fB = rkRay.Direction().Dot(kMatDiff);
    Real fC = kDiff.Dot(kMatDiff) - 1.0f;

    Real afT[2];
    Real fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
    {
        riQuantity = 0;
        return false;
    }
    else if ( fDiscr > 0.0f )
    {
        Real fRoot = Math::Sqrt(fDiscr);
        Real fInvA = 1.0f/fA;
        afT[0] = (-fB - fRoot)*fInvA;
        afT[1] = (-fB + fRoot)*fInvA;

        if ( afT[0] >= 0.0f )
        {
            riQuantity = 2;
            akPoint[0] = rkRay.Origin() + afT[0]*rkRay.Direction();
            akPoint[1] = rkRay.Origin() + afT[1]*rkRay.Direction();
            return true;
        }
        else if ( afT[1] >= 0.0f )
        {
            riQuantity = 1;
            akPoint[0] = rkRay.Origin() + afT[1]*rkRay.Direction();
            return true;
        }
        else
        {
            riQuantity = 0;
            return false;
        }
    }
    else
    {
        afT[0] = -fB/fA;
        if ( afT[0] >= 0.0f )
        {
            riQuantity = 1;
            akPoint[0] = rkRay.Origin() + afT[0]*rkRay.Direction();
            return true;
        }
        else
        {
            riQuantity = 0;
            return false;
        }
    }
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line3& rkLine, const Ellipsoid& rkEllipsoid,
    int& riQuantity, Vector3 akPoint[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkLine.Origin() - rkEllipsoid.Center();
    Vector3 kMatDir = rkEllipsoid.A()*rkLine.Direction();
    Vector3 kMatDiff = rkEllipsoid.A()*kDiff;
    Real fA = rkLine.Direction().Dot(kMatDir);
    Real fB = rkLine.Direction().Dot(kMatDiff);
    Real fC = kDiff.Dot(kMatDiff) - 1.0f;

    Real afT[2];
    Real fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
    {
        riQuantity = 0;
        return false;
    }
    else if ( fDiscr > 0.0f )
    {
        Real fRoot = Math::Sqrt(fDiscr);
        Real fInvA = 1.0f/fA;
        riQuantity = 2;
        afT[0] = (-fB - fRoot)*fInvA;
        afT[1] = (-fB + fRoot)*fInvA;
        akPoint[0] = rkLine.Origin() + afT[0]*rkLine.Direction();
        akPoint[1] = rkLine.Origin() + afT[1]*rkLine.Direction();
        return true;
    }
    else
    {
        riQuantity = 1;
        afT[0] = -fB/fA;
        akPoint[0] = rkLine.Origin() + afT[0]*rkLine.Direction();
        return true;
    }
}
//----------------------------------------------------------------------------


