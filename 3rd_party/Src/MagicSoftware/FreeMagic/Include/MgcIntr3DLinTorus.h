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

#ifndef MGCINTR3DLINTORUS_H
#define MGCINTR3DLINTORUS_H

#include "MgcRay3.h"
#include "MgcTorus.h"

namespace Mgc {

MAGICFM bool FindIntersection (const Ray3& rkRay, const Torus& rkTorus,
     Real& rfS, Real& rfT);

} // namespace Mgc

#endif


