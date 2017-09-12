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

#ifndef MGCDIST3DPGMPGM_H
#define MGCDIST3DPGMPGM_H

#include "MgcParallelogram3.h"

namespace Mgc {

// squared distance measurements

MAGICFM Real SqrDistance (const Parallelogram3& rkPgm0,
    const Parallelogram3& rkPgm1, Real* pfPgm0P0 = NULL,
    Real* pfPgm0P1 = NULL, Real* pfPgm1P0 = NULL, Real* pfPgm1P1 = NULL);

// distance measurements

MAGICFM Real Distance (const Parallelogram3& rkPgm0,
    const Parallelogram3& rkPgm1, Real* pfPgm0P0 = NULL,
    Real* pfPgm0P1 = NULL, Real* pfPgm1P0 = NULL, Real* pfPgm1P1 = NULL);

} // namespace Mgc

#endif


