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

#ifndef MGCDIST3DVECPLN_H
#define MGCDIST3DVECPLN_H

// Squared distance from point to plane.  First function handles
// Dot(N,X-A) = NULL.  Second function handles planes Dot(N,X) = c.  The plane
// normal N must be unit length.

#include "MgcVector3.h"

namespace Mgc {

MAGICFM Real Distance (const Vector3& rkPoint, const Vector3& rkNormal,
    const Vector3& rkOrigin, Vector3* pkClosest = NULL);

MAGICFM Real Distance (const Vector3& rkPoint, const Vector3& rkNormal,
    Real fConstant, Vector3* pkClosest = NULL);

} // namespace Mgc

#endif


