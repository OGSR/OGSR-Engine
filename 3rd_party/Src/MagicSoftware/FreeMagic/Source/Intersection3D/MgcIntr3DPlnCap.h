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

#ifndef MGCINTR3DPLNCAP_H
#define MGCINTR3DPLNCAP_H

#include "MgcCapsule.h"
#include "MgcPlane.h"

namespace Mgc {

// The boolean bUnitNormal is a hint about whether or not the plane normal
// is unit length.  If it is not, the length must be calculated by these
// routines.  For batch calls, the plane normal should be unitized in advance
// to avoid the expensive length calculation.

MAGICFM bool TestIntersection (const Plane& rkPlane, const Capsule& rkCapsule,
    bool bUnitNormal);

// Culling support.  View frustum is assumed to be on the positive side of
// the plane.  Capsule is culled if it is on the negative side.
MAGICFM bool Culled (const Plane& rkPlane, const Capsule& rkCapsule,
    bool bUnitNormal);

} // namespace Mgc

#endif


