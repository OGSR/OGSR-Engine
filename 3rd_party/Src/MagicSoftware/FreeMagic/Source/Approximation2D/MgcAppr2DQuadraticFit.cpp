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

#include "MgcAppr2DQuadraticFit.h"
#include "MgcEigen.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::QuadraticFit (int iQuantity, const Vector2* akPoint,
    Real afCoeff[6])
{
    Eigen kES(6);
    int iRow, iCol;
    for (iRow = 0; iRow < 6; iRow++)
    {
        for (iCol = 0; iCol < 6; iCol++)
            kES.Matrix(iRow,iCol) = 0.0f;
    }

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX = akPoint[i].x;
        Real fY = akPoint[i].y;
        Real fX2 = fX*fX;
        Real fY2 = fY*fY;
        Real fXY = fX*fY;
        Real fX3 = fX*fX2;
        Real fXY2 = fX*fY2;
        Real fX2Y = fX*fXY;
        Real fY3 = fY*fY2;
        Real fX4 = fX*fX3;
        Real fX2Y2 = fX*fXY2;
        Real fX3Y = fX*fX2Y;
        Real fY4 = fY*fY3;
        Real fXY3 = fX*fY3;

        kES.Matrix(0,1) += fX;
        kES.Matrix(0,2) += fY;
        kES.Matrix(0,3) += fX2;
        kES.Matrix(0,4) += fY2;
        kES.Matrix(0,5) += fXY;
        kES.Matrix(1,3) += fX3;
        kES.Matrix(1,4) += fXY2;
        kES.Matrix(1,5) += fX2Y;
        kES.Matrix(2,4) += fY3;
        kES.Matrix(3,3) += fX4;
        kES.Matrix(3,4) += fX2Y2;
        kES.Matrix(3,5) += fX3Y;
        kES.Matrix(4,4) += fY4;
        kES.Matrix(4,5) += fXY3;
    }

    kES.Matrix(0,0) = Real(iQuantity);
    kES.Matrix(1,1) = kES.Matrix(0,3);
    kES.Matrix(1,2) = kES.Matrix(0,5);
    kES.Matrix(2,2) = kES.Matrix(0,4);
    kES.Matrix(2,3) = kES.Matrix(1,5);
    kES.Matrix(2,5) = kES.Matrix(1,4);
    kES.Matrix(5,5) = kES.Matrix(3,4);

    for (iRow = 0; iRow < 6; iRow++)
    {
        for (iCol = 0; iCol < iRow; iCol++)
            kES.Matrix(iRow,iCol) = kES.Matrix(iCol,iRow);
    }

    kES.IncrSortEigenStuffN();

    for (iRow = 0; iRow < 6; iRow++)
        afCoeff[iRow] = kES.GetEigenvector(iRow,0);

    // For exact fit, numeric round-off errors may make the minimum
    // eigenvalue just slightly negative.  Return absolute value since
    // application may rely on the return value being nonnegative.
    return Math::FAbs(kES.GetEigenvalue(0));
}
//----------------------------------------------------------------------------
Real Mgc::QuadraticCircleFit (int iQuantity, const Vector2* akPoint,
    Vector2& rkCenter, Real& rfRadius)
{
    Eigen kES(4);
    int iRow, iCol;
    for (iRow = 0; iRow < 4; iRow++)
    {
        for (iCol = 0; iCol < 4; iCol++)
            kES.Matrix(iRow,iCol) = 0.0f;
    }

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX = akPoint[i].x;
        Real fY = akPoint[i].y;
        Real fX2 = fX*fX;
        Real fY2 = fY*fY;
        Real fXY = fX*fY;
        Real fR2 = fX2+fY2;
        Real fXR2 = fX*fR2;
        Real fYR2 = fY*fR2;
        Real fR4 = fR2*fR2;

        kES.Matrix(0,1) += fX;
        kES.Matrix(0,2) += fY;
        kES.Matrix(0,3) += fR2;
        kES.Matrix(1,1) += fX2;
        kES.Matrix(1,2) += fXY;
        kES.Matrix(1,3) += fXR2;
        kES.Matrix(2,2) += fY2;
        kES.Matrix(2,3) += fYR2;
        kES.Matrix(3,3) += fR4;
    }

    kES.Matrix(0,0) = Real(iQuantity);

    for (iRow = 0; iRow < 4; iRow++)
    {
        for (iCol = 0; iCol < iRow; iCol++)
            kES.Matrix(iRow,iCol) = kES.Matrix(iCol,iRow);
    }

    kES.IncrSortEigenStuffN();

    Real fInv = 1.0f/kES.GetEigenvector(3,0);  // watch out for zero divide
    Real afCoeff[3];
    for (iRow = 0; iRow < 3; iRow++)
        afCoeff[iRow] = fInv*kES.GetEigenvector(iRow,0);

    rkCenter.x = -0.5f*afCoeff[1];
    rkCenter.y = -0.5f*afCoeff[2];
    rfRadius = Math::Sqrt(Math::FAbs(rkCenter.x*rkCenter.x +
        rkCenter.y*rkCenter.y - afCoeff[0]));

    // For exact fit, numeric round-off errors may make the minimum
    // eigenvalue just slightly negative.  Return absolute value since
    // application may rely on the return value being nonnegative.
    return Math::FAbs(kES.GetEigenvalue(0));
}
//----------------------------------------------------------------------------


