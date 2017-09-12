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

#ifndef MGCINTR3DLINSPHR_H
#define MGCINTR3DLINSPHR_H

#include "MgcLine3.h"
#include "MgcRay3.h"
#include "MgcSegment3.h"
#include "MgcSphere.h"

namespace Mgc {

// return value is 'true' if and only if objects intersect

MAGICFM bool TestIntersection (const Segment3& rkSegment,
    const Sphere& rkSphere);
MAGICFM bool TestIntersection (const Ray3& rkRay, const Sphere& rkSphere);
MAGICFM bool TestIntersection (const Line3& rkLine, const Sphere& rkSphere);

MAGICFM bool FindIntersection (const Segment3& rkSegment,
    const Sphere& rkSphere, int& riQuantity, Vector3 akPoint[2]);
MAGICFM bool FindIntersection (const Ray3& rkRay, const Sphere& rkSphere,
    int& riQuantity, Vector3 akPoint[2]);
MAGICFM bool FindIntersection (const Line3& rkLine, const Sphere& rkSphere,
    int& riQuantity, Vector3 akPoint[2]);

} // namespace Mgc

#endif


