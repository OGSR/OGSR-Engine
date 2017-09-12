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

#ifndef MGCDIST2DVECELP_H
#define MGCDIST2DVECELP_H

// Input:   Ellipse (x/a)^2+(y/b)^2 = 1, point (u,v).
//          rkExtent = (a,b)
// Output:  Closest point (x,y) on ellipse to (u,v), function returns
//          the distance sqrt((x-u)^2+(y-v)^2).
//          rkPoint = (u,v), rkClosest = (x,y)
//
// Method sets up the distance as the maximum root to a fourth degree
// polynomial.  The root is found by Newton's method.  If the return value
// is -1, then the iterates failed to converge.

#include "MgcEllipse2.h"

namespace Mgc {

MAGICFM Real SqrDistance (const EllipseStandard2& rkEllipse,
    const Vector2& rkPoint, Vector2& rkClosest);

MAGICFM Real Distance (const EllipseStandard2& rkEllipse,
    const Vector2& rkPoint, Vector2& rkClosest);

} // namespace Mgc

#endif


