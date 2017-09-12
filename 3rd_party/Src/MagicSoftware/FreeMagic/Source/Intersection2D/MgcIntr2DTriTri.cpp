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

#include "MgcIntervalIntersection.h"
#include "MgcIntr2DTriTri.h"
using namespace Mgc;

//----------------------------------------------------------------------------
// stationary triangles
//----------------------------------------------------------------------------
static int WhichSide (const Vector2 akV[3], const Vector2& rkP,
    const Vector2& rkD)
{
    // Vertices are projected to the form P+t*D.  Return value is +1 if all
    // t > 0, -1 if all t < 0, 0 otherwise, in which case the line splits the
    // triangle.

    int iPositive = 0, iNegative = 0, iZero = 0;

    for (int i = 0; i < 3; i++)
    {
        Real fT = rkD.Dot(akV[i] - rkP);
        if ( fT > 0.0f )
            iPositive++;
        else if ( fT < 0.0f )
            iNegative++;
        else
            iZero++;

        if ( iPositive > 0 && iNegative > 0 )
            return 0;
    }

    return ( iZero == 0 ? ( iPositive > 0 ? 1 : -1 ) : 0 );
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Vector2 akV0[3], const Vector2 akV1[3])
{
    int iI0, iI1;
    Vector2 kD;

    // test edges of V0-triangle for separation
    for (iI0 = 0, iI1 = 2; iI0 < 3; iI1 = iI0, iI0++)
    {
        // test axis V0[i1] + t*perp(V0[i0]-V0[i1]), perp(x,y) = (y,-x)
        kD.x = akV0[iI0].y - akV0[iI1].y;
        kD.y = akV0[iI1].x - akV0[iI0].x;
        if ( WhichSide(akV1,akV0[iI1],kD) > 0 )
        {
            // V1-triangle is entirely on positive side of V0-triangle
            return false;
        }
    }

    // test edges of V1-triangle for separation
    for (iI0 = 0, iI1 = 2; iI0 < 3; iI1 = iI0, iI0++)
    {
        // test axis V1[i1] + t*perp(V1[i0]-V1[i1]), perp(x,y) = (y,-x)
        kD.x = akV1[iI0].y - akV1[iI1].y;
        kD.y = akV1[iI1].x - akV1[iI0].x;
        if ( WhichSide(akV0,akV1[iI1],kD) > 0 )
        {
            // V0-triangle is entirely on positive side of V1-triangle
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
static void ClipConvexPolygonAgainstLine (const Vector2& rkN, Real fC,
    int& riQuantity, Vector2 akV[6])
{
    // The input vertices are assumed to be in counterclockwise order.  The
    // ordering is an invariant of this function.

    // test on which side of line are the vertices
    int iPositive = 0, iNegative = 0, iPIndex = -1;
    Real afTest[6];
    int i;
    for (i = 0; i < riQuantity; i++)
    {
        afTest[i] = rkN.Dot(akV[i]) - fC;
        if ( afTest[i] > 0.0f )
        {
            iPositive++;
            if ( iPIndex < 0 )
                iPIndex = i;
        }
        else if ( afTest[i] < 0.0f )
        {
            iNegative++;
        }
    }

    if ( iPositive > 0 )
    {
        if ( iNegative > 0 )
        {
            // line transversely intersects polygon
            Vector2 akCV[6];
            int iCQuantity = 0, iCur, iPrv;
            Real fT;

            if ( iPIndex > 0 )
            {
                // first clip vertex on line
                iCur = iPIndex;
                iPrv = iCur-1;
                fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);

                // vertices on positive side of line
                while ( iCur < riQuantity && afTest[iCur] > 0.0f )
                    akCV[iCQuantity++] = akV[iCur++];

                // last clip vertex on line
                if ( iCur < riQuantity )
                {
                    iPrv = iCur-1;
                }
                else
                {
                    iCur = 0;
                    iPrv = riQuantity - 1;
                }
                fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);
            }
            else  // iPIndex is 0
            {
                // vertices on positive side of line
                iCur = 0;
                while ( iCur < riQuantity && afTest[iCur] > 0.0f )
                    akCV[iCQuantity++] = akV[iCur++];

                // last clip vertex on line
                iPrv = iCur-1;
                fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);

                // skip vertices on negative side
                while ( iCur < riQuantity && afTest[iCur] <= 0.0f )
                    iCur++;

                // first clip vertex on line
                if ( iCur < riQuantity )
                {
                    iPrv = iCur-1;
                    fT = afTest[iCur]/(afTest[iCur] - afTest[iPrv]);
                    akCV[iCQuantity++] = akV[iCur]+fT*(akV[iPrv]-akV[iCur]);

                    // vertices on positive side of line
                    while ( iCur < riQuantity && afTest[iCur] > 0.0f )
                        akCV[iCQuantity++] = akV[iCur++];
                }
                else
                {
                    // iCur = 0
                    iPrv = riQuantity - 1;
                    fT = afTest[0]/(afTest[0] - afTest[iPrv]);
                    akCV[iCQuantity++] = akV[0]+fT*(akV[iPrv]-akV[0]);
                }
            }

            riQuantity = iCQuantity;
            memcpy(akV,akCV,iCQuantity*sizeof(Vector2));
        }
        // else polygon fully on positive side of line, nothing to do
    }
    else
    {
        // polygon does not intersect positive side of line, clip all
        riQuantity = 0;
    }
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Vector2 akV0[3], const Vector2 akV1[3],
    int& riQuantity, Vector2 akVertex[6])
{
    // The potential intersection is initialized to the V1-triangle.  The
    // set of vertices is refined based on clipping against each edge of the
    // V0-triangle.
    riQuantity = 3;
    memcpy(akVertex,akV1,3*sizeof(Vector2));

    for (int iI1 = 2, iI2 = 0; iI2 < 3; iI1 = iI2, iI2++)
    {
        // clip against edge <V0[iI1],V0[iI2]>
        Vector2 kN(akV0[iI1].y-akV0[iI2].y,akV0[iI2].x-akV0[iI1].x);
        Real fC = kN.Dot(akV0[iI1]);
        ClipConvexPolygonAgainstLine(kN,fC,riQuantity,akVertex);
        if ( riQuantity == 0 )
        {
            // triangle completely clipped, no intersection occurs
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// moving triangles
//----------------------------------------------------------------------------
class _Configuration
{
public:
    void ComputeTwo (const Vector2 akV[3], const Vector2& rkD,
        int iI0, int iI1, int iI2);

    void ComputeThree (const Vector2 akV[3], const Vector2& rkD,
        const Vector2& rkP);

    static bool NoIntersect (const _Configuration& rkCfg0,
        const _Configuration& rkCfg1, Real fTMax, Real fSpeed,
        int& riSide,  _Configuration& rkTCfg0, _Configuration& rkTCfg1,
        Real& rfTFirst, Real& rfTLast);

    static void GetIntersection (const _Configuration& rkCfg0,
        const _Configuration& rkCfg1, int iSide, const Vector2 akV0[3],
        const Vector2 akV1[3], int& riQuantity, Vector2 akVertex[6]);

private:
    enum ProjectionMap
    {
        M21,  // 2 vertices map to min, 1 vertex maps to max
        M12,  // 1 vertex maps to min, 2 vertices map to max
        M11   // 1 vertex maps to min, 1 vertex maps to max
    };

    ProjectionMap m_eMap;    // how vertices map to the projection interval
    int m_aiIndex[3];        // the sorted indices of the vertices
    Real m_fMin, m_fMax;  // the interval is [min,max]
};
//----------------------------------------------------------------------------
void _Configuration::ComputeTwo (const Vector2 akV[3], const Vector2& rkD,
    int iI0, int iI1, int iI2)
{
    m_eMap = M12;
    m_aiIndex[0] = iI0;
    m_aiIndex[1] = iI1;
    m_aiIndex[2] = iI2;
    m_fMin = rkD.Dot(akV[iI0] - akV[iI1]);
    m_fMax = 0.0f;
}
//----------------------------------------------------------------------------
void _Configuration::ComputeThree (const Vector2 akV[3], const Vector2& rkD,
    const Vector2& rkP)
{
    Real fD0 = rkD.Dot(akV[0] - rkP);
    Real fD1 = rkD.Dot(akV[1] - rkP);
    Real fD2 = rkD.Dot(akV[2] - rkP);

    // Make sure that m_aiIndex[...] is an even permutation of (0,1,2)
    // whenever the map value is M12 or M21.  This is needed to guarantee
    // the intersection of overlapping edges is properly computed.

    if ( fD0 <= fD1 )
    {
        if ( fD1 <= fD2 )  // d0 <= d1 <= d2
        {
            if ( fD0 != fD1 )
                m_eMap = ( fD1 != fD2 ? M11 : M12 );
            else
                m_eMap = M21;

            m_aiIndex[0] = 0;
            m_aiIndex[1] = 1;
            m_aiIndex[2] = 2;
            m_fMin = fD0;
            m_fMax = fD2;
        }
        else if ( fD0 <= fD2 )  // d0 <= d2 < d1
        {
            if ( fD0 != fD2 )
            {
                m_eMap = M11;
                m_aiIndex[0] = 0;
                m_aiIndex[1] = 2;
                m_aiIndex[2] = 1;
            }
            else
            {
                m_eMap = M21;
                m_aiIndex[0] = 2;
                m_aiIndex[1] = 0;
                m_aiIndex[2] = 1;
            }

            m_fMin = fD0;
            m_fMax = fD1;
        }
        else  // d2 < d0 <= d1
        {
            m_eMap = ( fD0 != fD1 ? M12 : M11 );
            m_aiIndex[0] = 2;
            m_aiIndex[1] = 0;
            m_aiIndex[2] = 1;
            m_fMin = fD2;
            m_fMax = fD1;
        }
    }
    else
    {
        if ( fD2 <= fD1 )  // d2 <= d1 < d0
        {
            if ( fD2 != fD1 )
            {
                m_eMap = M11;
                m_aiIndex[0] = 2;
                m_aiIndex[1] = 1;
                m_aiIndex[2] = 0;
            }
            else
            {
                m_eMap = M21;
                m_aiIndex[0] = 1;
                m_aiIndex[1] = 2;
                m_aiIndex[2] = 0;
            }

            m_fMin = fD2;
            m_fMax = fD0;
        }
        else if ( fD2 <= fD0 )  // d1 < d2 <= d0
        {
            m_eMap = ( fD2 != fD0 ? M11 : M12 );
            m_aiIndex[0] = 1;
            m_aiIndex[1] = 2;
            m_aiIndex[2] = 0;
            m_fMin = fD1;
            m_fMax = fD0;
        }
        else  // d1 < d0 < d2
        {
            m_eMap = M11;
            m_aiIndex[0] = 1;
            m_aiIndex[1] = 0;
            m_aiIndex[2] = 2;
            m_fMin = fD1;
            m_fMax = fD2;
        }
    }
}
//----------------------------------------------------------------------------
bool _Configuration::NoIntersect (const _Configuration& rkCfg0,
    const _Configuration& rkCfg1, Real fTMax, Real fSpeed, int& riSide,
    _Configuration& rkTCfg0, _Configuration& rkTCfg1, Real& rfTFirst,
    Real& rfTLast)
{
    Real fInvSpeed, fT;

    if ( rkCfg1.m_fMax < rkCfg0.m_fMin )
    {
        // V1-interval initially on left of V0-interval
        if ( fSpeed <= 0.0f )
            return true;  // intervals moving apart

        // update first time
        fInvSpeed = 1.0f/fSpeed;
        fT = (rkCfg0.m_fMin - rkCfg1.m_fMax)*fInvSpeed;
        if ( fT > rfTFirst )
        {
            rfTFirst = fT;
            riSide = -1;
            rkTCfg0 = rkCfg0;
            rkTCfg1 = rkCfg1;
        }

        // test for exceedance of time interval
        if ( rfTFirst > fTMax )
            return true;

        // update last time
        fT = (rkCfg0.m_fMax - rkCfg1.m_fMin)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // test for separation
        if ( rfTFirst > rfTLast )
            return true;
    }
    else if ( rkCfg0.m_fMax < rkCfg1.m_fMin )
    {
        // V1-interval initially on right of V0-interval
        if ( fSpeed >= 0.0f )
            return true;  // intervals moving apart

        // update first time
        fInvSpeed = 1.0f/fSpeed;
        fT = (rkCfg0.m_fMax - rkCfg1.m_fMin)*fInvSpeed;
        if ( fT > rfTFirst )
        {
            rfTFirst = fT;
            riSide = 1;
            rkTCfg0 = rkCfg0;
            rkTCfg1 = rkCfg1;
        }

        // test for exceedance of time interval
        if ( rfTFirst > fTMax )
            return true;

        // update last time
        fT = (rkCfg0.m_fMin - rkCfg1.m_fMax)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;

        // test for separation
        if ( rfTFirst > rfTLast )
            return true;
    }
    else
    {
        // V0-interval and V1-interval initially overlap
        if ( fSpeed > 0.0f )
        {
            // update last time
            fInvSpeed = 1.0f/fSpeed;
            fT = (rkCfg0.m_fMax - rkCfg1.m_fMin)*fInvSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // test for separation
            if ( rfTFirst > rfTLast )
                return true;
        }
        else if ( fSpeed < 0.0f )
        {
            // update last time
            fInvSpeed = 1.0f/fSpeed;
            fT = (rkCfg0.m_fMin - rkCfg1.m_fMax)*fInvSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;

            // test for separation
            if ( rfTFirst > rfTLast )
                return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void _Configuration::GetIntersection (const _Configuration& rkCfg0,
    const _Configuration& rkCfg1, int iSide, const Vector2 akV0[3],
    const Vector2 akV1[3], int& riQuantity, Vector2 akVertex[6])
{
    Vector2 kEdge, kDiff;
    const Vector2* pkOrigin;
    Real fInvEdE, fMin, fMax, afInterval[2];
    int i;

    if ( iSide == 1 )  // V1-interval contacts V0-interval on right
    {
        if ( rkCfg0.m_eMap == M21 || rkCfg0.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV0[rkCfg0.m_aiIndex[2]];
        }
        else if ( rkCfg1.m_eMap == M12 || rkCfg1.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV1[rkCfg1.m_aiIndex[0]];
        }
        else  // rkCfg0.m_eMap == M12 && rkCfg1.m_eMap == M21 (edge overlap)
        {
            pkOrigin = &akV0[rkCfg0.m_aiIndex[1]];
            kEdge = akV0[rkCfg0.m_aiIndex[2]] - *pkOrigin;
            fInvEdE = 1.0f/kEdge.Dot(kEdge);
            kDiff = akV1[rkCfg1.m_aiIndex[1]] - *pkOrigin;
            fMin = kEdge.Dot(kDiff)*fInvEdE;
            kDiff = akV1[rkCfg1.m_aiIndex[0]] - *pkOrigin;
            fMax = kEdge.Dot(kDiff)*fInvEdE;
            assert( fMin <= fMax );
            FindIntersection(0.0f,1.0f,fMin,fMax,riQuantity,afInterval[0],
                afInterval[1]);
            assert( riQuantity > 0 );
            for (i = 0; i < riQuantity; i++)
                akVertex[i] = *pkOrigin + afInterval[i]*kEdge;
        }
    }
    else if ( iSide == -1 )  // V1-interval contacts V0-interval on left
    {
        if ( rkCfg1.m_eMap == M21 || rkCfg1.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV1[rkCfg1.m_aiIndex[2]];
        }
        else if ( rkCfg0.m_eMap == M12 || rkCfg0.m_eMap == M11 )
        {
            riQuantity = 1;
            akVertex[0] = akV0[rkCfg0.m_aiIndex[0]];
        }
        else  // rkCfg1.m_eMap == M12 && rkCfg0.m_eMap == M21 (edge overlap)
        {
            pkOrigin = &akV1[rkCfg1.m_aiIndex[1]];
            kEdge = akV1[rkCfg1.m_aiIndex[2]] - *pkOrigin;
            fInvEdE = 1.0f/kEdge.Dot(kEdge);
            kDiff = akV0[rkCfg0.m_aiIndex[1]] - *pkOrigin;
            fMin = kEdge.Dot(kDiff)*fInvEdE;
            kDiff = akV0[rkCfg0.m_aiIndex[0]] - *pkOrigin;
            fMax = kEdge.Dot(kDiff)*fInvEdE;
            assert( fMin <= fMax );
            FindIntersection(0.0f,1.0f,fMin,fMax,riQuantity,afInterval[0],
                afInterval[1]);
            assert( riQuantity > 0 );
            for (i = 0; i < riQuantity; i++)
                akVertex[i] = *pkOrigin + afInterval[i]*kEdge;
        }
    }
    else  // triangles were initially intersecting
    {
        FindIntersection(akV0,akV1,riQuantity,akVertex);
    }
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (Real fTMax, const Vector2 akV0[3],
    const Vector2& rkW0, const Vector2 akV1[3], const Vector2& rkW1,
    Real& rfTFirst, Real& rfTLast)
{
    // process as if V0-triangle is stationary and V1-triangle is moving
    Vector2 kW = rkW1 - rkW0;
    int iSide = 0;  // 0 = NONE, -1 = LEFT, +1 = RIGHT
    rfTFirst = 0.0f;
    rfTLast = Math::MAX_REAL;

    _Configuration kCfg0, kCfg1, kTCfg0, kTCfg1;
    int iI0, iI1, iI2;
    Vector2 kD;
    Real fSpeed;

    // process edges of V0-triangle
    for (iI0 = 1, iI1 = 2, iI2 = 0; iI2 < 3; iI0 = iI1, iI1 = iI2, iI2++)
    {
        // test axis V0[i1] + t*perp(V0[i2]-V0[i1]), perp(x,y) = (y,-x)
        kD.x = akV0[iI2].y - akV0[iI1].y;
        kD.y = akV0[iI1].x - akV0[iI2].x;
        fSpeed = kD.Dot(kW);

        kCfg0.ComputeTwo(akV0,kD,iI0,iI1,iI2);
        kCfg1.ComputeThree(akV1,kD,akV0[iI1]);

        if ( _Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,rfTFirst,rfTLast) )
        {
            return false;
        }
    }

    // process edges of V1-triangle
    for (iI0 = 1, iI1 = 2, iI2 = 0; iI2 < 3; iI0 = iI1, iI1 = iI2, iI2++)
    {
        // test axis V1[i1] + t*perp(V1[i2]-V1[i1]), perp(x,y) = (y,-x)
        kD.x = akV1[iI2].y - akV1[iI1].y;
        kD.y = akV1[iI1].x - akV1[iI2].x;
        fSpeed = kD.Dot(kW);

        kCfg1.ComputeTwo(akV1,kD,iI0,iI1,iI2);
        kCfg0.ComputeThree(akV0,kD,akV1[iI1]);

        if ( _Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,rfTFirst,rfTLast) )
        {
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (Real fTMax, const Vector2 akV0[3],
    const Vector2& rkW0, const Vector2 akV1[3], const Vector2& rkW1,
    Real& rfTFirst, Real& rfTLast, int& riQuantity, Vector2 akVertex[6])
{
    // process as if V0-triangle is stationary and V1-triangle is moving
    Vector2 kW = rkW1 - rkW0;
    int iSide = 0;  // 0 = NONE, -1 = LEFT, +1 = RIGHT
    rfTFirst = 0.0f;
    rfTLast = Math::MAX_REAL;

    _Configuration kCfg0, kCfg1, kTCfg0, kTCfg1;
    int iI0, iI1, iI2;
    Vector2 kD;
    Real fSpeed;

    // process edges of V0-triangle
    for (iI0 = 1, iI1 = 2, iI2 = 0; iI2 < 3; iI0 = iI1, iI1 = iI2, iI2++)
    {
        // test axis V0[i1] + t*perp(V0[i2]-V0[i1]), perp(x,y) = (y,-x)
        kD.x = akV0[iI2].y - akV0[iI1].y;
        kD.y = akV0[iI1].x - akV0[iI2].x;
        fSpeed = kD.Dot(kW);

        kCfg0.ComputeTwo(akV0,kD,iI0,iI1,iI2);
        kCfg1.ComputeThree(akV1,kD,akV0[iI1]);

        if ( _Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,rfTFirst,rfTLast) )
        {
            return false;
        }
    }

    // process edges of V1-triangle
    for (iI0 = 1, iI1 = 2, iI2 = 0; iI2 < 3; iI0 = iI1, iI1 = iI2, iI2++)
    {
        // test axis V1[i1] + t*perp(V1[i2]-V1[i1]), perp(x,y) = (y,-x)
        kD.x = akV1[iI2].y - akV1[iI1].y;
        kD.y = akV1[iI1].x - akV1[iI2].x;
        fSpeed = kD.Dot(kW);

        kCfg1.ComputeTwo(akV1,kD,iI0,iI1,iI2);
        kCfg0.ComputeThree(akV0,kD,akV1[iI1]);

        if ( _Configuration::NoIntersect(kCfg0,kCfg1,fTMax,fSpeed,iSide,
             kTCfg0,kTCfg1,rfTFirst,rfTLast) )
        {
            return false;
        }
    }

    // move triangles to first contact
    Vector2 akMoveV0[3], akMoveV1[3];
    for (int i = 0; i < 3; i++)
    {
        akMoveV0[i] = akV0[i] + rfTFirst*rkW0;
        akMoveV1[i] = akV1[i] + rfTFirst*rkW1;
    };

    _Configuration::GetIntersection(kTCfg0,kTCfg1,iSide,akMoveV0,akMoveV1,
        riQuantity,akVertex);

    return riQuantity > 0;
}
//----------------------------------------------------------------------------


