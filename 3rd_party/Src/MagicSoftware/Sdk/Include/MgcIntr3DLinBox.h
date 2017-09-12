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

#ifndef MGCINTR3DLINBOX_H
#define MGCINTR3DLINBOX_H

#include "MgcBox3.h"
#include "MgcLine3.h"
#include "MgcRay3.h"
#include "MgcSegment3.h"

namespace Mgc {


// return value is 'true' if and only if objects intersect

MAGICFM bool TestIntersection (const Segment3& rkSegment, const Box3& rkBox);
MAGICFM bool TestIntersection (const Ray3& rkRay, const Box3& rkBox);
MAGICFM bool TestIntersection (const Line3& rkLine, const Box3& rkBox);

// Clipping of a linear component 'origin'+t*'direction' against an
// axis-aligned box [-e0,e0]x[-e1,e1]x[-e2,e2] where 'extent'=(e0,e1,e2).
// The values of t0 and t1 must be set by the caller.  If the component is a
// segment, set t0 = 0 and t1 = 1.  If the component is a ray, set t0 = 0 and
// t1 = MAXREAL.  If the component is a line, set t0 = -MAXREAL and
// t1 = MAXREAL.  The values are (possibly) modified by the clipper.
MAGICFM bool FindIntersection (const Vector3& rkOrigin,
    const Vector3& rkDirection, const Real afExtent[3], Real& rfT0,
    Real& rfT1);

MAGICFM bool FindIntersection (const Segment3& rkSegment, const Box3& rkBox,
    int& riQuantity, Vector3 akPoint[2]);

MAGICFM bool FindIntersection (const Ray3& rkRay, const Box3& rkBox,
    int& riQuantity, Vector3 akPoint[2]);

MAGICFM bool FindIntersection (const Line3& rkLine, const Box3& rkBox,
    int& riQuantity, Vector3 akPoint[2]);

} // namespace Mgc

#endif


