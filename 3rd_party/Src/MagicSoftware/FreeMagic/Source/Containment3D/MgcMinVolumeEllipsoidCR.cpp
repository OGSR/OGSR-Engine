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

#include "MgcMath.h"
#include "MgcMinVolumeEllipsoidCR.h"
using namespace Mgc;

static const Real gs_fEpsilon = 1e-08f;

// forward reference for recursive chain with FindEdgeMax
static void FindFacetMax (int,Real*,Real*,Real*,int&,Real&,Real&,Real&);

//----------------------------------------------------------------------------
#ifdef _DEBUG
static Real gs_fSaveMax = 0.0f;
static void IsOkay (int iQuantity, Real* afA, Real* afB, Real* afC,
    Real fX0, Real fY0, Real fZ0)
{
    const Real fTolerance = 1e-06f;
    Real fMax = fX0*fY0*fZ0;
    assert( fMax + gs_fEpsilon >= gs_fSaveMax );
    gs_fSaveMax = fMax;

    for (int i = 0; i < iQuantity; i++)
    {
        Real fTest = afA[i]*fX0 + afB[i]*fY0 + afC[i]*fZ0;
        assert( fTest <= 1.0f + fTolerance );
    }
}
#endif
//----------------------------------------------------------------------------
static void FindEdgeMax (int iQuantity, Real* afA, Real* afB, Real* afC,
    int& riPlane0, int& riPlane1, Real& rfX0, Real& rfY0, Real& rfZ0)
{
    // compute direction to local maximum point on line of intersection
    Real fXDir = afB[riPlane0]*afC[riPlane1]-afB[riPlane1]*afC[riPlane0];
    Real fYDir = afC[riPlane0]*afA[riPlane1]-afC[riPlane1]*afA[riPlane0];
    Real fZDir = afA[riPlane0]*afB[riPlane1]-afA[riPlane1]*afB[riPlane0];

    // build quadratic Q'(t) = (d/dt)(x(t)y(t)z(t)) = a0+a1*t+a2*t^2
    Real fA0 = rfX0*rfY0*fZDir+rfX0*rfZ0*fYDir+rfY0*rfZ0*fXDir;
    Real fA1 = 2.0f*(rfZ0*fXDir*fYDir+rfY0*fXDir*fZDir+rfX0*fYDir*fZDir);
    Real fA2 = 3.0f*(fXDir*fYDir*fZDir);

    // find root to Q'(t) = 0 corresponding to maximum
    Real fTFinal;
    if ( fA2 != 0.0f )
    {
        Real fDiscr = fA1*fA1 - 4.0f*fA0*fA2;
        assert( fDiscr >= 0.0f );
        fDiscr = Math::Sqrt(fDiscr);
        fTFinal = -0.5f*(fA1 + fDiscr)/fA2;
        if ( fA1 + 2.0f*fA2*fTFinal > 0.0f )
            fTFinal = 0.5f*(-fA1 + fDiscr)/fA2;
    }
    else if ( fA1 != 0.0f )
    {
        fTFinal = -fA0/fA1;
    }
    else if ( fA0 != 0.0f )
    {
        fTFinal = ( fA0 >= 0.0f ? Math::MAX_REAL : -Math::MAX_REAL );
    }
    else
    {
        return;
    }

    if ( fTFinal < 0.0f )
    {
        // make (xDir,yDir,zDir) point in direction of increase of Q
        fTFinal = -fTFinal;
        fXDir = -fXDir;
        fYDir = -fYDir;
        fZDir = -fZDir;
    }

    // sort remaining planes along line from current point to
    // local maximum
    Real fTMax = fTFinal;
    int iPlane2 = -1;
    for (int i = 0; i < iQuantity; i++)
    {
        if ( i == riPlane0 || i == riPlane1 )
            continue;

        Real fNorDotDir = afA[i]*fXDir + afB[i]*fYDir + afC[i]*fZDir;
        if ( fNorDotDir <= 0.0f )
            continue;

        // Theoretically the numerator must be nonnegative since an
        // invariant in the algorithm is that (x0,y0,z0) is on the
        // convex hull of the constraints.  However, some numerical
        // error may make this a small negative number.  In that case
        // set tmax = 0 (no change in position).
        Real fNumer = 1.0f - afA[i]*rfX0 - afB[i]*rfY0 - afC[i]*rfZ0;
        if ( fNumer < 0.0f )
        {
            assert( fNumer >= -1e-04f );
            iPlane2 = i;
            fTMax = 0.0f;
            break;
        }

        Real fT = fNumer/fNorDotDir;
        if ( 0 <= fT && fT < fTMax )
        {
            iPlane2 = i;
            fTMax = fT;
        }
    }

    rfX0 += fTMax*fXDir;
    rfY0 += fTMax*fYDir;
    rfZ0 += fTMax*fZDir;
#ifdef _DEBUG
    IsOkay(iQuantity,afA,afB,afC,rfX0,rfY0,rfZ0);
#endif

    if ( fTMax == fTFinal )
        return;

    if ( fTMax > gs_fEpsilon )
    {
        riPlane0 = iPlane2;
        FindFacetMax(iQuantity,afA,afB,afC,riPlane0,rfX0,rfY0,rfZ0);
        return;
    }

    // tmax == 0.0, return with x0, y0, z0 unchanged
#ifdef _DEBUG
    IsOkay(iQuantity,afA,afB,afC,rfX0,rfY0,rfZ0);
#endif
}
//----------------------------------------------------------------------------
static void FindFacetMax (int iQuantity, Real* afA, Real* afB, Real* afC,
    int& riPlane0, Real& rfX0, Real& rfY0, Real& rfZ0)
{
    Real fTFinal, fXDir, fYDir, fZDir;

    if ( afA[riPlane0] > gs_fEpsilon
    &&   afB[riPlane0] > gs_fEpsilon
    &&   afC[riPlane0] > gs_fEpsilon )
    {
        // compute local maximum point on plane
        static const Real s_fOneThird = 1.0f/3.0f;
        Real fX1 = s_fOneThird/afA[riPlane0];
        Real fY1 = s_fOneThird/afB[riPlane0];
        Real fZ1 = s_fOneThird/afC[riPlane0];
        
        // compute direction to local maximum point on plane
        fTFinal = 1.0f;
        fXDir = fX1 - rfX0;
        fYDir = fY1 - rfY0;
        fZDir = fZ1 - rfZ0;
    }
    else
    {
        fTFinal = Math::MAX_REAL;
        fXDir = ( afA[riPlane0] > gs_fEpsilon ? 0.0f : 1.0f );
        fYDir = ( afB[riPlane0] > gs_fEpsilon ? 0.0f : 1.0f );
        fZDir = ( afC[riPlane0] > gs_fEpsilon ? 0.0f : 1.0f );
    }
    
    // sort remaining planes along line from current point
    Real fTMax = fTFinal;
    int iPlane1 = -1;
    for (int i = 0; i < iQuantity; i++)
    {
        if ( i == riPlane0 )
            continue;

        Real fNorDotDir = afA[i]*fXDir + afB[i]*fYDir + afC[i]*fZDir;
        if ( fNorDotDir <= 0.0f )
            continue;

        // Theoretically the numerator must be nonnegative since an
        // invariant in the algorithm is that (x0,y0,z0) is on the
        // convex hull of the constraints.  However, some numerical
        // error may make this a small negative number.  In that case
        // set tmax = 0 (no change in position).
        Real fNumer = 1.0f - afA[i]*rfX0 - afB[i]*rfY0 - afC[i]*rfZ0;
        if ( fNumer < 0.0f )
        {
            assert( fNumer >= -1e-04f );
            iPlane1 = i;
            fTMax = 0.0f;
            break;
        }

        Real fT = fNumer/fNorDotDir;
        if ( 0 <= fT && fT < fTMax )
        {
            iPlane1 = i;
            fTMax = fT;
        }
    }

    rfX0 += fTMax*fXDir;
    rfY0 += fTMax*fYDir;
    rfZ0 += fTMax*fZDir;
#ifdef _DEBUG
    IsOkay(iQuantity,afA,afB,afC,rfX0,rfY0,rfZ0);
#endif

    if ( fTMax == 1.0f )
        return;

    if ( fTMax > gs_fEpsilon )
    {
        riPlane0 = iPlane1;
        FindFacetMax(iQuantity,afA,afB,afC,riPlane0,rfX0,rfY0,rfZ0);
        return;
    }

    FindEdgeMax(iQuantity,afA,afB,afC,riPlane0,iPlane1,rfX0,rfY0,rfZ0);
}
//----------------------------------------------------------------------------
static void MaxProduct (int iQuantity, Real* afA, Real* afB, Real* afC,
    Real& rfX, Real& rfY, Real& rfZ)
{
    // Maximize x*y*z subject to x >= 0, y >= 0, z >= 0, and
    // A[i]*x+B[i]*y+C[i]*z <= 1 for 0 <= i < N where A[i] >= 0,
    // B[i] >= 0, and C[i] >= 0.

    // Jitter the lines to avoid cases where more than three planes
    // intersect at the same point.  Should also break parallelism
    // and planes parallel to the coordinate planes.
    const Real fMaxJitter = 1e-12f;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        afA[i] += fMaxJitter*Math::UnitRandom();
        afB[i] += fMaxJitter*Math::UnitRandom();
        afC[i] += fMaxJitter*Math::UnitRandom();
    }

    // sort lines along rfZ-axis (rfX=0 and rfY=0)
    int iPlane = -1;
    Real fCMax = 0.0f;
    for (i = 0; i < iQuantity; i++)
    {
        if ( afC[i] > fCMax )
        {
            fCMax = afC[i];
            iPlane = i;
        }
    }
    assert( iPlane != -1 );

    // walk along convex hull searching for maximum
