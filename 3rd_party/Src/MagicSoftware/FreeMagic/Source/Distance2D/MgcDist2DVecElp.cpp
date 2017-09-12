// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of afExtent[0] license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include "MgcDist2DVecElp.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const EllipseStandard2& rkEllipse,
    const Vector2& rkPoint, Vector2& rkClosest)
{
    const Real* afExtent = rkEllipse.Extents();

    Real fA2 = afExtent[0]*afExtent[0];
    Real fB2 = afExtent[1]*afExtent[1];
    Real fU2 = rkPoint.x*rkPoint.x;
    Real fV2 = rkPoint.y*rkPoint.y;
    Real fA2U2 = fA2*fU2, fB2V2 = fB2*fV2;
    Real fDx, fDy, fXDivA, fYDivB;

    // handle points near the coordinate axes
    const Real fThreshold = 1e-12f;
    if ( Math::FAbs(rkPoint.x) <= fThreshold )  // rkPoint.x == 0
    {
        if ( afExtent[0] >= afExtent[1] 
        ||   Math::FAbs(rkPoint.y) >= afExtent[1]-fA2/afExtent[1] )
        {
            rkClosest.x = 0.0f;
            rkClosest.y = ( rkPoint.y >= 0.0f ? afExtent[1] : -afExtent[1] );
            fDy = rkClosest.y - rkPoint.y;
            return Math::FAbs(fDy);
        }
        else
        {
            rkClosest.y = fB2*rkPoint.y/(fB2-fA2);
            fDy = rkClosest.y - rkPoint.y;
            fYDivB = rkClosest.y/afExtent[1];
            rkClosest.x = afExtent[0]*Math::Sqrt(
                Math::FAbs(1.0f-fYDivB*fYDivB));
            return Math::Sqrt(rkClosest.x*rkClosest.x+fDy*fDy);
        }
    }

    if ( Math::FAbs(rkPoint.y) <= fThreshold )  // rkPoint.y == 0
    {
        if ( afExtent[1] >= afExtent[0]
        ||   Math::FAbs(rkPoint.x) >= afExtent[0]-fB2/afExtent[0] )
        {
            rkClosest.x = ( rkPoint.x >= 0.0f ? afExtent[0] : -afExtent[0] );
            rkClosest.y = 0.0f;
            fDx = rkClosest.x - rkPoint.x;
            return Math::FAbs(fDx);
        }
        else
        {
            rkClosest.x = fA2*rkPoint.x/(fA2-fB2);
            fDx = rkClosest.x - rkPoint.x;
            fXDivA = rkClosest.x/afExtent[0];
            rkClosest.y = afExtent[1]*Math::Sqrt(
                Math::FAbs(1.0f-fXDivA*fXDivA));
            return Math::Sqrt(fDx*fDx+rkClosest.y*rkClosest.y);
        }
    }

    // initial guess
    Real fURatio = rkPoint.x/afExtent[0];
    Real fVRatio = rkPoint.y/afExtent[1];
    Real fT;
    if ( fURatio*fURatio + fVRatio*fVRatio < 1.0f )
    {
        fT = 0.0f;
    }
    else
    {
        Real fMax = afExtent[0];
        if ( afExtent[1] > fMax )
            fMax = afExtent[1];

        fT = fMax*rkPoint.Length();
    }

    // Newton's method
    const int iMaxIteration = 64;
    const Real fEpsilon = 1e-06f;
    Real fP, fQ;
    for (int i = 0; i < iMaxIteration; i++)
    {
        fP = fT+fA2;
        fQ = fT+fB2;
        Real fP2 = fP*fP;
        Real fQ2 = fQ*fQ;
        Real fR = fP2*fQ2-fA2U2*fQ2-fB2V2*fP2;
        if ( Math::FAbs(fR) < fEpsilon )
            break;

        Real fDR = 2.0f*(fP*fQ*(fP+fQ)-fA2U2*fQ-fB2V2*fP);
        fT -= fR/fDR;
    }

    rkClosest.x = fA2*rkPoint.x/fP;
    rkClosest.y = fB2*rkPoint.y/fQ;
    Vector2 kDiff = rkClosest - rkPoint;
    return kDiff.Length();
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const EllipseStandard2& rkEllipse,
    const Vector2& rkPoint, Vector2& rkClosest)
{
    return Math::Sqrt(SqrDistance(rkEllipse,rkPoint,rkClosest));
}
//----------------------------------------------------------------------------


