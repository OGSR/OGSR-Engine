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

#ifndef MGCINTR3DTRISPHR_H
#define MGCINTR3DTRISPHR_H

#include "MgcSphere.h"
#include "MgcTriangle3.h"

namespace Mgc {

// Determine if triangle transversely intersects sphere.  Return value is
// 'true' if and only if they intersect.  The Function does not indicate an
// intersection if one or more vertices are the only points of intersection
// or if the triangle is tangent to the sphere.

MAGICFM bool TestIntersection (const Triangle3& rkT, const Sphere& rkS);

} // namespace Mgc

#endif

