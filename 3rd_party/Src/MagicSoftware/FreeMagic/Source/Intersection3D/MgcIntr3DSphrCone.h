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

#ifndef MGCINTR3DSPHRCONE_H
#define MGCINTR3DSPHRCONE_H

#include "MgcCone3.h"
#include "MgcSphere.h"

namespace Mgc {

MAGICFM bool TestIntersection (const Sphere& rkSphere, const Cone3& rkCone);

// If an intersection occurs, the point of intersection closest to the cone
// vertex is returned.
MAGICFM bool FindIntersection (const Sphere& rkSphere, const Cone3& rkCone,
    Vector3& rkClosest);

} // namespace Mgc

#endif

