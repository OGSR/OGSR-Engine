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

#ifndef MGCINTR3DTRICONE_H
#define MGCINTR3DTRICONE_H

#include "MgcCone3.h"
#include "MgcTriangle3.h"

namespace Mgc {

// The cone is assumed to have an acute angle between cone axis and cone edge.
// The return value is 'true' if and only if there is an intersection.

MAGICFM bool TestIntersection (const Triangle3& rkTri, const Cone3& rkCone);

} // namespace Mgc

#endif

