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

#ifndef MGCINTR2DTRITRI_H
#define MGCINTR2DTRITRI_H

#include "MgcVector2.h"

namespace Mgc {

// The vertices are assumed to be counterclockwise ordered.  The functions
// return 'true' if and only if there is at least one point of intersection.
// Optimization of this code for your applications clearly depends on how you
// represent your triangles and what precomputed information is stored in that
// representation.

// The following two functions are for stationary triangles.

MAGICFM bool TestIntersection (const Vector2 akV0[3], const Vector2 akV1[3]);

// If the return value is true, then riQuantity > 0 and is the number of
// vertices of the polygon of intersection (at most a hexagon).  The vertices
// are stored in akVertex[0] through akVertex[riQuantity-1] and are in
// counterclockwise order.
MAGICFM bool FindIntersection (const Vector2 akV0[3], const Vector2 akV1[3],
    int& riQuantity, Vector2 akVertex[6]);

// The following two functions are for triangles moving with velocities W0 and
// W1 (not necessarily unit length).  The time interval over which the
// intersection query is valid is [0,TMax].  Set TMax = MAX_REAL if you just
// care if the triangles will intersect eventually.  If the function returns
// 'false', the values TFirst and TLast are not meaningful and should not be
// used.

MAGICFM bool TestIntersection (Real fTMax, const Vector2 akV0[3],
    const Vector2& rkW0, const Vector2 akV1[3], const Vector2& rkW1,
    Real& rfTFirst, Real& rfTLast);

MAGICFM bool FindIntersection (Real fTMax, const Vector2 akV0[3],
    const Vector2& rkW0, const Vector2 akV1[3], const Vector2& rkW1,
    Real& rfTFirst, Real& rfTLast, int& riQuantity, Vector2 akVertex[6]);

} // namespace Mgc

#endif


