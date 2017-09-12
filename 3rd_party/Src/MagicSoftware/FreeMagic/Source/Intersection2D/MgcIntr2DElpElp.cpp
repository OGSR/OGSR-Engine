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

#include "MgcIntr2DElpElp.h"
#include "MgcPolynomial.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void GetPoly (const QuadPoly2& rkP0, const QuadPoly2& rkP1,
    Polynomial& rkPoly)
{
    // polynomial is constructed as a Bezout determinant
    Real fAB = rkP0.m_fA*rkP1.m_fB-rkP1.m_fA*rkP0.m_fB;
    Real fAC = rkP0.m_fA*rkP1.m_fC-rkP1.m_fA*rkP0.m_fC;
    Real fAD = rkP0.m_fA*rkP1.m_fD-rkP1.m_fA*rkP0.m_fD;
    Real fAE = rkP0.m_fA*rkP1.m_fE-rkP1.m_fA*rkP0.m_fE;
    Real fAF = rkP0.m_fA*rkP1.m_fF-rkP1.m_fA*rkP0.m_fF;
    Real fBC = rkP0.m_fB*rkP1.m_fC-rkP1.m_fB*rkP0.m_fC;
    Real fBE = rkP0.m_fB*rkP1.m_fE-rkP1.m_fB*rkP0.m_fE;
    Real fBF = rkP0.m_fB*rkP1.m_fF-rkP1.m_fB*rkP0.m_fF;
    Real fCD = rkP0.m_fC*rkP1.m_fD-rkP1.m_fC*rkP0.m_fD;
    Real fDE = rkP0.m_fD*rkP1.m_fE-rkP1.m_fD*rkP0.m_fE;
    Real fDF = rkP0.m_fD*rkP1.m_fF-rkP1.m_fD*rkP0.m_fF;
    Real fBFpDE = fBF+fDE;
    Real fBEmCD = fBE-fCD;

    Real afU[5] =
    {
        fAD*fDF-fAF*fAF,
        fAB*fDF+fAD*fBFpDE-2.0f*fAE*fAF,
        fAB*fBFpDE+fAD*fBEmCD-fAE*fAE-2.0f*fAC*fAF,
        fAB*fBEmCD+fAD*fBC-2.0f*fAC*fAE,
        fAB*fBC-fAC*fAC
    };

    // reduce degree if necessary
    const Real fEpsilon = 1e-06f;
    int iDegree = 4;
    while ( iDegree > 0 && Math::FAbs(afU[iDegree]) < fEpsilon )
        iDegree--;

    rkPoly.SetDegree(iDegree);
    memcpy(&rkPoly[0],afU,(iDegree+1)*sizeof(Real));
}
//----------------------------------------------------------------------------
bool QuadPoly2::IsEllipse ()
{
    return m_fB*m_fB < 4.0f*m_fA*m_fC;
}
//----------------------------------------------------------------------------
void Mgc::FindIntersection (const QuadPoly2& rkE0, const QuadPoly2& rkE1,
    int& riQuantity, Vector2 akP[4])
{
    Polynomial kPoly;
    GetPoly(rkE0,rkE1,kPoly);

    int iYCount;
    Real afY[4];
    Polynomial::ZERO_TOLERANCE = 1e-04f;
    switch ( kPoly.GetDegree() )
    {
    case 4: kPoly.RootsDegree4(iYCount,afY); break;
    case 3: kPoly.RootsDegree3(iYCount,afY); break;
    case 2: kPoly.RootsDegree2(iYCount,afY); break;
    case 1:
        iYCount = 1;
        afY[0] = -kPoly[0]/kPoly[1];
        break;
    case 0:
        riQuantity = 0;
        return;
    }

    // Adjustment for quadratics to allow for relative error testing.
    Real fNorm0 = rkE0.m_fA*rkE0.m_fA + 2.0f*rkE0.m_fB*rkE0.m_fB +
        rkE0.m_fC*rkE0.m_fC;
    Real fNorm1 = rkE1.m_fA*rkE1.m_fA + 2.0f*rkE1.m_fB*rkE1.m_fB +
        rkE1.m_fC*rkE1.m_fC;

    riQuantity = 0;
    int iXCount;
    Real afX[2];
    for (int iY = 0; iY < iYCount; iY++)
    {
        Polynomial kAPoly(2);
        kAPoly[0] = rkE0.m_fF+afY[iY]*(rkE0.m_fE+afY[iY]*rkE0.m_fC);
        kAPoly[1] = rkE0.m_fD+afY[iY]*rkE0.m_fB;
        kAPoly[2] = rkE0.m_fA;
        kAPoly.GetAllRoots(iXCount,afX);
        for (int iX = 0; iX < iXCount; iX++)
        {
            const Real fEpsilon = 1e-03f;
            Real fTest = (rkE0.m_fA*afX[iX] + rkE0.m_fB*afY[iY] +
                rkE0.m_fD)*afX[iX] + (rkE0.m_fC*afY[iY] + rkE0.m_fE)*afY[iY]+
                rkE0.m_fF;
            if ( Math::FAbs(fTest) < fEpsilon*fNorm0 )
            {
                fTest = (rkE1.m_fA*afX[iX] + rkE1.m_fB*afY[iY] + rkE1.m_fD)*
                    afX[iX] + (rkE1.m_fC*afY[iY] + rkE1.m_fE)*afY[iY] +
                    rkE1.m_fF;
                if ( Math::FAbs(fTest) < fEpsilon*fNorm1 )
                {
                    akP[riQuantity].x = afX[iX];
                    akP[riQuantity].y = afY[iY];
                    riQuantity++;
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const QuadPoly2& rkE0, const QuadPoly2& rkE1)
{
    Polynomial kPoly;
    GetPoly(rkE0,rkE1,kPoly);
    return kPoly.GetRootCount(-Math::MAX_REAL,Math::MAX_REAL) > 0;
}
//----------------------------------------------------------------------------


