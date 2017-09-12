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

#ifndef MGCINTR3DLINCONE_H
#define MGCINTR3DLINCONE_H

#include "MgcCone3.h"
#include "MgcLine3.h"

namespace Mgc {

// The cone is assumed to have an acute angle between cone axis and cone edge.
// The return value is 'true' if and only if there is an intersection.  If
// there is an intersection, the number of intersections is stored in
// ruiQuantity.

// It is possible half the line is entirely on the cone surface.  In this
// case, the riQuantity is set to -1 and akPoint[] values are unassigned (the
// ray of intersection is V+(t*Dot(D,A))*D where V is the cone vertex, D is
// the line direction, and t >= 0.
MAGICFM bool FindIntersection (const Line3& rkLine, const Cone3& rkCone,
    int& riQuantity, Vector3 akPoint[2]);

} // namespace Mgc

#endif


