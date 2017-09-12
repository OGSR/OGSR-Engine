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

#ifndef MGCINTR3DPLNBOX_H
#define MGCINTR3DPLNBOX_H

#include "MgcBox3.h"
#include "MgcPlane.h"

namespace Mgc {

MAGICFM bool TestIntersection (const Plane& rkPlane, const Box3& rkBox);

// Culling support.  View frustum is assumed to be on the positive side of
// the plane.  Box is culled if it is on the negative side.
MAGICFM bool Culled (const Plane& rkPlane, const Box3& rkBox);

} // namespace Mgc

#endif


