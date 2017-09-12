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

#ifndef MGCCONT2DELLIPSE_H
#define MGCCONT2DELLIPSE_H

// The input points are fit with a Gaussian distribution.  The center C of the
// ellipse is chosen to be the mean of the distribution.  The axes of the
// ellipse are chosen to be the eigenvectors of the covariance matrix M.  The
// returned ellipse is of the form (X-C)^T*(M^{-1}/V)*(X-C) = 1 for an
// appropriately chosen V > 0.
//
// WARNING.  The construction is ill-conditioned if the points are (nearly)
// collinear.  In this case M has a (nearly) zero eigenvalue, so inverting M
// is problematic.

#include "MgcEllipse2.h"

namespace Mgc {

MAGICFM Ellipse2 ContEllipse (int iQuantity, const Vector2* akPoint);

} // namespace Mgc

#endif


