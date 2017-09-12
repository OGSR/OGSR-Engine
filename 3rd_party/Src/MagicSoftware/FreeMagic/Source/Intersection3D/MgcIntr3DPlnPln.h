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

#ifndef MGCINTR3DPLNPLN_H
#define MGCINTR3DPLNPLN_H

#include "MgcLine3.h"
#include "MgcPlane.h"

namespace Mgc {

MAGICFM bool TestIntersection (const Plane& rkPlane0, const Plane& rkPlane1);
MAGICFM bool FindIntersection (const Plane& rkPlane0, const Plane& rkPlane1,
    Line3& rkLine);

} // namespace Mgc

#endif


