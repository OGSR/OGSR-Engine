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

#include "MgcImplicitSurface.h"
using namespace Mgc;

//----------------------------------------------------------------------------
ImplicitSurface::ImplicitSurface (Function oF, Function aoDF[3],
    Function aoD2F[6])
{
    m_oF = oF;
    memcpy(m_aoDF,aoDF,3*sizeof(Function));
    memcpy(m_aoD2F,aoD2F,6*sizeof(Function));
}
//----------------------------------------------------------------------------
bool ImplicitSurface::IsOnSurface (Real fX, Real fY, Real fZ,
    Real fTolerance) const
{
    return Math::FAbs(m_oF(fX,fY,fZ)) <= fTolerance;
}
//----------------------------------------------------------------------------
Vector3 ImplicitSurface::GetGradient (Real fX, Real fY, Real fZ) const
{
    Real fDx = m_aoDF[0](fX,fY,fZ);
    Real fDy = m_aoDF[1](fX,fY,fZ);
    Real fDz = m_aoDF[2](fX,fY,fZ);
    return Vector3(fDx,fDy,fDz);
}
//----------------------------------------------------------------------------
Matrix3 ImplicitSurface::GetHessian (Real fX, Real fY, Real fZ) const
{
    Real fDxx = m_aoD2F[0](fX,fY,fZ);
    Real fDxy = m_aoD2F[1](fX,fY,fZ);
    Real fDxz = m_aoD2F[2](fX,fY,fZ);
    Real fDyy = m_aoD2F[3](fX,fY,fZ);
    Real fDyz = m_aoD2F[4](fX,fY,fZ);
    Real fDzz = m_aoD2F[5](fX,fY,fZ);
    return Matrix3(fDxx,fDxy,fDxz,fDxy,fDyy,fDyz,fDxz,fDyz,fDzz);
}
//----------------------------------------------------------------------------
void ImplicitSurface::GetFrame (Real fX, Real fY, Real fZ,
    Vector3& kTangent0, Vector3& kTangent1, Vector3& kNormal) const
{
    kNormal = GetGradient(fX,fY,fZ);

    if ( Math::FAbs(kNormal.x) >= Math::FAbs(kNormal.y)
    &&   Math::FAbs(kNormal.x) >= Math::FAbs(kNormal.z) )
    {
        kTangent0.x = -kNormal.y;
        kTangent0.y = kNormal.x;
        kTangent0.z = 0.0f;
    }
    else
    {
        kTangent0.x = 0.0f;
        kTangent0.y = kNormal.z;
        kTangent0.z = -kNormal.y;
    }

    kTangent0.Unitize();
    kTangent1 = kNormal.Cross(kTangent0);
}
//----------------------------------------------------------------------------
bool ImplicitSurface::ComputePrincipalCurvatureInfo (Real fX, Real fY,
    Real fZ, Real& rfCurv0, Real& rfCurv1, Vector3& rkDir0,
    Vector3& rkDir1)
{
    // Principal curvatures and directions for implicitly defined surfaces
    // F(x,y,z) = 0.
    //
    // DF = (Fx,Fy,Fz), L = Length(DF)
    //
    // D^2 F = +-           -+
    //         | Fxx Fxy Fxz |
    //         | Fxy Fyy Fyz |
    //         | Fxz Fyz Fzz |
    //         +-           -+
    //
    // adj(D^2 F) = +-                                                 -+
    //              | Fyy*Fzz-Fyz*Fyz  Fyz*Fxz-Fxy*Fzz  Fxy*Fyz-Fxz*Fyy |
    //              | Fyz*Fxz-Fxy*Fzz  Fxx*Fzz-Fxz*Fxz  Fxy*Fxz-Fxx*Fyz |
    //              | Fxy*Fyz-Fxz*Fyy  Fxy*Fxz-Fxx*Fyz  Fxx*Fyy-Fxy*Fxy |
    //              +-                                                 -+
    //
    // Gaussian curvature = [DF^t adj(D^2 F) DF]/L^4
    // 
    // Mean curvature = 0.5*[trace(D^2 F)/L - (DF^t D^2 F DF)/L^3]

    // first derivatives
    Vector3 kGradient = GetGradient(fX,fY,fZ);
    Real fL = kGradient.Length();
    const Real fTolerance = 1e-08f;
    if ( fL <= fTolerance )
        return false;

    Real fDxDx = kGradient.x*kGradient.x;
    Real fDxDy = kGradient.x*kGradient.y;
    Real fDxDz = kGradient.x*kGradient.z;
    Real fDyDy = kGradient.y*kGradient.y;
    Real fDyDz = kGradient.y*kGradient.z;
    Real fDzDz = kGradient.z*kGradient.z;

    Real fInvL = 1.0f/fL;
    Real fInvL2 = fInvL*fInvL;
    Real fInvL3 = fInvL*fInvL2;
    Real fInvL4 = fInvL2*fInvL2;

    // second derivatives
    Real fDxx = m_aoD2F[0](fX,fY,fZ);
    Real fDxy = m_aoD2F[1](fX,fY,fZ);
    Real fDxz = m_aoD2F[2](fX,fY,fZ);
    Real fDyy = m_aoD2F[3](fX,fY,fZ);
    Real fDyz = m_aoD2F[4](fX,fY,fZ);
    Real fDzz = m_aoD2F[5](fX,fY,fZ);

    // mean curvature
    Real fMCurv = 0.5f*fInvL3*(fDxx*(fDyDy+fDzDz) + fDyy*(fDxDx+fDzDz) +
        fDzz*(fDxDx+fDyDy) - 2.0f*(fDxy*fDxDy+fDxz*fDxDz+fDyz*fDyDz));

    // Gaussian curvature
    Real fGCurv = fInvL4*(fDxDx*(fDyy*fDzz-fDyz*fDyz) +
        fDyDy*(fDxx*fDzz-fDxz*fDxz) + fDzDz*(fDxx*fDyy-fDxy*fDxy) +
        2.0f*(fDxDy*(fDxz*fDyz-fDxy*fDzz) + fDxDz*(fDxy*fDyz-fDxz*fDyy) +
        fDyDz*(fDxy*fDxz-fDxx*fDyz)));

    // solve for principal curvatures
    Real fDiscr = Math::Sqrt(Math::FAbs(fMCurv*fMCurv-fGCurv));
    rfCurv0 = fMCurv - fDiscr;
    rfCurv1 = fMCurv + fDiscr;

    Real fM00 = ((-1.0f + fDxDx*fInvL2)*fDxx)*fInvL + (fDxDy*fDxy)*fInvL3 +
        (fDxDz*fDxz)*fInvL3;
    Real fM01 = ((-1.0f + fDxDx*fInvL2)*fDxy)*fInvL + (fDxDy*fDyy)*fInvL3 +
        (fDxDz*fDyz)*fInvL3;
    Real fM02 = ((-1.0f + fDxDx*fInvL2)*fDxz)*fInvL + (fDxDy*fDyz)*fInvL3 +
        (fDxDz*fDzz)*fInvL3;
    Real fM10 = (fDxDy*fDxx)*fInvL3 + ((-1.0f + fDyDy*fInvL2)*fDxy)*fInvL +
        (fDyDz*fDxz)*fInvL3;
    Real fM11 = (fDxDy*fDxy)*fInvL3 + ((-1.0f + fDyDy*fInvL2)*fDyy)*fInvL +
        (fDyDz*fDyz)*fInvL3;
    Real fM12 = (fDxDy*fDxz)*fInvL3 + ((-1.0f + fDyDy*fInvL2)*fDyz)*fInvL +
        (fDyDz*fDzz)*fInvL3;
    Real fM20 = (fDxDz*fDxx)*fInvL3 + (fDyDz*fDxy)*fInvL3 + ((-1.0f +
        fDzDz*fInvL2)*fDxz)*fInvL;
    Real fM21 = (fDxDz*fDxy)*fInvL3 + (fDyDz*fDyy)*fInvL3 + ((-1.0f +
        fDzDz*fInvL2)*fDyz)*fInvL;
    Real fM22 = (fDxDz*fDxz)*fInvL3 + (fDyDz*fDyz)*fInvL3 + ((-1.0f +
        fDzDz*fInvL2)*fDzz)*fInvL;

    // solve for principal directions
    Real fTmp1 = fM00 + rfCurv0;
    Real fTmp2 = fM11 + rfCurv0;
    Real fTmp3 = fM22 + rfCurv0;

    Vector3 akU[3];
    Real afLength[3];

    akU[0].x = fM01*fM12-fM02*fTmp2;
    akU[0].y = fM02*fM10-fM12*fTmp1;
    akU[0].z = fTmp1*fTmp2-fM01*fM10;
    afLength[0] = akU[0].Length();

    akU[1].x = fM01*fTmp3-fM02*fM21;
    akU[1].y = fM02*fM20-fTmp1*fTmp3;
    akU[1].z = fTmp1*fM21-fM01*fM20;
    afLength[1] = akU[1].Length();

    akU[2].x = fTmp2*fTmp3-fM12*fM21;
    akU[2].y = fM12*fM20-fM10*fTmp3;
    akU[2].z = fM10*fM21-fM20*fTmp2;
    afLength[2] = akU[2].Length();

    int iMaxIndex = 0;
    Real fMax = afLength[0];
    if ( afLength[1] > fMax )
    {
        iMaxIndex = 1;
        fMax = afLength[1];
    }
    if ( afLength[2] > fMax )
        iMaxIndex = 2;

    Real fInvLength = 1.0f/afLength[iMaxIndex];
    akU[iMaxIndex] *= fInvLength;

    rkDir1 = akU[iMaxIndex];
    rkDir0 = rkDir1.UnitCross(kGradient);

    return true;
}
//----------------------------------------------------------------------------