#ifdef _DEBUG
    gs_fSaveMax = 0.0f;
#endif
    rfX = 0.0f;
    rfY = 0.0f;
    rfZ = 1.0f/fCMax;
    FindFacetMax(iQuantity,afA,afB,afC,iPlane,rfX,rfY,rfZ);
}
//----------------------------------------------------------------------------
void Mgc::MinVolumeEllipsoidCR (int iQuantity, const Vector3* akPoint,
    const Vector3& rkC, const Matrix3& rkR, Real afD[3])
{
    // Given center C and orientation R, finds minimum volume ellipsoid
    // (X-C)^t R^t D R (X-C) = 1 where D is a diagonal matrix whose diagonal
    // entries are positive.  The problem is equivalent to maximizing the
    // product D[0]*D[1]*D[2] given C and R and subject to the constraints
    // (P[i]-C)^t R^t D R (P[i]-C) <= 1 for all input points P[i] with
    // 0 <= i < N.  Each constraint has form a0*D[0]+a1*D[1]+a2*D[2] <= 1
    // where a0 >= 0, a1 >= 0, and a2 >= 0.

    Real* afA0 = new Real[iQuantity];
    Real* afA1 = new Real[iQuantity];
    Real* afA2 = new Real[iQuantity];

    for (int i = 0; i < iQuantity; i++)
    {
        Vector3 kDiff = akPoint[i] - rkC;
        Vector3 kProd = rkR*kDiff;

        afA0[i] = kProd.x*kProd.x;
        afA1[i] = kProd.y*kProd.y;
        afA2[i] = kProd.z*kProd.z;
    }

    MaxProduct(iQuantity,afA0,afA1,afA2,afD[0],afD[1],afD[2]);

    delete[] afA2;
    delete[] afA1;
    delete[] afA0;
}
//----------------------------------------------------------------------------


