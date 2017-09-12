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

#include "MgcDist3DVecCir.h"
using namespace Mgc;

static const Real gs_fTolerance = 1e-05f;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Circle3& rkCircle,
    Vector3* pkClosest)
{
    // signed distance from point to plane of circle
    Vector3 kDiff0 = rkPoint - rkCircle.Center();
    Real fDist = kDiff0.Dot(rkCircle.N());

    // projection of P-C onto plane is Q-C = P-C - (fDist)*N
    Vector3 kDiff1 = kDiff0 - fDist*rkCircle.N();
    Real fSqrLen = kDiff1.SquaredLength();
    Vector3 kClosest;
    Real fSqrDist;

    if ( fSqrLen >= gs_fTolerance )
    {
        kClosest = rkCircle.Center() + (rkCircle.Radius()/
            Math::Sqrt(fSqrLen))*kDiff1;
        Vector3 kDiff2 = rkPoint - kClosest;
        fSqrDist = kDiff2.SquaredLength();
    }
    else
    {
        kClosest = Vector3(Math::MAX_REAL,Math::MAX_REAL,Math::MAX_REAL);
        fSqrDist = rkCircle.Radius()*rkCircle.Radius()+fDist*fDist;
    }

    if ( pkClosest )
        *pkClosest = kClosest;

    return fSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Disk3& rkDisk,
    Vector3* pkClosest)
{
    // signed distance from point to plane of rkDisk
    Vector3 kDiff0 = rkPoint - rkDisk.Center();
    Real fDist = kDiff0.Dot(rkDisk.N());

    // projection of P-C onto plane is Q-C = P-C - (fDist)*N
    Vector3 kDiff1 = kDiff0 - fDist*rkDisk.N();
    Real fSqrLen = kDiff1.SquaredLength();
    Vector3 kClosest;
    Real fSqrDist;

    if ( fSqrLen <= rkDisk.Radius()*rkDisk.Radius() )
    {
        // projected point Q is in disk and is closest to P
        kClosest = rkDisk.Center() + kDiff1;
        fSqrDist = fDist*fDist;
    }
    else
    {
        // projected point Q is outside disk, closest point is on circle
        kClosest = rkDisk.Center() + (rkDisk.Radius()/Math::Sqrt(fSqrLen))*
            kDiff1;
        Vector3 kDiff2 = rkPoint - kClosest;
        fSqrDist = kDiff2.SquaredLength();
    }

    if ( pkClosest )
        *pkClosest = kClosest;

    return fSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Circle3& rkCircle,
    Vector3* pkClosest)
{
    return Math::Sqrt(SqrDistance(rkPoint,rkCircle,pkClosest));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Disk3& rkDisk,
    Vector3* pkClosest)
{
    return Math::Sqrt(SqrDistance(rkPoint,rkDisk,pkClosest));
}
//----------------------------------------------------------------------------


