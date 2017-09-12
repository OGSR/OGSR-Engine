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

#ifndef MGCDIST3DVECBOX_H
#define MGCDIST3DVECBOX_H

#include "MgcBox3.h"

namespace Mgc {

// Use these for oriented bounding boxes.  If you provide three non-null
// pointers pfBParam0, pfBParam1, pfBParam2, both functions also return the
// closest point in box coordinates.  That is, if the box has center C and
// axes U0, U1, and U2, the closest point to the input point is
//   K = C + (*pfBParam0)*U0 + (*pfBParam1)*U1 + (*pfBParam2)*U2;

MAGICFM Real SqrDistance (const Vector3& rkPoint, const Box3& rkBox,
    Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

MAGICFM Real Distance (const Vector3& rkPoint, const Box3& rkBox,
    Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

// Use these for axis-aligned bounding boxes.  A point is in the box whenever
// xmin <= x <= xmax and ymin <= y <= ymax and zmin <= z <= zmax.  If you
// provide a non-null pointer pkClosest, both functions also return the
// closest point.

MAGICFM Real SqrDistance (const Vector3& rkPoint, float fXMin, float fXMax,
    float fYMin, float fYMax, float fZMin, float fZMax,
    Vector3* pkClosest = NULL);

MAGICFM Real Distance (const Vector3& rkPoint, float fXMin, float fXMax,
    float fYMin, float fYMax, float fZMin, float fZMax,
    Vector3* pkClosest = NULL);

} // namespace Mgc

#endif


