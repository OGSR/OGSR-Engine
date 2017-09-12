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

#include "MgcIntr3DSphrCone.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Sphere& rkSphere, const Cone3& rkCone)
{
    // TO DO.  Add these as members of Cone3 to avoid recomputing on each
    // intersection call.
    float fInvSin = 1.0f/rkCone.SinAngle();
    float fCosSqr = rkCone.CosAngle()*rkCone.CosAngle();

    Vector3 kCmV = rkSphere.Center() - rkCone.Vertex();
    Vector3 kD = kCmV + (rkSphere.Radius()*fInvSin)*rkCone.Axis();
    float fDSqrLen = kD.SquaredLength();
    float fE = kD.Dot(rkCone.Axis());
    if ( fE > 0.0f && fE*fE >= fDSqrLen*fCosSqr )
    {
        // TO DO.  Add this as member of Cone3 to avoid recomputing on each
        // intersection call.
        float fSinSqr = rkCone.SinAngle()*rkCone.SinAngle();

        fDSqrLen = kCmV.SquaredLength();
        fE = -kCmV.Dot(rkCone.Axis());
        if ( fE > 0.0f && fE*fE >= fDSqrLen*fSinSqr )
        {
            // TO DO.  Add this as member of Sphere to avoid recomputing on
            // each intersection call.  (Useful for other functions using
            // squared radius.)
            float fRSqr = rkSphere.Radius()*rkSphere.Radius();
            return fDSqrLen <= fRSqr;
        }
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Sphere& rkSphere, const Cone3& rkCone,
    Vector3& rkClosest)
{
    // test if cone vertex is in sphere
    Vector3 kDiff = rkSphere.Center() - rkCone.Vertex();
    Real fRSqr = rkSphere.Radius()*rkSphere.Radius();
    Real fLSqr = kDiff.SquaredLength();
    if ( fLSqr <= fRSqr)
        return true;

    // test if sphere center is in cone
    Real fDot = kDiff.Dot(rkCone.Axis());
    Real fDotSqr = fDot*fDot;
    Real fCosSqr = rkCone.CosAngle()*rkCone.CosAngle();
    if ( fDotSqr >= fLSqr*fCosSqr && fDot > 0.0f )
    {
        // sphere center is inside cone, so sphere and cone intersect
        return true;
    }

    // Sphere center is outside cone.  Problem now reduces to looking for
    // an intersection between circle and ray in the plane containing
    // cone vertex and spanned by cone axis and vector from vertex to
    // sphere center.

    // Ray is t*D+V (t >= 0) where |D| = 1 and dot(A,D) = cos(angle).
    // Also, D = e*A+f*(C-V).  Plugging ray equation into sphere equation
    // yields R^2 = |t*D+V-C|^2, so the quadratic for intersections is
    // t^2 - 2*dot(D,C-V)*t + |C-V|^2 - R^2 = 0.  An intersection occurs
    // if and only if the discriminant is nonnegative.  This test becomes
    //
    //     dot(D,C-V)^2 >= dot(C-V,C-V) - R^2
    //
    // Note that if the right-hand side is nonpositive, then the inequality
    // is true (the sphere contains V).  I have already ruled this out in
    // the first block of code in this function.

    Real fULen = Math::Sqrt(Math::FAbs(fLSqr-fDotSqr));
    Real fTest = rkCone.CosAngle()*fDot + rkCone.SinAngle()*fULen;
    Real fDiscr = fTest*fTest - fLSqr + fRSqr;

    // compute point of intersection closest to vertex V
    Real fT = fTest - Math::Sqrt(fDiscr);
    Vector3 kB = kDiff - fDot*rkCone.Axis();
    Real fTmp = rkCone.SinAngle()/fULen;
    rkClosest = fT*(rkCone.CosAngle()*rkCone.Axis() + fTmp*kB);

    return fDiscr >= 0.0f && fTest >= 0.0f;
}
//----------------------------------------------------------------------------


