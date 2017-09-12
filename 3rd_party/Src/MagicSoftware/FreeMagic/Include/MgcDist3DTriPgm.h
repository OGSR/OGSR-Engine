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

#ifndef MGCDIST3DTRIPGM_H
#define MGCDIST3DTRIPGM_H

#include "MgcParallelogram3.h"
#include "MgcTriangle3.h"

namespace Mgc {

// squared distance measurements

MAGICFM Real SqrDistance (const Triangle3& rkTri,
    const Parallelogram3& rkPgm, Real* pfTriP0 = NULL, Real* pfTriP1 = NULL,
    Real* pfPgmP0 = NULL, Real* pfPgmP1 = NULL);

// distance measurements

MAGICFM Real Distance (const Triangle3& rkTri, const Parallelogram3& rkPgm,
    Real* pfTriP0 = NULL, Real* pfTriP1 = NULL, Real* pfPgmP0 = NULL,
    Real* pfPgmP1 = NULL);

} // namespace Mgc

#endif


