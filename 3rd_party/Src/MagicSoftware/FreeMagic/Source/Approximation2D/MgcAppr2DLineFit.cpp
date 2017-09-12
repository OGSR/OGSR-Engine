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

#include "MgcAppr2DLineFit.h"
#include "MgcEigen.h"
#include "MgcLinearSystem.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::HeightLineFit (int iQuantity, const Vector2* akPoint, Real& rfA,
    Real& rfB)
{
    // You need at least two points to determine the line.  Even so, if
    // the points are on a vertical line, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix.
    if ( iQuantity < 2 )
        return false;

    // compute sums for linear system
    Real fSumX = 0.0f, fSumY = 0.0f, fSumXX = 0.0f, fSumXY = 0.0f;
    for (int i = 0; i < iQuantity; i++)
    {
        fSumX += akPoint[i].x;
        fSumY += akPoint[i].y;
        fSumXX += akPoint[i].x*akPoint[i].x;
        fSumXY += akPoint[i].x*akPoint[i].y;
    }

    Real aafA[2][2] =
    {
        fSumXX, fSumX,
        fSumX,  Real(iQuantity)
    };

    Real afB[2] =
    {
        fSumXY,
        fSumY
    };

    Real afX[2];

    bool bNonsingular = LinearSystem::Solve2(aafA,afB,afX);
    if ( bNonsingular )
    {
        rfA = afX[0];
        rfB = afX[1];
    }
    else
    {
        rfA = Math::MAX_REAL;
        rfB = Math::MAX_REAL;
    }

    return bNonsingular;
}
//----------------------------------------------------------------------------
void Mgc::OrthogonalLineFit (int iQuantity, const Vector2* akPoint,
    Vector2& rkOffset, Vector2& rkDirection)
{
    // compute average of points
    rkOffset = akPoint[0];
    int i;
    for (i = 1; i < iQuantity; i++)
        rkOffset += akPoint[i];
    Real fInvQuantity = 1.0f/iQuantity;
    rkOffset *= fInvQuantity;

    // compute sums of products
    Real fSumXX = 0.0f, fSumXY = 0.0f, fSumYY = 0.0f;
    for (i = 0; i < iQuantity; i++) 
    {
        Vector2 kDiff = akPoint[i] - rkOffset;
        fSumXX += kDiff.x*kDiff.x;
        fSumXY += kDiff.x*kDiff.y;
        fSumYY += kDiff.y*kDiff.y;
    }

    // setup the eigensolver
    Eigen kES(2);
    kES.Matrix(0,0) = fSumYY;
    kES.Matrix(0,1) = -fSumXY;
    kES.Matrix(1,0) = kES.Matrix(0,1);
    kES.Matrix(1,1) = fSumXX;

    // compute eigenstuff, smallest eigenvalue is in last position
    kES.DecrSortEigenStuff2();

    // unit-length direction for best-fit line
    rkDirection.x = kES.GetEigenvector(0,1);
    rkDirection.y = kES.GetEigenvector(1,1);
}
//----------------------------------------------------------------------------
bool Mgc::OrthogonalLineFit (int iQuantity, const Vector2* akPoint,
    const bool* abValid, Vector2& rkOffset, Vector2& rkDirection)
{
    // compute average of points
    rkOffset = Vector2::ZERO;
    int i, iValidQuantity = 0;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            rkOffset += akPoint[i];
            iValidQuantity++;
        }
    }
    if ( iValidQuantity == 0 )
        return false;

    Real fInvQuantity = 1.0f/iValidQuantity;
    rkOffset *= fInvQuantity;

    // compute sums of products
    Real fSumXX = 0.0f, fSumXY = 0.0f, fSumYY = 0.0f;
    for (i = 0; i < iQuantity; i++) 
    {
        if ( abValid[i] )
        {
            Vector2 kDiff = akPoint[i] - rkOffset;
            fSumXX += kDiff.x*kDiff.x;
            fSumXY += kDiff.x*kDiff.y;
            fSumYY += kDiff.y*kDiff.y;
        }
    }

    // setup the eigensolver
    Eigen kES(2);
    kES.Matrix(0,0) = fSumYY;
    kES.Matrix(0,1) = -fSumXY;
    kES.Matrix(1,0) = kES.Matrix(0,1);
    kES.Matrix(1,1) = fSumXX;

    // compute eigenstuff, smallest eigenvalue is in last position
    kES.DecrSortEigenStuff2();

    // unit-length direction for best-fit line
    rkDirection.x = kES.GetEigenvector(0,1);
    rkDirection.y = kES.GetEigenvector(1,1);

    return true;
}
//----------------------------------------------------------------------------


