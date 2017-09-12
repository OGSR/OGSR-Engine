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

#include "MgcIntr3DPlnBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Plane& rkPlane, const Box3& rkBox)
{
    Real fTmp[3] =
    {
        rkBox.Extent(0)*(rkPlane.Normal().Dot(rkBox.Axis(0))),
        rkBox.Extent(1)*(rkPlane.Normal().Dot(rkBox.Axis(1))),
        rkBox.Extent(2)*(rkPlane.Normal().Dot(rkBox.Axis(2)))
    };

    Real fRadius = Math::FAbs(fTmp[0]) + Math::FAbs(fTmp[1]) +
        Math::FAbs(fTmp[2]);

    Real fPseudoDistance = rkPlane.DistanceTo(rkBox.Center());
    return Math::FAbs(fPseudoDistance) <= fRadius;
}
//----------------------------------------------------------------------------
bool Mgc::Culled (const Plane& rkPlane, const Box3& rkBox)
{
    Real fTmp[3] =
    {
        rkBox.Extent(0)*(rkPlane.Normal().Dot(rkBox.Axis(0))),
        rkBox.Extent(1)*(rkPlane.Normal().Dot(rkBox.Axis(1))),
        rkBox.Extent(2)*(rkPlane.Normal().Dot(rkBox.Axis(2)))
    };

    Real fRadius = Math::FAbs(fTmp[0]) + Math::FAbs(fTmp[1]) +
        Math::FAbs(fTmp[2]);

    Real fPseudoDistance = rkPlane.DistanceTo(rkBox.Center());
    return fPseudoDistance <= -fRadius;
}
//----------------------------------------------------------------------------


