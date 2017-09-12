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

#include "MgcMatrix3.h"
#include "MgcParametricSurface.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Vector3 ParametricSurface::GetTangent0 (Real fU, Real fV) const
{
    Vector3 kTangent0 = GetDerivativeU(fU,fV);
    kTangent0.Unitize();
    return kTangent0;
}
//----------------------------------------------------------------------------
Vector3 ParametricSurface::GetTangent1 (Real fU, Real fV) const
{
    Vector3 kTangent1 = GetDerivativeV(fU,fV);
    kTangent1.Unitize();
    return kTangent1;
}
//----------------------------------------------------------------------------
Vector3 ParametricSurface::GetNormal (Real fU, Real fV) const
{
    Vector3 kTangent0 = GetDerivativeU(fU,fV);
    Vector3 kTangent1 = GetDerivativeV(fU,fV);
    Vector3 kNormal = kTangent0.UnitCross(kTangent1);
    return kNormal;
}
//----------------------------------------------------------------------------
void ParametricSurface::GetFrame (Real fU, Real fV, Vector3& kPosition,
    Vector3& kTangent0, Vector3& kTangent1, Vector3& kNormal) const
{
    kPosition = GetPosition(fU,fV);
    kTangent0 = GetDerivativeU(fU,fV);
    kTangent1 = GetDerivativeV(fU,fV);
    kTangent0.Unitize();
    kTangent1.Unitize();
    kNormal = kTangent0.UnitCross(kTangent1);
}
//----------------------------------------------------------------------------
void ParametricSurface::ComputePrincipalCurvatureInfo (Real fU, Real fV,
    Real& rfCurv0, Real& rfCurv1, Vector3& rkDir0, Vector3& rkDir1)
{
    // Tangents:  U = (x_s,y_s,z_s), V = (x_t,y_t,z_t)
    // Normal:    N = Cross(U,V)/Length(Cross(U,V))
    // Metric Tensor:    G = +-                  -+
    //                       | Dot(U,U)  Dot(U,V) |
    //                       | Dot(V,U)  Dot(V,V) |
    //                       +-                  -+
    //
    // Curvature Tensor:  B = +-                        -+
    //                        | -Dot(N,U_s)  -Dot(N,U_t) |
    //                        | -Dot(N,V_s)  -Dot(N,V_t) |
    //                        +-                        -+
    //
    // Principal curvatures k are the generalized eigenvalues of
    //
    //     Bw = kGw
    //
    // If k is a curvature and w=(a,b) is the corresponding solution to
    // Bw=kGw, then the principal direction as a 3D vector is d = a*U+b*V.
    //
    // Let k1 and k2 be the principal curvatures.  The mean curvature
    // is (k1+k2)/2 and the Gaussian curvature is k1*k2.

    // derivatives
    Vector3 kDerU = GetDerivativeU(fU,fV);
    Vector3 kDerV = GetDerivativeV(fU,fV);
    Vector3 kDerUU = GetDerivativeUU(fU,fV);
    Vector3 kDerUV = GetDerivativeUV(fU,fV);
    Vector3 kDerVV = GetDerivativeVV(fU,fV);

    // metric tensor
    Matrix3 kMetricTensor;
    kMetricTensor[0][0] = kDerU.Dot(kDerU);
    kMetricTensor[0][1] = kDerU.Dot(kDerV);
    kMetricTensor[1][0] = kMetricTensor[1][0];
    kMetricTensor[1][1] = kDerV.Dot(kDerV);

    // curvature tensor
    Vector3 kNormal = kDerU.UnitCross(kDerV);
    Matrix3 kCurvatureTensor;
    kCurvatureTensor[0][0] = -kNormal.Dot(kDerUU);
    kCurvatureTensor[0][1] = -kNormal.Dot(kDerUV);
    kCurvatureTensor[1][0] = kCurvatureTensor[0][1];
    kCurvatureTensor[1][1] = -kNormal.Dot(kDerVV);

    // characteristic polynomial is 0 = det(B-kG) = c2*k^2+c1*k+c0
    Real fC0 = kCurvatureTensor.Determinant();
    Real fC1 = 2.0f*kCurvatureTensor[0][1]* kMetricTensor[0][1] -
        kCurvatureTensor[0][0]*kMetricTensor[1][1] -
        kCurvatureTensor[1][1]*kMetricTensor[0][0];
    Real fC2 = kMetricTensor.Determinant();

    // principal curvatures are roots of characteristic polynomial
    Real fTemp = Math::Sqrt(Math::FAbs(fC1*fC1-4.0f*fC0*fC2));
    rfCurv0 = -0.5f*(fC1+fTemp);
    rfCurv1 = 0.5f*(-fC1+fTemp);

    // principal directions are solutions to (B-kG)w = 0
    // w1 = (b12-k1*g12,-(b11-k1*g11)) OR (b22-k1*g22,-(b12-k1*g12))
    const Real fTolerance = 1e-06f;
    Real fA0 = kCurvatureTensor[0][1] - rfCurv0*kMetricTensor[0][1];
    Real fA1 = rfCurv0*kMetricTensor[0][0] - kCurvatureTensor[0][0];
    Real fLength = Math::Sqrt(fA0*fA0+fA1*fA1);
    if ( fLength >= fTolerance )
    {
        rkDir0 = fA0*kDerU + fA1*kDerV;
    }
    else
    {
        fA0 = kCurvatureTensor[1][1] - rfCurv0*kMetricTensor[1][1];
        fA1 = rfCurv0*kMetricTensor[0][1] - kCurvatureTensor[0][1];
        fLength = Math::Sqrt(fA0*fA0+fA1*fA1);
        if ( fLength >= fTolerance )
        {
            rkDir0 = fA0*kDerU + fA1*kDerV;
        }
        else
        {
            // umbilic (surface is locally sphere, any direction principal)
            rkDir0 = kDerU;
        }
    }
    rkDir0.Unitize();

    // second tangent is cross product of first tangent and normal
    rkDir1 = rkDir0.Cross(kNormal);
}
//----------------------------------------------------------------------------

