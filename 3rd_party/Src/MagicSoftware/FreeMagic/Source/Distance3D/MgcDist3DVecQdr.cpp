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

#include "MgcDist3DVecQdr.h"
#include "MgcEigen.h"
#include "MgcPolynomial.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void ComputeCoeff3 (Real afA[3], Real afB[3], Real afD[3], Real fC,
    Polynomial& rkPoly)
{
    Real afBPad[3] =
    {
        afB[0]+afA[0]*afD[0],
        afB[1]+afA[1]*afD[1],
        afB[2]+afA[2]*afD[2]
    };

    Real afBSqr[3] =
    {
        afB[0]*afB[0],
        afB[1]*afB[1],
        afB[2]*afB[2]
    };

    Real afDSum[4] =
    {
        afD[0]+afD[1],
        afD[0]+afD[2],
        afD[1]+afD[2],
        afD[0]+afD[1]+afD[2]
    };

    Real afDPrd[4] =
    {
        afD[0]*afD[1],
        afD[0]*afD[2],
        afD[1]*afD[2],
        afD[0]*afD[1]*afD[2]
    };

    Real afDSqr[3] =
    {
        afD[0]*afD[0],
        afD[1]*afD[1],
        afD[2]*afD[2]
    };

    rkPoly[0] = afA[0]*afBPad[0]+afA[1]*afBPad[1]+afA[2]*afBPad[2]+fC;

    rkPoly[1] = - afBSqr[0] - afBSqr[1] - afBSqr[2] + 4.0f*(
        afA[0]*afBPad[0]*afDSum[2] + afA[1]*afBPad[1]*afDSum[1] +
        afA[2]*afBPad[2]*afDSum[0] + fC*afDSum[3]);

    rkPoly[2] = - afBSqr[0]*(afD[0] + 4.0f*afDSum[2]) - afBSqr[1]*(afD[1] +
        4.0f*afDSum[1]) - afBSqr[2]*(afD[2] + 4.0f*afDSum[0]) + 4.0f*(
        afA[0]*afBPad[0]*(afDSum[2]*afDSum[2]+2*afDPrd[2]) +
        afA[1]*afBPad[1]*(afDSum[1]*afDSum[1]+2*afDPrd[1]) +
        afA[2]*afBPad[2]*(afDSum[0]*afDSum[0]+2*afDPrd[0]) +
        fC*(afDSqr[0]+afDSqr[1]+afDSqr[2]+4.0f*(
        afDPrd[0]+afDPrd[1]+afDPrd[2])));

    rkPoly[3] =
        - afBSqr[0]*(afD[1]*afDSum[0]+afD[2]*afDSum[1]+4.0f*afDPrd[2])
        - afBSqr[1]*(afD[0]*afDSum[0]+afD[2]*afDSum[2]+4.0f*afDPrd[1])
        - afBSqr[2]*(afD[0]*afDSum[1]+afD[1]*afDSum[2]+4.0f*afDPrd[0])
        + 4.0f*(afA[0]*afDPrd[2]*afBPad[0]*afDSum[2] +
        afA[1]*afDPrd[1]*afBPad[1]*afDSum[1] +
        afA[2]*afDPrd[0]*afBPad[2]*afDSum[0] +
        fC*(afDSqr[0]*afDSum[2]+afDSqr[1]*afDSum[1]+afDSqr[2]*afDSum[0]+
        4.0f*afDPrd[3]));

    rkPoly[3] *= 4.0f;

    rkPoly[4] =
        - afBSqr[0]*(afD[0]*(afDSqr[1]+afDSqr[2])+4.0f*afDPrd[2]*afDSum[3])
        - afBSqr[1]*(afD[1]*(afDSqr[0]+afDSqr[2])+4.0f*afDPrd[1]*afDSum[3])
        - afBSqr[2]*(afD[2]*(afDSqr[0]+afDSqr[1])+4.0f*afDPrd[0]*afDSum[3])
        + 4.0f*(afA[0]*afDSqr[1]*afDSqr[2]*afBPad[0] +
        afA[1]*afDSqr[0]*afDSqr[2]*afBPad[1] +
        afA[2]*afDSqr[0]*afDSqr[1]*afBPad[2] +
        fC*(afDSqr[0]*afDSqr[1]+afDSqr[0]*afDSqr[2]+afDSqr[1]*afDSqr[2]
        + 4.0f*afDPrd[3]*afDSum[3]));

    rkPoly[4] *= 4.0f;

    rkPoly[5] = 16.0f*(afDPrd[0]+afDPrd[1]+afDPrd[2])*(
        - afBSqr[0]*afDPrd[2] - afBSqr[1]*afDPrd[1] - afBSqr[2]*afDPrd[0] +
        4.0f*fC*afDPrd[3]);

    rkPoly[6] = 16.0f*afDPrd[3]*(- afBSqr[0]*afDPrd[2] - afBSqr[1]*afDPrd[1]
        - afBSqr[2]*afDPrd[0] + 4.0f*fC*afDPrd[3]);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Real afQuad[10],
    Vector3& rkClosest)
{
    // eigendecomposition
    Eigen kES(3);
    kES.Matrix(0,0) = afQuad[4];
    kES.Matrix(0,1) = 0.5f*afQuad[7];
    kES.Matrix(0,2) = 0.5f*afQuad[8];
    kES.Matrix(1,0) = kES.Matrix(0,1);
    kES.Matrix(1,1) = afQuad[5];
    kES.Matrix(1,2) = 0.5f*afQuad[9];
    kES.Matrix(2,0) = kES.Matrix(0,2);
    kES.Matrix(2,1) = kES.Matrix(1,2);
    kES.Matrix(2,2) = afQuad[6];

    kES.IncrSortEigenStuff3();
    Real** aafR = kES.GetEigenvector();

    Real afA[3], afB[3], afD[3], fC = afQuad[0];
    int i;
    for (i = 0; i < 3; i++)
    {
        afA[i] = aafR[0][i]*rkPoint.x + aafR[1][i]*rkPoint.y +
            aafR[2][i]*rkPoint.z;
        afB[i] = aafR[0][i]*afQuad[1] + aafR[1][i]*afQuad[2] +
            aafR[2][i]*afQuad[3];
        afD[i] = kES.GetEigenvalue(i);
    }

    Polynomial kPoly(6);
    ComputeCoeff3(afA,afB,afD,fC,kPoly);

    int iCount;
    Real afRoot[6];
    kPoly.GetAllRoots(iCount,afRoot);

    if ( iCount > 0 )
    {
        Real fMinDistSqr = Math::MAX_REAL;
        int iMinIndex = -1;
        Real afV[3], fDenom;
        for (int iIndex = 0; iIndex < iCount; iIndex++)
        {
            // compute closest point for this root
            for (i = 0; i < 3; i++)
            {
                fDenom = 1.0f + 2.0f*afRoot[iIndex]*afD[i];
                afV[i] = (afA[i]-afRoot[iIndex]*afB[i])/fDenom;
            }

            rkClosest.x = aafR[0][0]*afV[0] + aafR[0][1]*afV[1] +
                aafR[0][2]*afV[2];
            rkClosest.y = aafR[1][0]*afV[0] + aafR[1][1]*afV[1] +
                aafR[1][2]*afV[2];
            rkClosest.z = aafR[2][0]*afV[0] + aafR[2][1]*afV[1] +
                aafR[2][2]*afV[2];

            // compute squared distance from point to quadric
            Vector3 kDiff = rkClosest - rkPoint;
            Real fDistSqr = kDiff.SquaredLength();
            if ( fDistSqr < fMinDistSqr )
            {
                fMinDistSqr = fDistSqr;
                iMinIndex = iIndex;
            }
        }

        for (i = 0; i < 3; i++)
        {
            fDenom = 1.0f+2.0f*afRoot[iMinIndex]*afD[i];
            afV[i] = (afA[i]-afRoot[iMinIndex]*afB[i])/fDenom;
        }

        rkClosest.x = aafR[0][0]*afV[0] + aafR[0][1]*afV[1] +
            aafR[0][2]*afV[2];
        rkClosest.y = aafR[1][0]*afV[0] + aafR[1][1]*afV[1] +
            aafR[1][2]*afV[2];
        rkClosest.z = aafR[2][0]*afV[0] + aafR[2][1]*afV[1] +
            aafR[2][2]*afV[2];
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
Real Mgc::Distance (const Vector3& rkPoint, const Real afQuad[10],
    Vector3& rkClosest)
{
    return Math::Sqrt(SqrDistance(rkPoint,afQuad,rkClosest));
}
//----------------------------------------------------------------------------


