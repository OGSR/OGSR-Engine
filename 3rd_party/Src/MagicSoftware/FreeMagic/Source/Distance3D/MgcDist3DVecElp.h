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

#ifndef MGCDIST3DVECELP_H
#define MGCDIST3DVECELP_H

// Input:   Ellipsoid (x/a)^2+(y/b)^2+(z/c)^2 = 1, point (u,v,w).
//          rkExtent = (a,b,x)
// Output:  Closest point (x,y,z) on ellipsoid to (u,v,w), function returns
//          the distance sqrt((x-u)^2+(y-v)^2+(z-w)^2).
//          rkPoint = (u,v,w), rkClosest = (x,y,z)
//
// Method sets up the distance as the maximum root to a sixth degree
// polynomial.  The root is found by Newton's method.  If the return value
// is -1, then the iterates failed to converge.

#include "MgcEllipsoid.h"

namespace Mgc {

MAGICFM Real SqrDistance (const EllipsoidStandard& rkEllipsoid,
    const Vector3& rkPoint, Vector3& rkClosest);

MAGICFM Real Distance (const EllipsoidStandard& rkEllipsoid,
    const Vector3& rkPoint, Vector3& rkClosest);

} // namespace Mgc

#endif


