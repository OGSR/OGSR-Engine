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

#include "MgcIntr3DLinTorus.h"
#include "MgcPolynomial.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray3& rkRay, const Torus& rkTorus,
    Real& rfS, Real& rfT)
{
    // The ray is p*dir+eye, p >= 0.  The intersection is determined by
    // p^4+c3*p^3+c2*p^2+c1*p+c0 = 0.  The direction is assumed to be towards
    // the object from the observer so that the minimum p solution gives the
    // nearest intersection.

    // compute coefficients of quartic polynomial
    Real fRo2 = rkTorus.Ro()*rkTorus.Ro();
    Real fRi2 = rkTorus.Ri()*rkTorus.Ri();
    const Vector3& rkEye = rkRay.Origin();
    const Vector3& rkDir = rkRay.Direction();
    Real fDD = rkDir.Dot(rkDir);
    Real fDE = rkEye.Dot(rkDir);
    Real fVal = rkEye.Dot(rkEye) - (fRo2 + fRi2);

    Polynomial kPoly(5);
    kPoly[0] = fVal*fVal - 4.0f*fRo2*(fRi2 - rkEye.z*rkEye.z);
    kPoly[1] = 4.0f*fDE*fVal + 8.0f*fRo2*rkDir.z*rkEye.z;
    kPoly[2] = 2.0f*fDD*fVal + 4.0f*fDE*fDE + 4.0f*fRo2*rkDir.z*rkDir.z;
    kPoly[3] = 4.0f*fDD*fDE;
    kPoly[4] = fDD*fDD;

    // solve the quartic
    int iCount;
    Real afRoot[4];
    kPoly.GetAllRoots(iCount,afRoot);

    // search for closest point
    Real fPMin = Math::MAX_REAL;
    rfS = 0.0f;
    rfT = 0.0f;
    for (int i = 0; i < iCount; i++)
    {
        if ( 0 <= afRoot[i] && afRoot[i] < fPMin )
        {
            fPMin = afRoot[i];
            Vector3 kPos = rkEye + fPMin*rkDir;
            rkTorus.GetParameters(kPos,rfS,rfT);
        }
    }

    return fPMin != Math::MAX_REAL;
}
//----------------------------------------------------------------------------


