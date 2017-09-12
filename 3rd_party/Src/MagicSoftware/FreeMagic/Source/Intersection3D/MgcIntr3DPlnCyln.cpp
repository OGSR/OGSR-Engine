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

#include "MgcIntr3DPlnCyln.h"
using namespace Mgc;

static const Real gs_fEpsilon = 1e-06f;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Plane& rkPlane, const Cylinder& rkCylinder,
    bool bUnitNormal)
{
    Vector3 kNormal = rkPlane.Normal();
    Real fConstant = rkPlane.Constant();
    if ( !bUnitNormal )
    {
        Real fLength = kNormal.Unitize();
        fConstant /= fLength;
    }

    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = kNormal.Dot(rkCylinder.Center()) - fConstant;
    Real fAbsNdW = Math::FAbs(kNormal.Dot(rkCylinder.Direction()));
    Real fRoot = Math::Sqrt(Math::FAbs(1.0f - fAbsNdW*fAbsNdW));
    Real fTerm = rkCylinder.Radius()*fRoot + 0.5f*rkCylinder.Height()*fAbsNdW;

    // intersection occurs if and only if 0 is in the interval [min,max]
    return Math::FAbs(fSDist) <= fTerm;
}
//----------------------------------------------------------------------------
bool Mgc::Culled (const Plane& rkPlane, const Cylinder& rkCylinder,
    bool bUnitNormal)
{
    Vector3 kNormal = rkPlane.Normal();
    Real fConstant = rkPlane.Constant();
    if ( !bUnitNormal )
    {
        Real fLength = kNormal.Unitize();
        fConstant /= fLength;
    }

    // Compute extremes of signed distance Dot(N,X)-d for points on the
    // cylinder.  These are
    //   min = (Dot(N,C)-d) - r*sqrt(1-Dot(N,W)^2) - (h/2)*|Dot(N,W)|
    //   max = (Dot(N,C)-d) + r*sqrt(1-Dot(N,W)^2) + (h/2)*|Dot(N,W)|
    Real fSDist = kNormal.Dot(rkCylinder.Center()) - fConstant;
    Real fAbsNdW = Math::FAbs(kNormal.Dot(rkCylinder.Direction()));
    Real fRoot = Math::Sqrt(Math::FAbs(1.0f - fAbsNdW*fAbsNdW));
    Real fTerm = rkCylinder.Radius()*fRoot + 0.5f*rkCylinder.Height()*fAbsNdW;

    // culling occurs if and only if max <= 0
    return fSDist + fTerm <= 0.0f;
}
//----------------------------------------------------------------------------


