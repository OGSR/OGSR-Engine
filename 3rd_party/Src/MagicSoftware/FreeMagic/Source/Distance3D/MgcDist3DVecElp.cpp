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

#include "MgcDist3DVecElp.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const EllipsoidStandard& rkEllipsoid,
    const Vector3& rkPoint, Vector3& rkClosest)
{
    const Real* afExtent = rkEllipsoid.Extents();

    Real fA2 = afExtent[0]*afExtent[0];
    Real fB2 = afExtent[1]*afExtent[1];
    Real fC2 = afExtent[2]*afExtent[2];
    Real fU2 = rkPoint.x*rkPoint.x;
    Real fV2 = rkPoint.y*rkPoint.y;
    Real fW2 = rkPoint.z*rkPoint.z;
    Real fA2U2 = fA2*fU2, fB2V2 = fB2*fV2, fC2W2 = fC2*fW2;

    // initial guess
    Real fURatio = rkPoint.x/afExtent[0];
    Real fVRatio = rkPoint.y/afExtent[1];
    Real fWRatio = rkPoint.z/afExtent[2];
    Real fT;
    if ( fURatio*fURatio+fVRatio*fVRatio+fWRatio*fWRatio < 1.0f )
    {
        fT = 0.0f;
    }
    else
    {
        Real fMax = afExtent[0];
        if ( afExtent[1] > fMax )
            fMax = afExtent[1];
        if ( afExtent[2] > fMax )
            fMax = afExtent[2];

        fT = fMax*rkPoint.Length();
    }

    // Newton's method
    const int iMaxIteration = 64;
    const Real fEpsilon = 1e-08f;
    Real fP, fQ, fR;
    for (int i = 0; i < iMaxIteration; i++)
    {
        fP = fT+fA2;
        fQ = fT+fB2;
        fR = fT+fC2;
        Real fP2 = fP*fP;
        Real fQ2 = fQ*fQ;
        Real fR2 = fR*fR;
        Real fS = fP2*fQ2*fR2-fA2U2*fQ2*fR2-fB2V2*fP2*fR2-fC2W2*fP2*fQ2;
        if ( Math::FAbs(fS) < fEpsilon )
            break;

        Real fPQ = fP*fQ, fPR = fP*fR, fQR = fQ*fR, fPQR = fP*fQ*fR;
        Real fDS = 2.0f*(fPQR*(fQR+fPR+fPQ)-fA2U2*fQR*(fQ+fR)-
            fB2V2*fPR*(fP+fR)-fC2W2*fPQ*(fP+fQ));
        fT -= fS/fDS;
    }

    rkClosest.x = fA2*rkPoint.x/fP;
    rkClosest.y = fB2*rkPoint.y/fQ;
    rkClosest.z = fC2*rkPoint.z/fR;
    Vector3 kDiff = rkClosest - rkPoint;
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const EllipsoidStandard& rkEllipsoid,
    const Vector3& rkPoint, Vector3& rkClosest)
{
    return Math::Sqrt(SqrDistance(rkEllipsoid,rkPoint,rkClosest));
}
//----------------------------------------------------------------------------


