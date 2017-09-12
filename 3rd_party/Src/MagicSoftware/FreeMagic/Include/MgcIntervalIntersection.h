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

#ifndef MGCINTERVALINTERSECTION_H
#define MGCINTERVALINTERSECTION_H

#include "MgcMath.h"

namespace Mgc {

// Compute the intersection of [u0,u1] and [v0,v1] where u0 < u1 and
// v0 < v1.  Values of +MAX_REAL or -MAX_REAL are allowed.  The returned
// quantity is
//   0:  no intersection (w[0] and w[1] are invalid)
//   1:  intersection contains one point (w[0] is either u0 or u1)
//   2:  intersection is an interval, [w[0],w[1]]
MAGICFM void FindIntersection (Real fU0, Real fU1, Real fV0, Real fV1,
    int& riQuantity, Real& rfW0, Real& rfW1);

// The intersection is stored in-place (for convenience in calling).
MAGICFM int FindIntersection (Real fU0, Real fU1, Real& rfV0, Real& rfV1);

} // namespace Mgc

#endif


