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


// Fitting a Paraboloid Uusing Least Squares
//
// Given a set of samples (x_i,y_i,z_i) for 0 <= i < N and assuming
// that the true values lie on a paraboloid
//     z = p0*x*x + p1*x*y + p2*y*y + p3*x + p4*y + p5  = Dot(P,Q(x,y))
// where P = (p0,p1,p2,p3,p4,p5) and Q(x,y) = (x*x,x*y,y*y,x,y,1),
// select P to minimize the sum of squared errors
//     E(P) = sum_{i=0}^{N-1} [Dot(P,Q_i)-z_i]^2
// where Q_i = Q(x_i,y_i).
//
// The minimum occurs when the gradient of E is the zero vector,
//     grad(E) = 2 sum_{i=0}^{N-1} [Dot(P,Q_i)-z_i] Q_i = 0
// Some algebra converts this to a system of 6 equations in 6 unknowns:
//     [(sum_{i=0}^{N-1} Q_i Q_i^t] P = sum_{i=0}^{N-1} z_i Q_i
// The product Q_i Q_i^t is a product of the 6x1 matrix Q_i with the
// 1x6 matrix Q_i^t, the result being a 6x6 matrix.
//
// Define the 6x6 symmetric matrix A = sum_{i=0}^{N-1} Q_i Q_i^t and the 6x1
// vector B = sum_{i=0}^{N-1} z_i Q_i.  The choice for P is the solution to
// the linear system of equations A*P = B.  The entries of A and B indicate
// summations over the appropriate product of variables.  For example,
// s(x^3 y) = sum_{i=0}^{N-1} x_i^3 y_i.
//
// +-                                                     -++  +   +-      -+
// | s(x^4) s(x^3 y)   s(x^2 y^2) s(x^3)   s(x^2 y) s(x^2) ||p0|   |s(z x^2)|
// |        s(x^2 y^2) s(x y^3)   s(x^2 y) s(x y^2) s(x y) ||p1|   |s(z x y)|
// |                   s(y^4)     s(x y^2) s(y^3)   s(y^2) ||p2| = |s(z y^2)|
// |                              s(x^2)   s(x y)   s(x)   ||p3|   |s(z x)  |
// |                                       s(y^2)   s(y)   ||p4|   |s(z y)  |
// |                                                s(1)   ||p5|   |s(z)    |
// +-                                                     -++  +   +-      -+

#include "MgcAppr3DParaboloidFit.h"
#include "MgcLinearSystem.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::ParaboloidFit (int iQuantity, const Vector3* akPoint,
    Real afCoeff[6])
{
    // allocate linear system (A and b are zeroed initially)
    Real** A = LinearSystem::NewMatrix(6);
    memset(afCoeff,0,6*sizeof(Real));

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX2 = akPoint[i].x*akPoint[i].x;
        Real fXY = akPoint[i].x*akPoint[i].y;
        Real fY2 = akPoint[i].y*akPoint[i].y;
        Real fZX = akPoint[i].z*akPoint[i].x;
        Real fZY = akPoint[i].z*akPoint[i].y;
        Real fX3 = akPoint[i].x*fX2;
        Real fX2Y = fX2*akPoint[i].y;
        Real fXY2 = akPoint[i].x*fY2;
        Real fY3 = akPoint[i].y*fY2;
        Real fZX2 = akPoint[i].z*fX2;
        Real fZXY = akPoint[i].z*fXY;
        Real fZY2 = akPoint[i].z*fY2;
        Real fX4 = fX2*fX2;
        Real fX3Y = fX3*akPoint[i].y;
        Real fX2Y2 = fX2*fY2;
        Real fXY3 = akPoint[i].x*fY3;
        Real fY4 = fY2*fY2;

        A[0][0] += fX4;
        A[0][1] += fX3Y;
        A[0][2] += fX2Y2;
        A[0][3] += fX3;
        A[0][4] += fX2Y;
        A[0][5] += fX2;
        A[1][2] += fXY3;
        A[1][4] += fXY2;
        A[1][5] += fXY;
        A[2][2] += fY4;
        A[2][4] += fY3;
        A[2][5] += fY2;
        A[3][3] += fX2;
        A[3][5] += akPoint[i].x;
        A[4][5] += akPoint[i].y;

        afCoeff[0] += fZX2;
        afCoeff[1] += fZXY;
        afCoeff[2] += fZY2;
        afCoeff[3] += fZX;
        afCoeff[4] += fZY;
        afCoeff[5] += akPoint[i].z;
    }

    A[1][0] = A[0][1];
    A[1][1] = A[0][2];
    A[1][3] = A[0][4];
    A[2][0] = A[0][2];
    A[2][1] = A[1][2];
    A[2][3] = A[1][4];
    A[3][0] = A[0][3];
    A[3][1] = A[1][3];
    A[3][2] = A[2][3];
    A[3][4] = A[1][5];
    A[4][0] = A[0][4];
    A[4][1] = A[1][4];
    A[4][2] = A[2][4];
    A[4][3] = A[3][4];
    A[4][4] = A[2][5];
    A[5][0] = A[0][5];
    A[5][1] = A[1][5];
    A[5][2] = A[2][5];
    A[5][3] = A[3][5];
    A[5][4] = A[4][5];
    A[5][5] = Real(iQuantity);

    bool bResult = LinearSystem::SolveSymmetric(6,A,afCoeff);
    LinearSystem::DeleteMatrix(6,A);

    return bResult;
}
//----------------------------------------------------------------------------


