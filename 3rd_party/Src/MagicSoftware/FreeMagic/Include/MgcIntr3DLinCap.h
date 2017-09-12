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

#ifndef MGCINTR3DLINCAP_H
#define MGCINTR3DLINCAP_H

#include "MgcCapsule.h"
#include "MgcLine3.h"
#include "MgcRay3.h"
#include "MgcSegment3.h"

namespace Mgc {

// return value is 'true' if and only if objects intersect

MAGICFM bool TestIntersection (const Segment3& rkSegment,
    const Capsule& rkCapsule);
MAGICFM bool TestIntersection (const Ray3& rkRay,
    const Capsule& rkCapsule);
MAGICFM bool TestIntersection (const Line3& rkLine,
    const Capsule& rkCapsule);

MAGICFM bool FindIntersection (const Segment3& rkSegment,
    const Capsule& rkCapsule, int& riQuantity, Vector3 akPoint[2]);
MAGICFM bool FindIntersection (const Ray3& rkRay,
    const Capsule& rkCapsule, int& riQuantity, Vector3 akPoint[2]);
MAGICFM bool FindIntersection (const Line3& rkLine,
    const Capsule& rkCapsule, int& riQuantity, Vector3 akPoint[2]);

// utility function

MAGICFM int Find (const Vector3& rkOrigin, const Vector3& rkDirection,
    const Capsule& rkCapsule, Real afT[2]);

} // namespace Mgc

#endif


