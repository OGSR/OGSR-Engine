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

#include "MgcIntr3DPlnPln.h"
using namespace Mgc;

static const Real gs_fEpsilon = 1e-08f;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Plane& rkPlane0, const Plane& rkPlane1)
{
    Vector3 kCross = rkPlane0.Normal().Cross(rkPlane1.Normal());
    Real fSqrLength = kCross.SquaredLength();
    return fSqrLength > gs_fEpsilon;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Plane& rkPlane0, const Plane& rkPlane1,
    Line3& rkLine)
{
    // If Cross(N0,N1) is zero, then either planes are parallel and separated
    // or the same plane.  In both cases, 'false' is returned.  Otherwise,
    // the intersection line is
    //
    //   L(t) = t*Cross(N0,N1) + c0*N0 + c1*N1
    //
    // for some coefficients c0 and c1 and for t any real number (the line
    // parameter).  Taking dot products with the normals,
    //
    //   d0 = Dot(N0,L) = c0*Dot(N0,N0) + c1*Dot(N0,N1)
    //   d1 = Dot(N1,L) = c0*Dot(N0,N1) + c1*Dot(N1,N1)
    //
    // which are two equations in two unknowns.  The solution is
    //
    //   c0 = (Dot(N1,N1)*d0 - Dot(N0,N1)*d1)/det
    //   c1 = (Dot(N0,N0)*d1 - Dot(N0,N1)*d0)/det
    //
    // where det = Dot(N0,N0)*Dot(N1,N1)-Dot(N0,N1)^2.

    Real fN00 = rkPlane0.Normal().SquaredLength();
    Real fN01 = rkPlane0.Normal().Dot(rkPlane1.Normal());
    Real fN11 = rkPlane1.Normal().SquaredLength();
    Real fDet = fN00*fN11 - fN01*fN01;

    if ( Math::FAbs(fDet) < gs_fEpsilon )
        return false;

    Real fInvDet = 1.0f/fDet;
    Real fC0 = (fN11*rkPlane0.Constant() - fN01*rkPlane1.Constant())*fInvDet;
    Real fC1 = (fN00*rkPlane1.Constant() - fN01*rkPlane0.Constant())*fInvDet;

    rkLine.Direction() = rkPlane0.Normal().Cross(rkPlane1.Normal());
    rkLine.Origin() = fC0*rkPlane0.Normal() + fC1*rkPlane1.Normal();
    return true;
}
//----------------------------------------------------------------------------


