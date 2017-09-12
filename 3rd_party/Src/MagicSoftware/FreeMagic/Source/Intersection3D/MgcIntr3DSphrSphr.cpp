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

// Intersection of sphere with sphere.
//
// Two spheres are |X-C0|^2 = R0^2 and |X-C1|^2 = R1^2.  If the centers are
// identical (C1=C0), then the spheres intersect if and only if R0=R1.  If
// there is a circle of intersection, the plane of intersection must have
// normal N = C1-C0.  Let U and V be unit vectors such that Dot(N,U) = 0,
// Dot(N,V) = 0, Dot(U,V) = 0.  The circle of intersection is given by
//
//   X = C+R*(cos(A)*U+sin(A)*V)
//
// where C=C0+t*N with 0 <= t <= 1 (t is fixed) and 0 <= A < 2*pi
// (A is variable).
//
// Plug into first sphere equation to get
//
//   R0^2 = t^2*|N|^2 + R^2
//
// Plug into second sphere equation to get
//
//   R1^2 = (t-1)^2*|N|^2 + R^2
//
// Subtract these two equations and solve for t to get
//
//   t = 0.5*(1+(R0^2-R1^2)/|N|^2)
//
// It is necessary that 0 <= t <= 1 for there to be an intersection since
// the center of the circle of intersection must be on the segment connecting
// C0 and C1.
//
// Then plug this value of t into the first of the previous equations and
// solve for
//
//   R^2 = R0^2 - t^2*|N|^2
//
// As long as the right-hand side is nonnegative, the circle of intersection
// exists and the radius is R = sqrt(R0^2 - t^2*|N|^2).

#include "MgcIntr3DSphrSphr.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Sphere& rkS0, const Sphere& rkS1)
{
    Vector3 kDiff = rkS1.Center() - rkS0.Center();
    Real fSqrLen = kDiff.SquaredLength();
    Real fRSum = rkS0.Radius() + rkS1.Radius();
    Real fRSumSqr = fRSum*fRSum;

    return fSqrLen <= fRSumSqr;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Sphere& rkS0, const Sphere& rkS1,
    Vector3& rkU, Vector3& rkV, Vector3& rkC, Real& rfR)
{
    // plane of intersection must have N as its normal
    Vector3 kN = rkS1.Center() - rkS0.Center();
    Real fNSqrLen = kN.SquaredLength();
    Real fRSum = rkS0.Radius() + rkS1.Radius();
    if ( fNSqrLen > fRSum*fRSum )
    {
        // sphere centers are too far apart for intersection
        return false;
    }

    Real fR0Sqr = rkS0.Radius()*rkS0.Radius();
    Real fR1Sqr = rkS1.Radius()*rkS1.Radius();
    Real fInvNSqrLen = 1.0f/fNSqrLen;
    Real fT = 0.5f*(1.0f+(fR0Sqr-fR1Sqr)*fInvNSqrLen);
    if ( fT < 0.0f || fT > 1.0f )
        return false;

    Real fRSqr = fR0Sqr - fT*fT*fNSqrLen;
    if ( fRSqr < 0.0f )
        return false;

    // center and radius of circle of intersection
    rkC = rkS0.Center() + fT*kN;
    rfR = Math::Sqrt(fRSqr);

    // compute U and V for plane of circle
    kN *= Math::Sqrt(fInvNSqrLen);
    Vector3::GenerateOrthonormalBasis(rkU,rkV,kN);

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Sphere& rkS0, const Sphere& rkS1,
    Real fTime, const Vector3& rkV0, const Vector3& rkV1)
{
    Vector3 kVDiff = rkV1 - rkV0;
    Real fA = kVDiff.SquaredLength();
    Vector3 kCDiff = rkS1.Center() - rkS0.Center();
    Real fC = kCDiff.SquaredLength();
    Real fRSum = rkS0.Radius() + rkS1.Radius();
    Real fRSumSqr = fRSum*fRSum;

    if ( fA > 0.0f )
    {
        Real fB = kCDiff.Dot(kVDiff);
        if ( fB <= 0.0f )
        {
            if ( -fTime*fA <= fB )
                return fA*fC - fB*fB <= fA*fRSumSqr;
            else
                return fTime*(fTime*fA + 2.0f*fB) + fC <= fRSumSqr;
        }
    }

    return fC <= fRSumSqr;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Sphere& rkS0, const Sphere& rkS1,
    Real fTime, const Vector3& rkV0, const Vector3& rkV1, Real& rfFirstTime,
    Vector3& rkFirstPoint)
{
    Vector3 kVDiff = rkV1 - rkV0;
    Real fA = kVDiff.SquaredLength();
    Vector3 kCDiff = rkS1.Center() - rkS0.Center();
    Real fC = kCDiff.SquaredLength();
    Real fRSum = rkS0.Radius() + rkS1.Radius();
    Real fRSumSqr = fRSum*fRSum;

    if ( fA > 0.0f )
    {
        Real fB = kCDiff.Dot(kVDiff);
        if ( fB <= 0.0f )
        {
            if ( -fTime*fA <= fB
            ||   fTime*(fTime*fA + 2.0f*fB) + fC <= fRSumSqr )
            {
                Real fCDiff = fC - fRSumSqr;
                Real fDiscr = fB*fB - fA*fCDiff;
                if ( fDiscr >= 0.0f )
                {
                    if ( fCDiff <= 0.0f )
                    {
                        // The spheres are initially intersecting.  Estimate a
                        // point of contact by using the midpoint of the line
                        // segment connecting the sphere centers.
                        rfFirstTime = 0.0f;
                        rkFirstPoint = 0.5f*(rkS0.Center() + rkS1.Center());
                    }
                    else
                    {
                        // The first time of contact is in [0,fTime].
                        rfFirstTime = -(fB + Math::Sqrt(fDiscr))/fA;
                        if ( rfFirstTime < 0.0f )
                            rfFirstTime = 0.0f;
                        else if ( rfFirstTime > fTime )
                            rfFirstTime = fTime;

                        Vector3 kNewCDiff = kCDiff + rfFirstTime*kVDiff;

                        rkFirstPoint = rkS0.Center() + rfFirstTime*rkV0 +
                            (rkS0.Radius()/fRSum)*kNewCDiff;
                    }
                    return true;
                }
            }
            return false;
        }
    }

    if ( fC <= fRSumSqr )
    {
        // The spheres are initially intersecting.  Estimate a point of
        // contact by using the midpoint of the line segment connecting the
        // sphere centers.
        rfFirstTime = 0.0f;
        rkFirstPoint = 0.5f*(rkS0.Center() + rkS1.Center());
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------

