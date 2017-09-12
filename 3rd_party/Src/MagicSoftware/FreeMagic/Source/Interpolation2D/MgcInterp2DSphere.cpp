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

#include "MgcInterp2DSphere.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DSphere::Interp2DSphere (int iVertexQuantity, Real* afTheta,
    Real* afPhi, Real* afF)
{
    // Copy the input data.  The larger arrays are used to support wrap-around
    // in the Delaunay triangulation for the interpolator.  The Vector2
    // object V corresponds to (V.x,V.y) = (theta,phi).
    int iThreeQuantity = 3*iVertexQuantity;
    Vector2* akWrapAngles = new Vector2[iThreeQuantity];
    Real* afWrapF = new Real[iThreeQuantity];
    for (int i = 0; i < iVertexQuantity; i++)
    {
        akWrapAngles[i].x = afTheta[i];
        akWrapAngles[i].y = afPhi[i];
        afWrapF[i] = afF[i];
    }
    delete[] afTheta;
    delete[] afPhi;
    delete[] afF;

    // use periodicity to get wrap-around in the Delaunay triangulation
    int iI0 = 0, iI1 = iVertexQuantity, iI2 = 2*iVertexQuantity;
    for (/**/; iI0 < iVertexQuantity; iI0++, iI1++, iI2++)
    {
        akWrapAngles[iI1].x = akWrapAngles[iI0].x + Math::TWO_PI;
        akWrapAngles[iI2].x = akWrapAngles[iI0].x - Math::TWO_PI;
        akWrapAngles[iI1].y = akWrapAngles[iI0].y;
        akWrapAngles[iI2].y = akWrapAngles[iI0].y;
        afWrapF[iI1] = afWrapF[iI0];
        afWrapF[iI2] = afWrapF[iI0];
    }

    m_pkInterp = new Interp2DQdrNonuniform(iThreeQuantity,akWrapAngles,
        afWrapF);
}
//----------------------------------------------------------------------------
Interp2DSphere::~Interp2DSphere ()
{
    delete m_pkInterp;
}
//----------------------------------------------------------------------------
void Interp2DSphere::GetSphericalCoords (Real fX, Real fY, Real fZ,
    Real& rfTheta, Real& rfPhi)
{
    // Assumes (x,y,z) is unit length.  Returns -PI <= theta <= PI and
    // 0 <= phiAngle <= PI.

    if ( fZ < 1.0f )
    {
        if ( fZ > -1.0f )
        {
            rfTheta = (Real)atan2(fY,fX);
            rfPhi = (Real)acos(fZ);
        }
        else
        {
            rfTheta = -Math::PI;
            rfPhi = Math::PI;
        }
    }
    else
    {
        rfTheta = -Math::PI;
        rfPhi = 0.0f;
    }
}
//----------------------------------------------------------------------------
bool Interp2DSphere::Evaluate (Real fTheta, Real fPhi, Real& rfF)
{
    Vector2 kAngles(fTheta,fPhi);
    Real fThetaDeriv, fPhiDeriv;
    return m_pkInterp->Evaluate(kAngles,rfF,fThetaDeriv,fPhiDeriv);
}
//----------------------------------------------------------------------------


