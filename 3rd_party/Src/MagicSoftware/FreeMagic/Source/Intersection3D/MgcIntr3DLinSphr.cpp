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

#include "MgcDist3DVecLin.h"
#include "MgcIntr3DLinSphr.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Segment3& rkSegment, const Sphere& rkSphere)
{
    Real fSqrDist = SqrDistance(rkSphere.Center(),rkSegment);
    return fSqrDist <= rkSphere.Radius()*rkSphere.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Ray3& rkRay, const Sphere& rkSphere)
{
    Real fSqrDist = SqrDistance(rkSphere.Center(),rkRay);
    return fSqrDist <= rkSphere.Radius()*rkSphere.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Line3& rkLine, const Sphere& rkSphere)
{
    Real fSqrDist = SqrDistance(rkSphere.Center(),rkLine);
    return fSqrDist <= rkSphere.Radius()*rkSphere.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment3& rkSegment, const Sphere& rkSphere,
    int& riQuantity, Vector3 akPoint[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkSegment.Origin() - rkSphere.Center();
    Real fA = rkSegment.Direction().SquaredLength();
    Real fB = kDiff.Dot(rkSegment.Direction());
    Real fC = kDiff.SquaredLength() -
        rkSphere.Radius()*rkSphere.Radius();

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
bool Mgc::FindIntersection (const Ray3& rkRay, const Sphere& rkSphere,
    int& riQuantity, Vector3 akPoint[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkRay.Origin() - rkSphere.Center();
    Real fA = rkRay.Direction().SquaredLength();
    Real fB = kDiff.Dot(rkRay.Direction());
    Real fC = kDiff.SquaredLength() -
        rkSphere.Radius()*rkSphere.Radius();

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
bool Mgc::FindIntersection (const Line3& rkLine, const Sphere& rkSphere,
    int& riQuantity, Vector3 akPoint[2])
{
    // set up quadratic Q(t) = a*t^2 + 2*b*t + c
    Vector3 kDiff = rkLine.Origin() - rkSphere.Center();
    Real fA = rkLine.Direction().SquaredLength();
    Real fB = kDiff.Dot(rkLine.Direction());
    Real fC = kDiff.SquaredLength() -
        rkSphere.Radius()*rkSphere.Radius();

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


