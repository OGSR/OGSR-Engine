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

#include "MgcDist3DLinLin.h"
#include "MgcIntr3DLinCap.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Segment3& rkSegment,
    const Capsule& rkCapsule)
{
    Real fSqrDist = SqrDistance(rkSegment,rkCapsule.Segment());
    return fSqrDist <= rkCapsule.Radius()*rkCapsule.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Ray3& rkRay, const Capsule& rkCapsule)
{
    Real fSqrDist = SqrDistance(rkRay,rkCapsule.Segment());
    return fSqrDist <= rkCapsule.Radius()*rkCapsule.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Line3& rkLine, const Capsule& rkCapsule)
{
    Real fSqrDist = SqrDistance(rkLine,rkCapsule.Segment());
    return fSqrDist <= rkCapsule.Radius()*rkCapsule.Radius();
}
//----------------------------------------------------------------------------
int Mgc::Find (const Vector3& rkOrigin, const Vector3& rkDirection,
    const Capsule& rkCapsule, Real afT[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kU, kV, kW = rkCapsule.Direction();
    Real fWLength = kW.Unitize();
    Vector3::GenerateOrthonormalBasis(kU,kV,kW);
    Vector3 kD(kU.Dot(rkDirection),kV.Dot(rkDirection),
        kW.Dot(rkDirection));
    Real fDLength = kD.Unitize();

    Real fEpsilon = 1e-6f;

    Real fInvDLength = 1.0f/fDLength;
    Vector3 kDiff = rkOrigin - rkCapsule.Origin();
    Vector3 kP(kU.Dot(kDiff),kV.Dot(kDiff),kW.Dot(kDiff));
    Real fRadiusSqr = rkCapsule.Radius()*rkCapsule.Radius();

    Real fInv, fA, fB, fC, fDiscr, fRoot, fT, fTmp;

    // Is the velocity parallel to the capsule direction? (or zero)
    if ( Math::FAbs(kD.z) >= 1.0f - fEpsilon || fDLength < fEpsilon )
    {

        Real fAxisDir = rkDirection.Dot( rkCapsule.Direction() );

        fDiscr = fRadiusSqr - kP.x*kP.x - kP.y*kP.y;
        if ( fAxisDir < 0 && fDiscr >= 0.0f )
        {
            // Velocity anti-parallel to the capsule direction
            fRoot = Math::Sqrt(fDiscr);
            afT[0] = (kP.z + fRoot)*fInvDLength;
            afT[1] = -(fWLength - kP.z + fRoot)*fInvDLength;
            return 2;
        }
        else if ( fAxisDir > 0  && fDiscr >= 0.0f )
        {
            // Velocity parallel to the capsule direction
            fRoot = Math::Sqrt(fDiscr);
            afT[0] = -(kP.z + fRoot)*fInvDLength;
            afT[1] = (fWLength - kP.z + fRoot)*fInvDLength;
            return 2;
        }
        else
        {
            // sphere heading wrong direction, or no velocity at all
            return 0;
        }   
    }

    // test intersection with infinite cylinder
    fA = kD.x*kD.x + kD.y*kD.y;
    fB = kP.x*kD.x + kP.y*kD.y;
    fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
    fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
    {
        // line does not intersect infinite cylinder
        return 0;
    }

    int iQuantity = 0;

    if ( fDiscr > 0.0f )
    {
        // line intersects infinite cylinder in two places
        fRoot = Math::Sqrt(fDiscr);
        fInv = 1.0f/fA;
        fT = (-fB - fRoot)*fInv;
        fTmp = kP.z + fT*kD.z;
        if ( 0.0f <= fTmp && fTmp <= fWLength )
            afT[iQuantity++] = fT*fInvDLength;

        fT = (-fB + fRoot)*fInv;
        fTmp = kP.z + fT*kD.z;
        if ( 0.0f <= fTmp && fTmp <= fWLength )
            afT[iQuantity++] = fT*fInvDLength;

        if ( iQuantity == 2 )
        {
            // line intersects capsule wall in two places
            return 2;
        }
    }
    else
    {
        // line is tangent to infinite cylinder
        fT = -fB/fA;
        fTmp = kP.z + fT*kD.z;
        if ( 0.0f <= fTmp && fTmp <= fWLength )
        {
            afT[0] = fT*fInvDLength;
            return 1;
        }
    }

    // test intersection with bottom hemisphere
    // fA = 1
    fB += kP.z*kD.z;
    fC += kP.z*kP.z;
    fDiscr = fB*fB - fC;
    if ( fDiscr > 0.0f )
    {
        fRoot = Math::Sqrt(fDiscr);
        fT = -fB - fRoot;
        fTmp = kP.z + fT*kD.z;
        if ( fTmp <= 0.0f )
        {
            afT[iQuantity++] = fT*fInvDLength;
            if ( iQuantity == 2 )
                return 2;
        }

        fT = -fB + fRoot;
        fTmp = kP.z + fT*kD.z;
        if ( fTmp <= 0.0f )
        {
            afT[iQuantity++] = fT*fInvDLength;
            if ( iQuantity == 2 )
                return 2;
        }
    }
    else if ( fDiscr == 0.0f )
    {
        fT = -fB;
        fTmp = kP.z + fT*kD.z;
        if ( fTmp <= 0.0f )
        {
            afT[iQuantity++] = fT*fInvDLength;
            if ( iQuantity == 2 )
                return 2;
        }
    }

    // test intersection with top hemisphere
    // fA = 1
    fB -= kD.z*fWLength;
    fC += fWLength*(fWLength - 2.0f*kP.z);

    fDiscr = fB*fB - fC;
    if ( fDiscr > 0.0f )
    {
        fRoot = Math::Sqrt(fDiscr);
        fT = -fB - fRoot;
        fTmp = kP.z + fT*kD.z;
        if ( fTmp >= fWLength )
        {
            afT[iQuantity++] = fT*fInvDLength;
            if ( iQuantity == 2 )
                return 2;
        }

        fT = -fB + fRoot;
        fTmp = kP.z + fT*kD.z;
        if ( fTmp >= fWLength )
        {
            afT[iQuantity++] = fT*fInvDLength;
            if ( iQuantity == 2 )
                return 2;
        }
    }
    else if ( fDiscr == 0.0f )
    {
        fT = -fB;
        fTmp = kP.z + fT*kD.z;
        if ( fTmp >= fWLength )
        {
            afT[iQuantity++] = fT*fInvDLength;
            if ( iQuantity == 2 )
                return 2;
        }
    }

    return iQuantity;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment3& rkSegment,
    const Capsule& rkCapsule, int& riQuantity, Vector3 akPoint[2])
{
    Real afT[2];
    riQuantity = Find(rkSegment.Origin(),rkSegment.Direction(),rkCapsule,afT);

    int iClipQuantity = 0;
    for (int i = 0; i < riQuantity; i++)
    {
        if ( 0.0f <= afT[i] && afT[i] <= 1.0f )
        {
            akPoint[iClipQuantity++] = rkSegment.Origin() +
                afT[i]*rkSegment.Direction();
        }
    }

    riQuantity = iClipQuantity;
    return riQuantity > 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray3& rkRay, const Capsule& rkCapsule,
    int& riQuantity, Vector3 akPoint[2])
{
    Real afT[2];
    riQuantity = Find(rkRay.Origin(),rkRay.Direction(),rkCapsule,afT);

    int iClipQuantity = 0;
    for (int i = 0; i < riQuantity; i++)
    {
        if ( afT[i] >= 0.0f )
        {
            akPoint[iClipQuantity++] = rkRay.Origin() +
                afT[i]*rkRay.Direction();
        }
    }

    riQuantity = iClipQuantity;
    return riQuantity > 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line3& rkLine, const Capsule& rkCapsule,
    int& riQuantity, Vector3 akPoint[2])
{
    Real afT[2];
    riQuantity = Find(rkLine.Origin(),rkLine.Direction(),rkCapsule,afT);

    for (int i = 0; i < riQuantity; i++)
        akPoint[i] = rkLine.Origin() + afT[i]*rkLine.Direction();

    return riQuantity > 0;
}
//----------------------------------------------------------------------------


