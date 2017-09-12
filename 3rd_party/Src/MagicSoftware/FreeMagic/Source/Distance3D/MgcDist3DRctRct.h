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

#ifndef MGCDIST3DRCTRCT_H
#define MGCDIST3DRCTRCT_H

#include "MgcRectangle3.h"

namespace Mgc {

// squared distance measurements

MAGICFM Real SqrDistance (const Rectangle3& rkRct0, const Rectangle3& rkRct1,
    Real* pfRct0P0 = NULL, Real* pfRct0P1 = NULL, Real* pfRct1P0 = NULL,
    Real* pfRct1P1 = NULL);

// distance measurements

MAGICFM Real Distance (const Rectangle3& rkRct0, const Rectangle3& rkRct1,
    Real* pfRct0P0 = NULL, Real* pfRct0P1 = NULL, Real* pfRct1P0 = NULL,
    Real* pfRct1P1 = NULL);

} // namespace Mgc

#endif


