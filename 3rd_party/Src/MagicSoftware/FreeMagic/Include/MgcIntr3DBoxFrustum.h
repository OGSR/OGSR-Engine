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

#ifndef MGCINTR3DBOXFRUSTUM_H
#define MGCINTR3DBOXFRUSTUM_H

#include "MgcBox3.h"
#include "MgcFrustum.h"

namespace Mgc {

MAGICFM bool TestIntersection (const Box3& rkBox, const Frustum& rkFrustum);

} // namespace Mgc

#endif


