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

#ifndef MGCAPPR2DCIRCLEFIT_H
#define MGCAPPR2DCIRCLEFIT_H

// Least-squares fit of a circle to a set of points.  Successful fit is
// indicated by return value of 'true'.  If return value is false, number of
// iterations was exceeded.  Try increasing the maximum number inside the
// source code.

#include "MgcVector2.h"

namespace Mgc {

MAGICFM bool CircleFit (int iQuantity, const Vector2* akPoint,
    Vector2& rkCenter, Real& rfRadius);

} // namespace Mgc

#endif


