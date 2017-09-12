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

#include "MgcCont2DMinCircle.h"
using namespace Mgc;

// error checking
static const Real gs_fEpsilon = 1e-05f;

// indices of points that support current minimum area circle
class Support
{
public:
    int m_iQuantity;
    int m_aiIndex[3];

    bool Contains (int iIndex, Vector2** apkPoint)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            Vector2 kDiff = *apkPoint[iIndex] - *apkPoint[m_aiIndex[i]];
            if ( kDiff.SquaredLength() < gs_fEpsilon )
                return true;
        }
        return false;
    }
};

// All internal minimal circle calculations store the squared radius in the
// radius member of Circle2.  Only at the end is a sqrt computed.

//----------------------------------------------------------------------------
static bool PointInsideCircle (const Vector2& rkP, const Circle2& rkC,
    Real& rfDistDiff)
{
    Vector2 kDiff = rkP - rkC.Center();
    Real fTest = kDiff.SquaredLength();
    rfDistDiff = fTest - rkC.Radius();

    // Testing |Point-Center|^2 <= Radius^2 allowing a small relative error.
    return rfDistDiff <= 0.0f;
}
//----------------------------------------------------------------------------
static Circle2 ExactCircle1 (const Vector2& rkP)
{
    Circle2 kMinimal;
    kMinimal.Center() = rkP;
    kMinimal.Radius() = 0.0f;
    return kMinimal;
}
//----------------------------------------------------------------------------
static Circle2 ExactCircle2 (const Vector2& rkP0, const Vector2& rkP1)
{
    Circle2 kMinimal;
    kMinimal.Center() = 0.5f*(rkP0 + rkP1);
    Vector2 kDiff = rkP1 - rkP0;
    kMinimal.Radius() = 0.25f*kDiff.SquaredLength();
    return kMinimal;
}
//----------------------------------------------------------------------------
static Circle2 ExactCircle3 (const Vector2& rkP0, const Vector2& rkP1,
    const Vector2& rkP2)
{
    Vector2 kE10 = rkP1 - rkP0;
    Vector2 kE20 = rkP2 - rkP0;

    Real aafA[2][2];
    aafA[0][0] = kE10.x;  aafA[0][1] = kE10.y;
    aafA[1][0] = kE20.x;  aafA[1][1] = kE20.y;

    Real afB[2];
    afB[0] = 0.5f*kE10.SquaredLength();
    afB[1] = 0.5f*kE20.SquaredLength();

    Circle2 kMinimal;
    Real fDet = aafA[0][0]*aafA[1][1]-aafA[0][1]*aafA[1][0];

    if ( Math::FAbs(fDet) > gs_fEpsilon )
    {
        Real fInvDet = 1.0f/fDet;
        Vector2 kQ;
        kQ.x = (aafA[1][1]*afB[0]-aafA[0][1]*afB[1])*fInvDet;
        kQ.y = (aafA[0][0]*afB[1]-aafA[1][0]*afB[0])*fInvDet;
        kMinimal.Center() = rkP0 + kQ;
        kMinimal.Radius() = kQ.SquaredLength();
    }
    else
    {
        kMinimal.Center() = Vector2::ZERO;
        kMinimal.Radius() = Math::MAX_REAL;
    }

    return kMinimal;
}
//----------------------------------------------------------------------------
static Circle2 UpdateSupport1 (int i, Vector2** apkPerm, Support& rkSupp)
{
    const Vector2& rkP0 = *apkPerm[rkSupp.m_aiIndex[0]];
    const Vector2& rkP1 = *apkPerm[i];

    Circle2 kMinimal = ExactCircle2(rkP0,rkP1);
    rkSupp.m_iQuantity = 2;
    rkSupp.m_aiIndex[1] = i;

    return kMinimal;
}
//----------------------------------------------------------------------------
static Circle2 UpdateSupport2 (int i, Vector2** apkPerm, Support& rkSupp)
{
    const Vector2& rkP0 = *apkPerm[rkSupp.m_aiIndex[0]];
    const Vector2& rkP1 = *apkPerm[rkSupp.m_aiIndex[1]];
    const Vector2& rkP2 = *apkPerm[i];

    Circle2 akC[3];
    Real fMinRSqr = Math::MAX_REAL;
    Real fDistDiff;
    int iIndex = -1;

    akC[0] = ExactCircle2(rkP0,rkP2);
    if ( PointInsideCircle(rkP1,akC[0],fDistDiff) )
    {
        fMinRSqr = akC[0].Radius();
        iIndex = 0;
    }

    akC[1] = ExactCircle2(rkP1,rkP2);
    if ( akC[1].Radius() < fMinRSqr
    &&   PointInsideCircle(rkP0,akC[1],fDistDiff) )
    {
        fMinRSqr = akC[1].Radius();
        iIndex = 1;
    }

    Circle2 kMinimal;

    if ( iIndex != -1 )
    {
        kMinimal = akC[iIndex];
        rkSupp.m_aiIndex[1-iIndex] = i;
    }
    else
    {
        kMinimal = ExactCircle3(rkP0,rkP1,rkP2);
        assert( kMinimal.Radius() <= fMinRSqr );
        rkSupp.m_iQuantity = 3;
        rkSupp.m_aiIndex[2] = i;
    }

    return kMinimal;
}
//----------------------------------------------------------------------------
static Circle2 UpdateSupport3 (int i, Vector2** apkPerm, Support& rkSupp)
{
    const Vector2& rkP0 = *apkPerm[rkSupp.m_aiIndex[0]];
    const Vector2& rkP1 = *apkPerm[rkSupp.m_aiIndex[1]];
    const Vector2& rkP2 = *apkPerm[rkSupp.m_aiIndex[2]];
    const Vector2& rkP3 = *apkPerm[i];

    Circle2 akC[6];
    Real fMinRSqr = Math::MAX_REAL;
    int iIndex = -1;
    Real fDistDiff, fMinDistDiff = Math::MAX_REAL;
    int iMinIndex = -1;

    akC[0] = ExactCircle2(rkP0,rkP3);
    if ( PointInsideCircle(rkP1,akC[0],fDistDiff) )
    {
        if ( PointInsideCircle(rkP2,akC[0],fDistDiff) )
        {
            fMinRSqr = akC[0].Radius();
            iIndex = 0;
        }
        else
        {
            fMinDistDiff = fDistDiff;
            iMinIndex = 0;
        }
    }
    else
    {
        fMinDistDiff = fDistDiff;
        iMinIndex = 0;
    }

    akC[1] = ExactCircle2(rkP1,rkP3);
    if ( akC[1].Radius() < fMinRSqr )
    {
        if ( PointInsideCircle(rkP0,akC[1],fDistDiff) )
        {
            if ( PointInsideCircle(rkP2,akC[1],fDistDiff) )
            {
                fMinRSqr = akC[1].Radius();
                iIndex = 1;
            }
            else if ( fDistDiff < fMinDistDiff )
            {
                fMinDistDiff = fDistDiff;
                iMinIndex = 1;
            }
        }
        else if ( fDistDiff < fMinDistDiff )
        {
            fMinDistDiff = fDistDiff;
            iMinIndex = 1;
        }
    }

    akC[2] = ExactCircle2(rkP2,rkP3);
    if ( akC[2].Radius() < fMinRSqr )
    {
        if ( PointInsideCircle(rkP0,akC[2],fDistDiff) )
        {
            if ( PointInsideCircle(rkP1,akC[2],fDistDiff) )
            {
                fMinRSqr = akC[2].Radius();
                iIndex = 2;
            }
            else if ( fDistDiff < fMinDistDiff )
            {
                fMinDistDiff = fDistDiff;
                iMinIndex = 2;
            }
        }
        else if ( fDistDiff < fMinDistDiff )
        {
            fMinDistDiff = fDistDiff;
            iMinIndex = 2;
        }
    }

    akC[3] = ExactCircle3(rkP0,rkP1,rkP3);
    if ( akC[3].Radius() < fMinRSqr )
    {
        if ( PointInsideCircle(rkP2,akC[3],fDistDiff) )
        {
            fMinRSqr = akC[3].Radius();
            iIndex = 3;
        }
        else if ( fDistDiff < fMinDistDiff )
        {
            fMinDistDiff = fDistDiff;
            iMinIndex = 3;
        }
    }

    akC[4] = ExactCircle3(rkP0,rkP2,rkP3);
    if ( akC[4].Radius() < fMinRSqr )
    {
        if ( PointInsideCircle(rkP1,akC[4],fDistDiff) )
        {
            fMinRSqr = akC[4].Radius();
            iIndex = 4;
        }
        else if ( fDistDiff < fMinDistDiff )
        {
            fMinDistDiff = fDistDiff;
            iMinIndex = 4;
        }
    }

    akC[5] = ExactCircle3(rkP1,rkP2,rkP3);
    if ( akC[5].Radius() < fMinRSqr )
    {
        if ( PointInsideCircle(rkP0,akC[5],fDistDiff) )
        {
            fMinRSqr = akC[5].Radius();
            iIndex = 5;
        }
        else if ( fDistDiff < fMinDistDiff )
        {
            fMinDistDiff = fDistDiff;
            iMinIndex = 5;
        }
    }

    // Theoretically, iIndex >= 0 should happen, but floating point round-off
    // error can lead to this.  When this happens, the circle is chosen for
    // which the minimum absolute errors between barely outside points and
    // that circle.
    assert( iIndex != -1 || iMinIndex != -1 );
    if ( iIndex == -1 )
    {
        iIndex = iMinIndex;
    }

    Circle2 kMinimal = akC[iIndex];

    switch ( iIndex )
    {
    case 0:
        rkSupp.m_iQuantity = 2;
        rkSupp.m_aiIndex[1] = i;
        break;
    case 1:
        rkSupp.m_iQuantity = 2;
        rkSupp.m_aiIndex[0] = i;
        break;
    case 2:
        rkSupp.m_iQuantity = 2;
        rkSupp.m_aiIndex[0] = rkSupp.m_aiIndex[2];
        rkSupp.m_aiIndex[1] = i;
        break;
    case 3:
        rkSupp.m_aiIndex[2] = i;
        break;
    case 4:
        rkSupp.m_aiIndex[1] = i;
        break;
    case 5:
        rkSupp.m_aiIndex[0] = i;
        break;
    }

    return kMinimal;
}
//----------------------------------------------------------------------------
typedef Circle2 (*UpdateFunction)(int,Vector2**,Support&);
static UpdateFunction gs_aoUpdate[4] =
{
    NULL,
    UpdateSupport1,
    UpdateSupport2,
    UpdateSupport3
};
//----------------------------------------------------------------------------
Circle2 Mgc::MinCircle (int iQuantity, const Vector2* akPoint)
{
    // initialize random number generator
    static bool s_bFirstTime = true;
    if ( s_bFirstTime )
    {
        srand(367);
        s_bFirstTime = false;
    }

    Circle2 kMinimal;
    Support kSupp;
    Real fDistDiff;

    if ( iQuantity >= 1 )
    {
        // create identity permutation (0,1,...,iQuantity-1)
        Vector2** apkPerm = new Vector2*[iQuantity];
        int i;
        for (i = 0; i < iQuantity; i++)
            apkPerm[i] = (Vector2*) &akPoint[i];
        
        // generate random permutation
        for (i = iQuantity-1; i > 0; i--)
        {
            int j = rand() % (i+1);
            if ( j != i )
            {
                Vector2* pSave = apkPerm[i];
                apkPerm[i] = apkPerm[j];
                apkPerm[j] = pSave;
            }
        }
        
        kMinimal = ExactCircle1(*apkPerm[0]);
        kSupp.m_iQuantity = 1;
        kSupp.m_aiIndex[0] = 0;
        i = 1;
        while ( i < iQuantity )
        {
            if ( !kSupp.Contains(i,apkPerm) )
            {
                if ( !PointInsideCircle(*apkPerm[i],kMinimal,fDistDiff) )
                {
                    Circle2 kCir = gs_aoUpdate[kSupp.m_iQuantity](i,apkPerm,
                        kSupp);
                    if ( kCir.Radius() > kMinimal.Radius() )
                    {
                        kMinimal = kCir;
                        i = 0;
                        continue;
                    }
                }
            }
            i++;
        }
        
        delete[] apkPerm;
    }
    else
    {
        assert( false );
    }

    kMinimal.Radius() = Math::Sqrt(kMinimal.Radius());
    return kMinimal;
}
//----------------------------------------------------------------------------


