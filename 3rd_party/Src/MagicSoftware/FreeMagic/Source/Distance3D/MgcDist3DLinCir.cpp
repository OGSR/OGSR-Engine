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

#include "MgcDist3DLinCir.h"
#include "MgcDist3DVecCir.h"
#include "MgcPolynomial.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine, const Circle3& rkCircle,
    Vector3* pkLineClosest, Vector3* pkCircleClosest)
{
    Vector3 kDiff = rkLine.Origin() - rkCircle.Center();
    Real fDSqrLen = kDiff.SquaredLength();
    Real fMdM = rkLine.Direction().SquaredLength();
    Real fDdM = kDiff.Dot(rkLine.Direction());
    Real fNdM = rkCircle.N().Dot(rkLine.Direction());
    Real fDdN = kDiff.Dot(rkCircle.N());

    Real fA0 = fDdM;
    Real fA1 = fMdM;
    Real fB0 = fDdM - fNdM*fDdN;
    Real fB1 = fMdM - fNdM*fNdM;
    Real fC0 = fDSqrLen - fDdN*fDdN;
    Real fC1 = fB0;
    Real fC2 = fB1;
    Real fRSqr = rkCircle.Radius()*rkCircle.Radius();

    Real fA0Sqr = fA0*fA0;
    Real fA1Sqr = fA1*fA1;
    Real fTwoA0A1 = 2.0f*fA0*fA1;
    Real fB0Sqr = fB0*fB0;
    Real fB1Sqr = fB1*fB1;
    Real fTwoB0B1 = 2.0f*fB0*fB1;
    Real fTwoC1 = 2.0f*fC1;

    // The minimum point B+t*M occurs when t is a root of the quartic
    // equation whose coefficients are defined below.
    Polynomial kPoly(5);
    kPoly[0] = fA0Sqr*fC0 - fB0Sqr*fRSqr;
    kPoly[1] = fTwoA0A1*fC0 + fA0Sqr*fTwoC1 - fTwoB0B1*fRSqr;
    kPoly[2] = fA1Sqr*fC0 + fTwoA0A1*fTwoC1 + fA0Sqr*fC2 - fB1Sqr*fRSqr;
    kPoly[3] = fA1Sqr*fTwoC1 + fTwoA0A1*fC2;
    kPoly[4] = fA1Sqr*fC2;

    int iNumRoots;
    Real afRoot[4];
    kPoly.GetAllRoots(iNumRoots,afRoot);

    Real fMinSqrDist = Math::MAX_REAL;
    for (int i = 0; i < iNumRoots; i++)
    {
        // compute distance from P(t) to circle
        Vector3 kP = rkLine.Origin() + afRoot[i]*rkLine.Direction();
        Vector3 kCircleClosest;
        Real fSqrDist = SqrDistance(kP,rkCircle,&kCircleClosest);
        if ( fSqrDist < fMinSqrDist )
        {
            fMinSqrDist = fSqrDist;
            if ( pkLineClosest )
                *pkLineClosest = kP;
            if ( pkCircleClosest )
                *pkCircleClosest = kCircleClosest;
        }
    }

    return fMinSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine, const Circle3& rkCircle,
    Vector3* pkLineClosest, Vector3* pkCircleClosest)
{
    return Math::Sqrt(SqrDistance(rkLine,rkCircle,pkLineClosest,
        pkCircleClosest));
}
//----------------------------------------------------------------------------


