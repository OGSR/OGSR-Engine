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

#ifndef MGCINTR2DCIRCIR_H
#define MGCINTR2DCIRCIR_H

#include "MgcArc2.h"
#include "MgcCircle2.h"

// The return value is 'true' if and only if the objects intersect.  The
// returned quantity is usually 0, 1, or 2 and is the number of valid points
// in akPoint[2] (the intersection points).  It is also possible that the
// two objects overlap in infinitely many points.  For the circle-circle case,
// the returned quantity is -1 and the first circle can be used as the
// intersection set.  The akPoint[] values are invalid.  For the circle-arc
// case, the returned quantity is -1 and the arc can be used as the
// intersection set.  The akPoint[] values are invalid.  For the arc-arc case,
// the returned quantity is
//    0: arcs do not overlap
//   -1: arcs just touch at a single point, akPoint[0]
//   -2: arcs overlap on an arc, <akPoint[0],akPoint[1]>
// The application must look at the sign of riQuantity when the function
// returns 'true' to trap these cases.

namespace Mgc {

MAGICFM bool FindIntersection (const Circle2& rkCircle0,
    const Circle2& rkCircle1, int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Circle2& rkCircle, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2]);

MAGICFM bool FindIntersection (const Arc2& rkArc0, const Arc2& rkArc1,
    int& riQuantity, Vector2 akPoint[2]);

} // namespace Mgc

#endif


