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

#include "MgcIntr3DTriCone.h"
using namespace Mgc;

//----------------------------------------------------------------------------
//#define MGC_DEBUG_TRICONE
#ifdef MGC_DEBUG_TRICONE
#include "MgcRTLib.h"
static const int gs_iMaxSample = 128;
//----------------------------------------------------------------------------
static bool PointInsideCone (const Vector3& rkP, const Cone3& rkCone)
{
    Vector3 kDiff = rkP - rkCone.Vertex();
    kDiff.Unitize();
    return rkCone.Axis().Dot(kDiff) >= rkCone.CosAngle();
}
//----------------------------------------------------------------------------
static bool EdgeIntersectCone (const Vector3& rkP, const Vector3& rkE,
    const Cone3& rkCone)
{
    for (int i = 1; i < gs_iMaxSample; i++)
    {
        Real fT = i/Real(gs_iMaxSample);
        Vector3 kP = rkP + fT*rkE;
        if ( PointInsideCone(kP,rkCone) )
            return true;
    }

    return false;
}
//----------------------------------------------------------------------------
static bool TriangleIntersectCone (const Triangle3& rkTri,
    const Cone3& rkCone)
{
    for (int iT = 1; iT < gs_iMaxSample; iT++)
    {
        Real fT = iT/Real(gs_iMaxSample);
        for (int iS = 1; iS+iT < gs_iMaxSample; iS++)
        {
            Real fS = iS/Real(gs_iMaxSample);
            Vector3 kP = rkTri.Origin()+fS*rkTri.Edge0()+fT*rkTri.Edge1();
            if ( PointInsideCone(kP,rkCone) )
                return true;
        }
    }

    return false;
}
#endif
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Triangle3& rkTri, const Cone3& rkCone)
{
    // NOTE.  The following quantities computed in this function can be
    // precomputed and stored in the cone and triangle classes as an
    // optimization.
    //   1. The cone squared cosine.
    //   2. The triangle squared edge lengths |E0|^2 and |E1|^2.
    //   3. The third triangle edge E2 = E1 - E0 and squared length |E2|^2.
    //   4. The triangle normal N = Cross(E0,E1) or unitized normal
    //      N = Cross(E0,E1)/Length(Cross(E0,E1)).

    // triangle is <P0,P1,P2>, edges are E0 = P1-P0, E1=P2-P0
    int iOnConeSide = 0;
    Real fP0Test, fP1Test, fP2Test, fAdE, fEdE, fEdD, fC1, fC2;

    Real fCosSqr = rkCone.CosAngle()*rkCone.CosAngle();

    // test vertex P0
    Vector3 kDiff0 = rkTri.Origin() - rkCone.Vertex();
    Real fAdD0 = rkCone.Axis().Dot(kDiff0);
    if ( fAdD0 >= 0.0f )
    {
        // P0 is on cone side of plane
        fP0Test = fAdD0*fAdD0 - fCosSqr*(kDiff0.Dot(kDiff0));
        if ( fP0Test >= 0.0f )
        {
            // P0 is inside the cone
            return true;
        }
        else
        {
            // P0 is outside the cone, but on cone side of plane
            iOnConeSide |= 1;
        }
    }
    // else P0 is not on cone side of plane

#ifdef MGC_DEBUG_TRICONE
    assert( !PointInsideCone(rkTri.Origin(),rkCone) );
#endif

    // test vertex P1
    Vector3 kDiff1 = kDiff0 + rkTri.Edge0();
    Real fAdD1 = rkCone.Axis().Dot(kDiff1);
    if ( fAdD1 >= 0.0f )
    {
        // P1 is on cone side of plane
        fP1Test = fAdD1*fAdD1 - fCosSqr*(kDiff1.Dot(kDiff1));
        if ( fP1Test >= 0.0f )
        {
            // P1 is inside the cone
            return true;
        }
        else
        {
            // P1 is outside the cone, but on cone side of plane
            iOnConeSide |= 2;
        }
    }
    // else P1 is not on cone side of plane

#ifdef MGC_DEBUG_TRICONE
    assert( !PointInsideCone(rkTri.Origin()+rkTri.Edge0(),rkCone) );
#endif

    // test vertex P2
    Vector3 kDiff2 = kDiff0 + rkTri.Edge1();
    Real fAdD2 = rkCone.Axis().Dot(kDiff2);
    if ( fAdD2 >= 0.0f )
    {
        // P2 is on cone side of plane
        fP2Test = fAdD2*fAdD2 - fCosSqr*(kDiff2.Dot(kDiff2));
        if ( fP2Test >= 0.0f )
        {
            // P2 is inside the cone
            return true;
        }
        else
        {
            // P2 is outside the cone, but on cone side of plane
            iOnConeSide |= 4;
        }
    }
    // else P2 is not on cone side of plane


#ifdef MGC_DEBUG_TRICONE
    assert( !PointInsideCone(rkTri.Origin()+rkTri.Edge1(),rkCone) );
#endif

    // test edge <P0,P1> = E0
    if ( iOnConeSide & 3 )
    {
        fAdE = fAdD1 - fAdD0;
        fEdE = rkTri.Edge0().Dot(rkTri.Edge0());
        fC2 = fAdE*fAdE - fCosSqr*fEdE;
        if ( fC2 < 0.0f )
        {
            fEdD = rkTri.Edge0().Dot(kDiff0);
            fC1 = fAdE*fAdD0 - fCosSqr*fEdD;
            if ( iOnConeSide & 1 )
            {
                if ( iOnConeSide & 2 )
                {
                    // <P0,P1> fully on cone side of plane, fC0 = fP0Test
                    if ( 0.0f <= fC1 && fC1 <= -fC2
                    &&   fC1*fC1 >= fP0Test*fC2 )
                    {
                        return true;
                    }
                }
                else
                {
                    // P0 on cone side (Dot(A,P0-V) >= 0),
                    // P1 on opposite side (Dot(A,P1-V) <= 0)
                    // (Dot(A,E0) <= 0), fC0 = fP0Test
                    if ( 0.0f <= fC1 && fC2*fAdD0 <= fC1*fAdE
                    &&   fC1*fC1 >= fP0Test*fC2 )
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P1 on cone side (Dot(A,P1-V) >= 0),
                // P0 on opposite side (Dot(A,P0-V) <= 0)
                // (Dot(A,E0) >= 0), fC0 = fP0Test (needs calculating)
                if ( fC1 <= -fC2 && fC2*fAdD0 <= fC1*fAdE )
                {
                    fP0Test = fAdD0*fAdD0 - fCosSqr*(kDiff0.Dot(kDiff0));
                    if ( fC1*fC1 >= fP0Test*fC2 )
                        return true;
                }
            }
        }
    }
    // else <P0,P1> does not intersect cone half space

#ifdef MGC_DEBUG_TRICONE
    assert( !EdgeIntersectCone(rkTri.Origin(),rkTri.Edge0(),rkCone) );
#endif

    // test edge <P0,P2> = E1
    if ( iOnConeSide & 5 )
    {
        fAdE = fAdD2 - fAdD0;
        fEdE = rkTri.Edge1().Dot(rkTri.Edge1());
        fC2 = fAdE*fAdE - fCosSqr*fEdE;
        if ( fC2 < 0.0f )
        {
            fEdD = rkTri.Edge1().Dot(kDiff0);
            fC1 = fAdE*fAdD0 - fCosSqr*fEdD;
            if ( iOnConeSide & 1 )
            {
                if ( iOnConeSide & 4 )
                {
                    // <P0,P2> fully on cone side of plane, fC0 = fP0Test
                    if ( 0.0f <= fC1 && fC1 <= -fC2
                    &&   fC1*fC1 >= fP0Test*fC2 )
                    {
                        return true;
                    }
                }
                else
                {
                    // P0 on cone side (Dot(A,P0-V) >= 0),
                    // P2 on opposite side (Dot(A,P2-V) <= 0)
                    // (Dot(A,E1) <= 0), fC0 = fP0Test
                    if ( 0.0f <= fC1 && fC2*fAdD0 <= fC1*fAdE
                    &&   fC1*fC1 >= fP0Test*fC2 )
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P2 on cone side (Dot(A,P2-V) >= 0),
                // P0 on opposite side (Dot(A,P0-V) <= 0)
                // (Dot(A,E1) >= 0), fC0 = fP0Test (needs calculating)
                if ( fC1 <= -fC2 && fC2*fAdD0 <= fC1*fAdE )
                {
                    fP0Test = fAdD0*fAdD0 - fCosSqr*(kDiff0.Dot(kDiff0));
                    if ( fC1*fC1 >= fP0Test*fC2 )
                        return true;
                }
            }
        }
    }
    // else <P0,P2> does not intersect cone half space

#ifdef MGC_DEBUG_TRICONE
    assert( !EdgeIntersectCone(rkTri.Origin(),rkTri.Edge1(),rkCone) );
#endif

    // test edge <P1,P2> = E1-E0 = E2
    if ( iOnConeSide & 6 )
    {
        Vector3 kE2 = rkTri.Edge1() - rkTri.Edge0();
        fAdE = fAdD2 - fAdD1;
        fEdE = kE2.Dot(kE2);
        fC2 = fAdE*fAdE - fCosSqr*fEdE;
        if ( fC2 < 0.0f )
        {
            fEdD = kE2.Dot(kDiff1);
            fC1 = fAdE*fAdD1 - fCosSqr*fEdD;
            if ( iOnConeSide & 2 )
            {
                if ( iOnConeSide & 4 )
                {
                    // <P1,P2> fully on cone side of plane, fC0 = fP1Test
                    if ( 0.0f <= fC1 && fC1 <= -fC2
                    &&   fC1*fC1 >= fP1Test*fC2 )
                    {
                        return true;
                    }
                }
                else
                {
                    // P1 on cone side (Dot(A,P1-V) >= 0),
                    // P2 on opposite side (Dot(A,P2-V) <= 0)
                    // (Dot(A,E2) <= 0), fC0 = fP1Test
                    if ( 0.0f <= fC1 && fC2*fAdD1 <= fC1*fAdE
                    &&   fC1*fC1 >= fP1Test*fC2 )
                    {
                        return true;
                    }
                }
            }
            else
            {
                // P2 on cone side (Dot(A,P2-V) >= 0),
                // P1 on opposite side (Dot(A,P1-V) <= 0)
                // (Dot(A,E2) >= 0), fC0 = fP1Test (needs calculating)
                if ( fC1 <= -fC2 && fC2*fAdD1 <= fC1*fAdE )
                {
                    fP1Test = fAdD1*fAdD1 - fCosSqr*(kDiff1.Dot(kDiff1));
                    if ( fC1*fC1 >= fP1Test*fC2 )
                        return true;
                }
            }
        }
    }
    // else <P1,P2> does not intersect cone half space

#ifdef MGC_DEBUG_TRICONE
    assert( !EdgeIntersectCone(rkTri.Origin()+rkTri.Edge0(),rkTri.Edge1() -
        rkTri.Edge0(),rkCone) );
#endif

    // Test triangle <P0,P1,P2>.  It is enough to handle only the case when
    // at least one Pi is on the cone side of the plane.  In this case and
    // after the previous testing, if the triangle intersects the cone, the
    // set of intersection must contain the point of intersection between
    // the cone axis and the triangle.
    if ( iOnConeSide > 0 )
    {
        Vector3 kN = rkTri.Edge0().Cross(rkTri.Edge1());
        Real fNdA = kN.Dot(rkCone.Axis());
        Real fNdD = kN.Dot(kDiff0);
        Vector3 kU = fNdD*rkCone.Axis() - fNdA*kDiff0;
        Vector3 kNcU = kN.Cross(kU);

        Real fNcUdE0 = kNcU.Dot(rkTri.Edge0()), fNcUdE1, fNcUdE2, fNdN;
        if ( fNdA >= 0.0f )
        {
            if ( fNcUdE0 <= 0.0f )
            {
                fNcUdE1 = kNcU.Dot(rkTri.Edge1());
                if ( fNcUdE1 >= 0.0f )
                {
                    fNcUdE2 = fNcUdE1 - fNcUdE0;
                    fNdN = kN.SquaredLength();
                    if ( fNcUdE2 <= fNdA*fNdN )
                        return true;
                }
            }
        }
        else
        {
            if ( fNcUdE0 >= 0.0f )
            {
                fNcUdE1 = kNcU.Dot(rkTri.Edge1());
                if ( fNcUdE1 <= 0.0f )
                {
                    fNcUdE2 = fNcUdE1 - fNcUdE0;
                    fNdN = kN.SquaredLength();
                    if ( fNcUdE2 >= fNdA*fNdN )
                        return true;
                }
            }
        }
    }

#ifdef MGC_DEBUG_TRICONE
    assert( !TriangleIntersectCone(rkTri,rkCone) );
#endif

    return false;
}
//----------------------------------------------------------------------------

