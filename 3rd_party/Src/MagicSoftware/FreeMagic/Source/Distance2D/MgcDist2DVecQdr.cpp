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

#include "MgcDist2DVecQdr.h"
#include "MgcEigen.h"
#include "MgcPolynomial.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void ComputeCoeff2 (Real afA[2], Real afB[2], Real afD[2],
    Real fC, Polynomial& rkPoly)
{
    Real afBPad[2] = { afB[0]+afA[0]*afD[0], afB[1]+afA[1]*afD[1] };
    Real afBSqr[2] = { afB[0]*afB[0], afB[1]*afB[1] };
    Real afDSqr[2] = { afD[0]*afD[0], afD[1]*afD[1] };
    Real fDPrd = afD[0]*afD[1];
    Real fDSum = afD[0]+afD[1];

    rkPoly[0] = afA[0]*afBPad[0]+afA[1]*afBPad[1]+fC;
    rkPoly[1] = -afBSqr[0]-afBSqr[1]+4.0f*(afA[0]*afD[1]*afBPad[0]+
        afA[1]*afD[0]*afBPad[1]+fC*fDSum);
    rkPoly[2] = -afBSqr[0]*(afD[0]+4.0f*afD[1])-afBSqr[1]*(afD[1]+
        4.0f*afD[0])+4.0f*(afA[0]*afDSqr[1]*afBPad[0]+afA[1]*afDSqr[0]*
        afBPad[1]+fC*(afDSqr[0]+afDSqr[1]+4.0f*fDPrd));

    Real fTmp = -4.0f*(afBSqr[0]*afD[1]+afBSqr[1]*afD[0]-4.0f*fC*fDPrd);
    rkPoly[3] = fDSum*fTmp;
    rkPoly[4] = fDPrd*fTmp;
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector2& rkPoint, const Real afQuad[6],
    Vector2& rkClosest)
{
    // eigendecomposition
    Eigen kES(2);
    kES.Matrix(0,0) = afQuad[3];
    kES.Matrix(0,1) = 0.5f*afQuad[5];
    kES.Matrix(1,0) = kES.Matrix(0,1);
    kES.Matrix(1,1) = afQuad[4];

    kES.IncrSortEigenStuff2();
    Real** aafR = kES.GetEigenvector();

    Real afA[2], afB[2], afD[2], fC = afQuad[0];
    int i;
    for (i = 0; i < 2; i++)
    {
        afA[i] = aafR[0][i]*rkPoint.x + aafR[1][i]*rkPoint.y;
        afB[i] = aafR[0][i]*afQuad[1] + aafR[1][i]*afQuad[2];
        afD[i] = kES.GetEigenvalue(i);
    }

    Polynomial kPoly(4);
    ComputeCoeff2(afA,afB,afD,fC,kPoly);

    int iCount;
    Real afRoot[4];
    kPoly.GetAllRoots(iCount,afRoot);

    if ( iCount > 0 )
    {
        Real fMinDistSqr = Math::MAX_REAL;
        int iMinIndex = -1;
        Real afV[2], fDenom;
        for (int iIndex = 0; iIndex < iCount; iIndex++)
        {
            // compute closest point for this root
            for (i = 0; i < 2; i++)
            {
                fDenom = 1.0f + 2.0f*afRoot[iIndex]*afD[i];
                afV[i] = (afA[i]-afRoot[iIndex]*afB[i])/fDenom;
            }

            rkClosest.x = aafR[0][0]*afV[0] + aafR[0][1]*afV[1];
            rkClosest.y = aafR[1][0]*afV[0] + aafR[1][1]*afV[1];

            // compute squared distance from point to quadratic
            Vector2 kDiff = rkClosest - rkPoint;
            Real fDistSqr = kDiff.SquaredLength();
            if ( fDistSqr < fMinDistSqr )
            {
                fMinDistSqr = fDistSqr;
                iMinIndex = iIndex;
            }
        }

        for (i = 0; i < 2; i++)
        {
            fDenom = 1.0f + 2.0f*afRoot[iMinIndex]*afD[i];
            afV[i] = (afA[i]-afRoot[iMinIndex]*afB[i])/fDenom;
        }

        rkClosest.x = aafR[0][0]*afV[0] + aafR[0][1]*afV[1];
        rkClosest.y = aafR[1][0]*afV[0] + aafR[1][1]*afV[1];
        return fMinDistSqr;
    }
    else
    {
        // should not happen
        assert( false );
        return -1.0f;
    }
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector2& rkPoint, const Real afQuad[6],
    Vector2& rkClosest)
{
    return Math::Sqrt(SqrDistance(rkPoint,afQuad,rkClosest));
}
//----------------------------------------------------------------------------


