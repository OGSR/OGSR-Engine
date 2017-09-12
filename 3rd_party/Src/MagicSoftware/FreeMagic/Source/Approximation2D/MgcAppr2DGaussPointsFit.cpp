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

#include "MgcEigen.h"
#include "MgcAppr2DGaussPointsFit.h"
using namespace Mgc;

//----------------------------------------------------------------------------
void Mgc::GaussPointsFit (int iQuantity, const Vector2* akPoint,
    Vector2& rkCenter, Vector2 akAxis[2], Real afExtent[2])
{
    // compute mean of points
    rkCenter = akPoint[0];
    int i;
    for (i = 1; i < iQuantity; i++)
        rkCenter += akPoint[i];
    Real fInvQuantity = 1.0f/iQuantity;
    rkCenter *= fInvQuantity;

    // compute covariances of points
    Real fSumXX = 0.0f, fSumXY = 0.0f, fSumYY = 0.0f;
    for (i = 0; i < iQuantity; i++)
    {
        Vector2 kDiff = akPoint[i] - rkCenter;
        fSumXX += kDiff.x*kDiff.x;
        fSumXY += kDiff.x*kDiff.y;
        fSumYY += kDiff.y*kDiff.y;
    }
    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumYY *= fInvQuantity;

    // solve eigensystem of covariance matrix
    Eigen kES(2);
    kES.Matrix(0,0) = fSumXX;
    kES.Matrix(0,1) = fSumXY;
    kES.Matrix(1,0) = fSumXY;
    kES.Matrix(1,1) = fSumYY;
    kES.IncrSortEigenStuff2();

    akAxis[0].x = kES.GetEigenvector(0,0);
    akAxis[0].y = kES.GetEigenvector(1,0);
    akAxis[1].x = kES.GetEigenvector(0,1);
    akAxis[1].y = kES.GetEigenvector(1,1);

    afExtent[0] = kES.GetEigenvalue(0);
    afExtent[1] = kES.GetEigenvalue(1);
}
//----------------------------------------------------------------------------
bool Mgc::GaussPointsFit (int iQuantity, const Vector2* akPoint,
    const bool* abValid, Vector2& rkCenter, Vector2 akAxis[2],
    Real afExtent[2])
{
    // compute mean of points
    rkCenter = Vector2::ZERO;
    int i, iValidQuantity = 0;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            rkCenter += akPoint[i];
            iValidQuantity++;
        }
    }
    if ( iValidQuantity == 0 )
        return false;

    Real fInvQuantity = 1.0f/iValidQuantity;
    rkCenter *= fInvQuantity;

    // compute covariances of points
    Real fSumXX = 0.0f, fSumXY = 0.0f, fSumYY = 0.0f;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            Vector2 kDiff = akPoint[i] - rkCenter;
            fSumXX += kDiff.x*kDiff.x;
            fSumXY += kDiff.x*kDiff.y;
            fSumYY += kDiff.y*kDiff.y;
        }
    }
    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumYY *= fInvQuantity;

    // solve eigensystem of covariance matrix
    Eigen kES(2);
    kES.Matrix(0,0) = fSumXX;
    kES.Matrix(0,1) = fSumXY;
    kES.Matrix(1,0) = fSumXY;
    kES.Matrix(1,1) = fSumYY;
    kES.IncrSortEigenStuff2();

    akAxis[0].x = kES.GetEigenvector(0,0);
    akAxis[0].y = kES.GetEigenvector(1,0);
    akAxis[1].x = kES.GetEigenvector(0,1);
    akAxis[1].y = kES.GetEigenvector(1,1);

    afExtent[0] = kES.GetEigenvalue(0);
    afExtent[1] = kES.GetEigenvalue(1);

    return true;
}
//----------------------------------------------------------------------------


