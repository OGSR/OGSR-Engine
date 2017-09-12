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

#ifndef MGCINTR3DLINCYLN_H
#define MGCINTR3DLINCYLN_H

#include "MgcCylinder.h"
#include "MgcLine3.h"
#include "MgcRay3.h"
#include "MgcSegment3.h"

namespace Mgc {

// These intersection routines are for bounded cylinders, that is, those for
// which the height is finite.  The return value is 'true' if and only if
// the objects intersect.  The caller should make sure that the cylinder is
// appropriately tagged as capped or hollow.  This is accomplished by using
// the Cylinder::Capped member function.

MAGICFM bool FindIntersection (const Segment3& rkSegment,
    const Cylinder& rkCylinder, int& riQuantity, Vector3 akPoint[2]);
MAGICFM bool FindIntersection (const Ray3& rkRay,
    const Cylinder& rkCylinder, int& riQuantity, Vector3 akPoint[2]);
MAGICFM bool FindIntersection (const Line3& rkLine,
    const Cylinder& rkCylinder, int& riQuantity, Vector3 akPoint[2]);

} // namespace Mgc

#endif


