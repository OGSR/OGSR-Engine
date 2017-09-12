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

#ifndef MGCAPPR3DPARABOLOIDFIT_H
#define MGCAPPR3DPARABOLOIDFIT_H

// Least-squares fit of a paraboloid to a set of point.  The paraboloid is
// of the form z = c0*x^2+c1*x*y+c2*y^2+c3*x+c4*y+c5.  Successful fit is
// indicated by return value of 'true'.  If return value is false, the
// internal linear system solver was unable to invert the system.

#include "MgcVector3.h"

namespace Mgc {

MAGICFM bool ParaboloidFit (int iQuantity, const Vector3* akPoint,
    Real afCoeff[6]);

} // namespace Mgc

#endif


