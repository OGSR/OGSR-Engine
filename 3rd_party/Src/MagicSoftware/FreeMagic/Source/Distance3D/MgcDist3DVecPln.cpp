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

#include "MgcDist3DVecPln.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Vector3& rkNormal,
    const Vector3& rkOrigin, Vector3* pkClosest)
{
    Vector3 kDiff = rkPoint - rkOrigin;
    Real fSignedDist = rkNormal.Dot(kDiff);

    if ( pkClosest )
        *pkClosest = rkPoint - fSignedDist*rkNormal;

    return Math::FAbs(fSignedDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Vector3& rkNormal,
    Real fConstant, Vector3* pkClosest)
{
    Real fSignedDist = rkNormal.Dot(rkPoint) - fConstant;

    if ( pkClosest )
        *pkClosest = rkPoint - fSignedDist*rkNormal;

    return Math::FAbs(fSignedDist);
}
//----------------------------------------------------------------------------


