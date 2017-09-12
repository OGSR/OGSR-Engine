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

#include "MgcAppr3DQuadraticFit.h"
#include "MgcEigen.h"
#include "MgcRTLib.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::QuadraticFit (int iQuantity, const Vector3* akPoint,
    Real afCoeff[10])
{
    Eigen kES(10);
    int iRow, iCol;
    for (iRow = 0; iRow < 10; iRow++)
    {
        for (iCol = 0; iCol < 10; iCol++)
            kES.Matrix(iRow,iCol) = 0.0f;
    }

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX = akPoint[i].x;
        Real fY = akPoint[i].y;
        Real fZ = akPoint[i].z;
        Real fX2 = fX*fX;
        Real fY2 = fY*fY;
        Real fZ2 = fZ*fZ;
        Real fXY = fX*fY;
        Real fXZ = fX*fZ;
        Real fYZ = fY*fZ;
        Real fX3 = fX*fX2;
        Real fXY2 = fX*fY2;
        Real fXZ2 = fX*fZ2;
        Real fX2Y = fX*fXY;
        Real fX2Z = fX*fXZ;
        Real fXYZ = fX*fY*fZ;
        Real fY3 = fY*fY2;
        Real fYZ2 = fY*fZ2;
        Real fY2Z = fY*fYZ;
        Real fZ3 = fZ*fZ2;
        Real fX4 = fX*fX3;
        Real fX2Y2 = fX*fXY2;
        Real fX2Z2 = fX*fXZ2;
        Real fX3Y = fX*fX2Y;
        Real fX3Z = fX*fX2Z;
        Real fX2YZ = fX*fXYZ;
        Real fY4 = fY*fY3;
        Real fY2Z2 = fY*fYZ2;
        Real fXY3 = fX*fY3;
        Real fXY2Z = fX*fY2Z;
        Real fY3Z = fY*fY2Z;
        Real fZ4 = fZ*fZ3;
        Real fXYZ2 = fX*fYZ2;
        Real fXZ3 = fX*fZ3;
        Real fYZ3 = fY*fZ3;

        kES.Matrix(0,1) += fX;
        kES.Matrix(0,2) += fY;
        kES.Matrix(0,3) += fZ;
        kES.Matrix(0,4) += fX2;
        kES.Matrix(0,5) += fY2;
        kES.Matrix(0,6) += fZ2;
        kES.Matrix(0,7) += fXY;
        kES.Matrix(0,8) += fXZ;
        kES.Matrix(0,9) += fYZ;
        kES.Matrix(1,4) += fX3;
        kES.Matrix(1,5) += fXY2;
        kES.Matrix(1,6) += fXZ2;
        kES.Matrix(1,7) += fX2Y;
        kES.Matrix(1,8) += fX2Z;
        kES.Matrix(1,9) += fXYZ;
        kES.Matrix(2,5) += fY3;
        kES.Matrix(2,6) += fYZ2;
        kES.Matrix(2,9) += fY2Z;
        kES.Matrix(3,6) += fZ3;
        kES.Matrix(4,4) += fX4;
        kES.Matrix(4,5) += fX2Y2;
        kES.Matrix(4,6) += fX2Z2;
        kES.Matrix(4,7) += fX3Y;
        kES.Matrix(4,8) += fX3Z;
        kES.Matrix(4,9) += fX2YZ;
        kES.Matrix(5,5) += fY4;
        kES.Matrix(5,6) += fY2Z2;
        kES.Matrix(5,7) += fXY3;
        kES.Matrix(5,8) += fXY2Z;
        kES.Matrix(5,9) += fY3Z;
        kES.Matrix(6,6) += fZ4;
        kES.Matrix(6,7) += fXYZ2;
        kES.Matrix(6,8) += fXZ3;
        kES.Matrix(6,9) += fYZ3;
        kES.Matrix(9,9) += fY2Z2;
    }

    kES.Matrix(0,0) = Real(iQuantity);
    kES.Matrix(1,1) = kES.Matrix(0,4);
    kES.Matrix(1,2) = kES.Matrix(0,7);
    kES.Matrix(1,3) = kES.Matrix(0,8);
    kES.Matrix(2,2) = kES.Matrix(0,5);
    kES.Matrix(2,3) = kES.Matrix(0,9);
    kES.Matrix(2,4) = kES.Matrix(1,7);
    kES.Matrix(2,7) = kES.Matrix(1,5);
    kES.Matrix(2,8) = kES.Matrix(1,9);
    kES.Matrix(3,3) = kES.Matrix(0,6);
    kES.Matrix(3,4) = kES.Matrix(1,8);
    kES.Matrix(3,5) = kES.Matrix(2,9);
    kES.Matrix(3,7) = kES.Matrix(1,9);
    kES.Matrix(3,8) = kES.Matrix(1,6);
    kES.Matrix(3,9) = kES.Matrix(2,6);
    kES.Matrix(7,7) = kES.Matrix(4,5);
    kES.Matrix(7,8) = kES.Matrix(4,9);
    kES.Matrix(7,9) = kES.Matrix(5,8);
    kES.Matrix(8,8) = kES.Matrix(4,6);
    kES.Matrix(8,9) = kES.Matrix(6,7);
    kES.Matrix(9,9) = kES.Matrix(5,6);

    for (iRow = 0; iRow < 10; iRow++)
    {
        for (iCol = 0; iCol < iRow; iCol++)
            kES.Matrix(iRow,iCol) = kES.Matrix(iCol,iRow);
    }

    kES.IncrSortEigenStuffN();

    for (iRow = 0; iRow < 10; iRow++)
        afCoeff[iRow] = kES.GetEigenvector(iRow,0);

    // For exact fit, numeric round-off errors may make the minimum
    // eigenvalue just slightly negative.  Return absolute value since
    // application may rely on the return value being nonnegative.
    return Math::FAbs(kES.GetEigenvalue(0));
}
//----------------------------------------------------------------------------
Real Mgc::QuadraticSphereFit (int iQuantity, const Vector3* akPoint,
    Vector3& rkCenter, Real& rfRadius)
{
    Eigen kES(5);
    int iRow, iCol;
    for (iRow = 0; iRow < 5; iRow++)
    {
        for (iCol = 0; iCol < 5; iCol++)
            kES.Matrix(iRow,iCol) = 0.0f;
    }

    for (int i = 0; i < iQuantity; i++)
    {
        Real fX = akPoint[i].x;
        Real fY = akPoint[i].y;
        Real fZ = akPoint[i].z;
        Real fX2 = fX*fX;
        Real fY2 = fY*fY;
        Real fZ2 = fZ*fZ;
        Real fXY = fX*fY;
        Real fXZ = fX*fZ;
        Real fYZ = fY*fZ;
        Real fR2 = fX2+fY2;
        Real fXR2 = fX*fR2;
        Real fYR2 = fY*fR2;
        Real fZR2 = fZ*fR2;
        Real fR4 = fR2*fR2;

        kES.Matrix(0,1) += fX;
        kES.Matrix(0,2) += fY;
        kES.Matrix(0,3) += fZ;
        kES.Matrix(0,4) += fR2;
        kES.Matrix(1,1) += fX2;
        kES.Matrix(1,2) += fXY;
        kES.Matrix(1,3) += fXZ;
        kES.Matrix(1,4) += fXR2;
        kES.Matrix(2,2) += fY2;
        kES.Matrix(2,3) += fYZ;
        kES.Matrix(2,4) += fYR2;
        kES.Matrix(3,3) += fZ2;
        kES.Matrix(3,4) += fZR2;
        kES.Matrix(4,4) += fR4;
    }

    kES.Matrix(0,0) = Real(iQuantity);

    for (iRow = 0; iRow < 5; iRow++)
    {
        for (iCol = 0; iCol < iRow; iCol++)
            kES.Matrix(iRow,iCol) = kES.Matrix(iCol,iRow);
    }

    kES.IncrSortEigenStuffN();

    Real fInv = 1.0f/kES.GetEigenvector(4,0);  // watch out for zero divide
    Real afCoeff[4];
    for (iRow = 0; iRow < 4; iRow++)
        afCoeff[iRow] = fInv*kES.GetEigenvector(iRow,0);

    rkCenter.x = -0.5f*afCoeff[1];
    rkCenter.y = -0.5f*afCoeff[2];
    rkCenter.z = -0.5f*afCoeff[3];
    rfRadius = Math::Sqrt(Math::FAbs(rkCenter.x*rkCenter.x +
        rkCenter.y*rkCenter.y + rkCenter.z*rkCenter.z - afCoeff[0]));

    // For exact fit, numeric round-off errors may make the minimum
    // eigenvalue just slightly negative.  Return absolute value since
    // application may rely on the return value being nonnegative.
    return Math::FAbs(kES.GetEigenvalue(0));
}
//----------------------------------------------------------------------------


