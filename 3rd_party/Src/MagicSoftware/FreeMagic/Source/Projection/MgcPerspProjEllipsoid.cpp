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

#include "MgcPerspProjEllipsoid.h"
using namespace Mgc;

//----------------------------------------------------------------------------
void Mgc::PerspProjEllipsoid (const GeneralEllipsoid& rkEllipsoid,
    const Vector3& rkEye, const Plane& rkPlane, GeneralEllipse& rkEllipse)
{
    // compute matrix M
    Vector3 kAE = rkEllipsoid.m_kA*rkEye;
    Real fEAE = rkEye.Dot(kAE);
    Real fBE = rkEllipsoid.m_kB.Dot(rkEye);
    Real fTmp = 4.0f*(fEAE + fBE + rkEllipsoid.m_fC);
    Vector3 kTmp = rkEllipsoid.m_kB + 2.0f*kAE;

    Matrix3 kMat;
    kMat[0][0] = kTmp.x*kTmp.x - fTmp*rkEllipsoid.m_kA[0][0];
    kMat[0][1] = kTmp.x*kTmp.y - fTmp*rkEllipsoid.m_kA[0][1];
    kMat[0][2] = kTmp.x*kTmp.z - fTmp*rkEllipsoid.m_kA[0][2];
    kMat[1][1] = kTmp.y*kTmp.y - fTmp*rkEllipsoid.m_kA[1][1];
    kMat[1][2] = kTmp.y*kTmp.z - fTmp*rkEllipsoid.m_kA[1][2];
    kMat[2][2] = kTmp.z*kTmp.z - fTmp*rkEllipsoid.m_kA[2][2];
    kMat[1][0] = kMat[0][1];
    kMat[2][0] = kMat[0][2];
    kMat[2][1] = kMat[1][2];

    // Normalize N and construct U and V so that {U,V,N} forms a
    // right-handed, orthonormal basis.
    Vector3 kU, kV, kN = rkPlane.Normal();
    Vector3::GenerateOrthonormalBasis(kU,kV,kN,false);

    // compute coefficients for projected ellipse
    Vector3 kMU = kMat*kU, kMV = kMat*kV, kMN = kMat*kN;
    Real fDmNE = rkPlane.Constant() - kN.Dot(rkEye);

    rkEllipse.m_kA[0][0] = kU.Dot(kMU);
    rkEllipse.m_kA[0][1] = kU.Dot(kMV);
    rkEllipse.m_kA[1][1] = kV.Dot(kMV);
    rkEllipse.m_kA[1][0] = rkEllipse.m_kA[0][1];
    rkEllipse.m_kB.x = 2.0f*fDmNE*(kU.Dot(kMN));
    rkEllipse.m_kB.y = 2.0f*fDmNE*(kV.Dot(kMN));
    rkEllipse.m_fC = fDmNE*fDmNE*(kN.Dot(kMN));
}
//----------------------------------------------------------------------------
void Mgc::ConvertEllipse (GeneralEllipse& rkEllipse, Vector3& rkCenter,
    Vector3& rkAxis0, Vector3& rkAxis1, Real& rfHalfLength0,
    Real& rfHalfLength1)
{
    // factor A = R^t D R
    Real fTrace = rkEllipse.m_kA[0][0] + rkEllipse.m_kA[1][1];
    if ( fTrace < 0.0f )
    {
        // Convert A from negative definite to positive definite
        // (multiply quadratic equation by -1).
        rkEllipse.m_kA = -rkEllipse.m_kA;
        rkEllipse.m_kB = -rkEllipse.m_kB;
        rkEllipse.m_fC = -rkEllipse.m_fC;

        fTrace = -fTrace;
    }

    Real fTmp = rkEllipse.m_kA[0][0] - rkEllipse.m_kA[1][1];
    Real fDiscr = Math::Sqrt(fTmp*fTmp + 4.0f*rkEllipse.m_kA[0][1] *
        rkEllipse.m_kA[0][1]);

    // matrix D (eigenvalues of A)
    Real fD0 = 0.5f*(fTrace - fDiscr);
    Real fD1 = 0.5f*(fTrace + fDiscr);
    Real fInvD0 = 1.0f/fD0;
    Real fInvD1 = 1.0f/fD1;

    // matrix R (columns are eigenvectors of A)
    Matrix2 kR;
    if ( rkEllipse.m_kA[0][1] != 0.0f )
    {
        Real fInvLength;

        kR[0][0] = rkEllipse.m_kA[0][1];
        kR[1][0] = fD0 - rkEllipse.m_kA[0][0];
        fInvLength = Math::InvSqrt(kR[0][0]*kR[0][0]+kR[1][0]*kR[1][0]);
        kR[0][0] *= fInvLength;
        kR[1][0] *= fInvLength;

        kR[0][1] = fD1 - rkEllipse.m_kA[1][1];
        kR[1][1] = rkEllipse.m_kA[0][1];
        fInvLength = Math::InvSqrt(kR[0][1]*kR[0][1]+kR[1][1]*kR[1][1]);
        kR[0][1] *= fInvLength;
        kR[1][1] *= fInvLength;
    }
    else
    {
        kR = Matrix2::IDENTITY;
    }

    // compute the ellipse center
    Vector2 kBeta = kR*rkEllipse.m_kB;
    rkCenter.x = -0.5f*kBeta.x*fInvD0;
    rkCenter.y = -0.5f*kBeta.y*fInvD1;

    // comptue the ellipse axes
    rkAxis0.x = kR[0][0];
    rkAxis0.y = kR[1][0];
    rkAxis1.x = kR[0][1];
    rkAxis1.y = kR[1][1];

    // compute the ellipse axis half lengths
    fTmp = fD0*rkCenter.x*rkCenter.x + fD1*rkCenter.y*rkCenter.y -
        rkEllipse.m_fC;
    rfHalfLength0 = Math::Sqrt(Math::FAbs(fTmp*fInvD0));
    rfHalfLength1 = Math::Sqrt(Math::FAbs(fTmp*fInvD1));
}
//----------------------------------------------------------------------------

