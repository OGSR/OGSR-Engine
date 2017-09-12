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
#include "MgcIntr3DLinCyln.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static int Find (const Vector3& rkOrigin, const Vector3& rkDirection,
    const Cylinder& rkCylinder, Real afT[2])
{
    const Real fEpsilon = 1e-12f;

    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kU, kV, kW = rkCylinder.Direction();
    Vector3::GenerateOrthonormalBasis(kU,kV,kW);
    Vector3 kD(kU.Dot(rkDirection),kV.Dot(rkDirection),kW.Dot(rkDirection));
    Real fDLength = kD.Unitize();
    Real fInvDLength = 1.0f/fDLength;
    Vector3 kDiff = rkOrigin - rkCylinder.Center();
    Vector3 kP(kU.Dot(kDiff),kV.Dot(kDiff),kW.Dot(kDiff));
    Real fHalfHeight = 0.5f*rkCylinder.Height();
    Real fRadiusSqr = rkCylinder.Radius()*rkCylinder.Radius();

    Real fInv, fA, fB, fC, fDiscr, fRoot, fT, fT0, fT1, fTmp0, fTmp1;

    if ( Math::FAbs(kD.z) >= 1.0f - fEpsilon )
    {
        // line is parallel to cylinder axis
        if ( kP.x*kP.x+kP.y*kP.y <= fRadiusSqr )
        {
            fTmp0 = fInvDLength/kD.z;
            afT[0] = (+fHalfHeight - kP.z)*fTmp0;
            afT[1] = (-fHalfHeight - kP.z)*fTmp0;
            return 2;
        }
        else
        {
            return 0;
        }
    }

    if ( Math::FAbs(kD.z) <= fEpsilon )
    {
        // line is perpendicular to axis of cylinder
        if ( Math::FAbs(kP.z) > fHalfHeight )
        {
            // line is outside the planar caps of cylinder
            return 0;
        }

        fA = kD.x*kD.x + kD.y*kD.y;
        fB = kP.x*kD.x + kP.y*kD.y;
        fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
        fDiscr = fB*fB - fA*fC;
        if ( fDiscr < 0.0f )
        {
            // line does not intersect cylinder wall
            return 0;
        }
        else if ( fDiscr > 0.0f )
        {
            fRoot = Math::Sqrt(fDiscr);
            fTmp0 = fInvDLength/fA;
            afT[0] = (-fB - fRoot)*fTmp0;
            afT[1] = (-fB + fRoot)*fTmp0;
            return 2;
        }
        else
        {
            afT[0] = -fB*fInvDLength/fA;
            return 1;
        }
    }

    // test plane intersections first
    int iQuantity = 0;
    fInv = 1.0f/kD.z;
    fT0 = (+fHalfHeight - kP.z)*fInv;
    fTmp0 = kP.x + fT0*kD.x;
    fTmp1 = kP.y + fT0*kD.y;
    if ( fTmp0*fTmp0 + fTmp1*fTmp1 <= fRadiusSqr )
        afT[iQuantity++] = fT0*fInvDLength;

    fT1 = (-fHalfHeight - kP.z)*fInv;
    fTmp0 = kP.x + fT1*kD.x;
    fTmp1 = kP.y + fT1*kD.y;
    if ( fTmp0*fTmp0 + fTmp1*fTmp1 <= fRadiusSqr )
        afT[iQuantity++] = fT1*fInvDLength;

    if ( iQuantity == 2 )
    {
        // line intersects both top and bottom
        return 2;
    }

    // If iQuantity == 1, then line must intersect cylinder wall
    // somewhere between caps in a single point.  This case is detected
    // in the following code that tests for intersection between line and
    // cylinder wall.

    fA = kD.x*kD.x + kD.y*kD.y;
    fB = kP.x*kD.x + kP.y*kD.y;
    fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
    fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
    {
        // line does not intersect cylinder wall
        assert( iQuantity == 0 );
        return 0;
    }
    else if ( fDiscr > 0.0f )
    {
        fRoot = Math::Sqrt(fDiscr);
        fInv = 1.0f/fA;
        fT = (-fB - fRoot)*fInv;
        if ( fT0 <= fT1 )
        {
            if ( fT0 <= fT && fT <= fT1 )
                afT[iQuantity++] = fT*fInvDLength;
        }
        else
        {
            if ( fT1 <= fT && fT <= fT0 )
                afT[iQuantity++] = fT*fInvDLength;
        }

        if ( iQuantity == 2 )
        {
            // Line intersects one of top/bottom of cylinder and once on
            // cylinder wall.
            return 2;
        }

        fT = (-fB + fRoot)*fInv;
        if ( fT0 <= fT1 )
        {
            if ( fT0 <= fT && fT <= fT1 )
                afT[iQuantity++] = fT*fInvDLength;
        }
        else
        {
            if ( fT1 <= fT && fT <= fT0 )
                afT[iQuantity++] = fT*fInvDLength;
        }
    }
    else
    {
        fT = -fB/fA;
        if ( fT0 <= fT1 )
        {
            if ( fT0 <= fT && fT <= fT1 )
                afT[iQuantity++] = fT*fInvDLength;
        }
        else
        {
            if ( fT1 <= fT && fT <= fT0 )
                afT[iQuantity++] = fT*fInvDLength;
        }
    }

    return iQuantity;
}
//----------------------------------------------------------------------------
static int FindHollow (const Vector3& rkOrigin, const Vector3& rkDirection,
    const Cylinder& rkCylinder, Real afT[2])
{
    const Real fEpsilon = 1e-12f;

    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kU, kV, kW = rkCylinder.Direction();
    Vector3::GenerateOrthonormalBasis(kU,kV,kW);
    Vector3 kD(kU.Dot(rkDirection),kV.Dot(rkDirection),kW.Dot(rkDirection));
    Real fDLength = kD.Unitize();
    Real fInvDLength = 1.0f/fDLength;
    Vector3 kDiff = rkOrigin - rkCylinder.Center();
    Vector3 kP(kU.Dot(kDiff),kV.Dot(kDiff),kW.Dot(kDiff));
    Real fHalfHeight = 0.5f*rkCylinder.Height();
    Real fRadiusSqr = rkCylinder.Radius()*rkCylinder.Radius();

    Real fA, fB, fC, fDiscr, fRoot, fT;

    if ( Math::FAbs(kD.z) >= 1.0f - fEpsilon )
    {
        // line is parallel to cylinder axis
        if ( kP.x*kP.x + kP.y*kP.y != fRadiusSqr )
        {
            // line inside or outside the cylinder
            return 0;
        }
        else
        {
            // The line intersects the cylinder along a line segment on the
            // cylinder wall.  Compute the line parameters for the end points
            // of the segment.
            fT = fInvDLength/kD.z;
            afT[0] = (+fHalfHeight - kP.z)*fT;
            afT[1] = (-fHalfHeight - kP.z)*fT;
            return 2;
        }
    }

    if ( Math::FAbs(kD.z) <= fEpsilon )
    {
        // line is perpendicular to axis of cylinder
        if ( Math::FAbs(kP.z) > fHalfHeight )
        {
            // line is outside the planar caps of cylinder
            return 0;
        }

        fA = kD.x*kD.x + kD.y*kD.y;
        fB = kP.x*kD.x + kP.y*kD.y;
        fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
        fDiscr = fB*fB - fA*fC;
        if ( fDiscr < 0.0f )
        {
            // line does not intersect cylinder wall
            return 0;
        }
        else if ( fDiscr > 0.0f )
        {
            fRoot = Math::Sqrt(fDiscr);
            fT = fInvDLength/fA;
            afT[0] = (-fB - fRoot)*fT;
            afT[1] = (-fB + fRoot)*fT;
            return 2;
        }
        else
        {
            afT[0] = -fB*fInvDLength/fA;
            return 1;
        }
    }

    // Clip line to a segment that is between the two planes of the cylinder
    // end disks.
    Real fInv = 1.0f/kD.z;
    Real fT0 = (+fHalfHeight - kP.z)*fInv;
    Real fT1 = (-fHalfHeight - kP.z)*fInv;

    // Compute the intersections (if any) between the line and the infinite
    // cylinder.
    int iQuantity = 0;
    fA = kD.x*kD.x + kD.y*kD.y;
    fB = kP.x*kD.x + kP.y*kD.y;
    fC = kP.x*kP.x + kP.y*kP.y - fRadiusSqr;
    fDiscr = fB*fB - fA*fC;
    if ( fDiscr < 0.0f )
    {
        // line does not intersect infinite cylinder
        return 0;
    }
    else if ( fDiscr > 0.0f )
    {
        // Line intersects infinite cylinder in two points.  Only save the
        // line-parameters of intersection if those parameters are within the
        // clipped line.
        fRoot = Math::Sqrt(fDiscr);
        fInv = 1.0f/fA;

        fT = (-fB - fRoot)*fInv;
        if ( fT0 <= fT1 )
        {
            if ( fT0 <= fT && fT <= fT1 )
                afT[iQuantity++] = fT*fInvDLength;
        }
        else
        {
            if ( fT1 <= fT && fT <= fT0 )
                afT[iQuantity++] = fT*fInvDLength;
        }

        fT = (-fB + fRoot)*fInv;
        if ( fT0 <= fT1 )
        {
            if ( fT0 <= fT && fT <= fT1 )
                afT[iQuantity++] = fT*fInvDLength;
        }
        else
        {
            if ( fT1 <= fT && fT <= fT0 )
                afT[iQuantity++] = fT*fInvDLength;
        }
    }
    else
    {
        // Line intersects infinite cylinder in one point (line is tangent to
        // cylinder).  Only save the line-parameter of intersection if that
        // parameter is within the clipped line.
        fT = -fB/fA;
        if ( fT0 <= fT1 )
        {
            if ( fT0 <= fT && fT <= fT1 )
                afT[iQuantity++] = fT*fInvDLength;
        }
        else
        {
            if ( fT1 <= fT && fT <= fT0 )
                afT[iQuantity++] = fT*fInvDLength;
        }
    }

    return iQuantity;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment3& rkSegment,
    const Cylinder& rkCylinder, int& riQuantity, Vector3 akPoint[2])
{
    Real afT[2];

    if ( rkCylinder.Capped() )
    {
        riQuantity = Find(rkSegment.Origin(),rkSegment.Direction(),
            rkCylinder,afT);
    }
    else
    {
        riQuantity = FindHollow(rkSegment.Origin(),rkSegment.Direction(),
            rkCylinder,afT);
    }

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
bool Mgc::FindIntersection (const Ray3& rkRay, const Cylinder& rkCylinder,
    int& riQuantity, Vector3 akPoint[2])
{
    Real afT[2];

    if ( rkCylinder.Capped() )
    {
        riQuantity = Find(rkRay.Origin(),rkRay.Direction(),rkCylinder,afT);
    }
    else
    {
        riQuantity = FindHollow(rkRay.Origin(),rkRay.Direction(),
            rkCylinder,afT);
    }

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
bool Mgc::FindIntersection (const Line3& rkLine, const Cylinder& rkCylinder,
    int& riQuantity, Vector3 akPoint[2])
{
    Real afT[2];

    if ( rkCylinder.Capped() )
    {
        riQuantity = Find(rkLine.Origin(),rkLine.Direction(),rkCylinder,afT);
    }
    else
    {
        riQuantity = FindHollow(rkLine.Origin(),rkLine.Direction(),
            rkCylinder,afT);
    }

    for (int i = 0; i < riQuantity; i++)
        akPoint[i] = rkLine.Origin() + afT[i]*rkLine.Direction();

    return riQuantity > 0;
}
//----------------------------------------------------------------------------


