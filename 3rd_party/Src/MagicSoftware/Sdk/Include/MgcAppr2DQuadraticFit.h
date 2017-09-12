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

#ifndef MGCAPPR2DQUADRATICFIT_H
#define MGCAPPR2DQUADRATICFIT_H

#include "MgcVector2.h"

// Quadratic fit is
//
//   0 = C[0] + C[1]*X + C[2]*Y + C[3]*X^2 + C[4]*Y^2 + C[5]*X*Y
//
// subject to Length(C) = 1.  Minimize E(C) = C^t M C with Length(C) = 1
// and M = (sum_i V_i)(sum_i V_i)^t where
//
//   V = (1, X, Y, X^2, Y^2, X*Y)
//         
// Minimum value is the smallest eigenvalue of M and C is a corresponding
// unit length eigenvector.
//
// Input:
//   n = number of points to fit
//   p[0..n-1] = array of points to fit
//
// Output:
//   c[0..5] = coefficients of quadratic fit (the eigenvector)
//   return value of function is nonnegative and a measure of the fit
//   (the minimum eigenvalue; 0 = exact fit, positive otherwise)

// Canonical forms.  The quadratic equation can be factored into
// P^T A P + B^T P + K = 0 where P = (X,Y,Z), K = C[0], B = (C[1],C[2],C[3]),
// and A is a 3x3 symmetric matrix with A00 = C[4], A11 = C[5], A22 = C[6],
// A01 = C[7]/2, A02 = C[8]/2, and A12 = C[9]/2.  Matrix A = R^T D R where
// R is orthogonal and D is diagonal (using an eigendecomposition).  Define
// V = R P = (v0,v1,v2), E = R B = (e0,e1,e2), D = diag(d0,d1,d2), and f = K
// to obtain
//
//   d0 v0^2 + d1 v1^2 + d2 v^2 + e0 v0 + e1 v1 + e2 v2 + f = 0
//
// Characterization depends on the signs of the d_i.

namespace Mgc {

MAGICFM Real QuadraticFit (int iQuantity, const Vector2* akPoint,
    Real afCoeff[6]);


// If you think your points are nearly circular, use this.  Circle is of
// form C'[0]+C'[1]*X+C'[2]*Y+C'[3]*(X^2+Y^2) where Length(C') = 1.
// Function returns C = (C'[0]/C'[3],C'[1]/C'[3],C'[2]/C'[3]), so fitted
// circle is C[0]+C[1]*X+C[2]*Y+X^2+Y^2.  Center is (xc,yc) =
// -0.5*(C[1],C[2]) and radius is rad = sqrt(xc*xc+yc*yc-C[0]).

MAGICFM Real QuadraticCircleFit (int iQuantity, const Vector2* akPoint,
    Vector2& rkCenter, Real& rfRadius);

} // namespace Mgc

#endif


