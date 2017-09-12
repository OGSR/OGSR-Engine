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

#include "MgcIntr3DPlnCap.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Plane& rkPlane, const Capsule& rkCapsule,
    bool bUnitNormal)
{
    Vector3 kNormal = rkPlane.Normal();
    Real fConstant = rkPlane.Constant();
    if ( !bUnitNormal )
    {
        Real fLength = kNormal.Unitize();
        fConstant /= fLength;
    }

    Real fTmp0 = kNormal.Dot(rkCapsule.Origin()) - fConstant;
    Real fTmp1 = fTmp0 + kNormal.Dot(rkCapsule.Direction());
    if ( fTmp0*fTmp1 <= 0.0f )
    {
        // capsule ends on opposite sides of the plane
        return true;
    }
    else
    {
        return Math::FAbs(fTmp0) <= rkCapsule.Radius()
            || Math::FAbs(fTmp1) <= rkCapsule.Radius();
    }
}
//----------------------------------------------------------------------------
bool Mgc::Culled (const Plane& rkPlane, const Capsule& rkCapsule,
    bool bUnitNormal)
{
    Vector3 kNormal = rkPlane.Normal();
    Real fConstant = rkPlane.Constant();
    if ( !bUnitNormal )
    {
        Real fLength = kNormal.Unitize();
        fConstant /= fLength;
    }

    Real fTmp0 = kNormal.Dot(rkCapsule.Origin()) - fConstant;
    if ( fTmp0 < 0.0f )
    {
        Real fTmp1 = fTmp0 + kNormal.Dot(rkCapsule.Direction());
        if ( fTmp1 < 0.0f )
        {
            if ( fTmp0 <= fTmp1 )
                return fTmp0 <= -rkCapsule.Radius();
            else
                return fTmp1 <= -rkCapsule.Radius();
        }
    }

    return false;
}
//----------------------------------------------------------------------------


