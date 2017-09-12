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

#include "MgcCont2DEllipse.h"
#include "MgcAppr2DGaussPointsFit.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Ellipse2 Mgc::ContEllipse (int iQuantity, const Vector2* akPoint)
{
    Ellipse2 kEllipse;

    // Fit the points with a Gaussian distribution.  The covariance matrix
    // is M = D[0]*U[0]*U[0]^T+D[1]*U[1]*U[1]^T where D represents afD and
    // U represents akAxis.
    Vector2 akAxis[2];
    Real afD[2];
    GaussPointsFit(iQuantity,akPoint,kEllipse.Center(),akAxis,afD);

    // Grow the ellipse, while retaining its shape determined by the
    // covariance matrix, to enclose all the input points.  The quadratic form
    // that is used for the ellipse construction is
    //
    //   Q(X) = (X-C)^T*M*(X-C)
    //        = (X-C)^T*(D[0]*U[0]*U[0]^T+D[1]*U[1]*U[1]^T)*(X-C)
    //        = D[0]*Dot(U[0],X-C)^2 + D[1]*Dot(U[1],X-C)^2
    //
    // If the maximum value of Q(X[i]) for all input points is V^2, then a
    // bounding ellipse is Q(X) = V^2 since Q(X[i]) <= V^2 for all i.

    afD[0] = 1.0f/Math::FAbs(afD[0]);
    afD[1] = 1.0f/Math::FAbs(afD[1]);

    Real fMaxValue = 0.0f;
    for (int i = 0; i < iQuantity; i++)
    {
        Vector2 kDiff = akPoint[i] - kEllipse.Center();
        Real afDot[2] =
        {
            akAxis[0].Dot(kDiff),
            akAxis[1].Dot(kDiff)
        };

        Real fValue = afD[0]*afDot[0]*afDot[0] + afD[1]*afDot[1]*afDot[1];
        if ( fValue > fMaxValue )
            fMaxValue = fValue;
    }

    float fInv = 1.0f/fMaxValue;
    afD[0] *= fInv;
    afD[1] *= fInv;

    Matrix2 akTensor[2];
    Matrix2::TensorProduct(akAxis[0],akAxis[0],akTensor[0]);
    Matrix2::TensorProduct(akAxis[1],akAxis[1],akTensor[1]);
    kEllipse.A() = afD[0]*akTensor[0] + afD[1]*akTensor[1];

    return kEllipse;
}
//----------------------------------------------------------------------------


