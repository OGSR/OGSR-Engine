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

#include "MgcQuadToQuadTransforms.h"
using namespace Mgc;

static const Real gs_fEpsilon = 1e-06f;

//----------------------------------------------------------------------------
HmQuadToSqr::HmQuadToSqr (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    // translate to origin
    m_kT = rkP00;
    Vector2 kQ10 = rkP10 - rkP00;
    Vector2 kQ11 = rkP11 - rkP00;
    Vector2 kQ01 = rkP01 - rkP00;

    Matrix2 kInvM(kQ10.x,kQ01.x,kQ10.y,kQ01.y);
    bool bInverted = kInvM.Inverse(m_kM,gs_fEpsilon);
    assert( bInverted );  // else quad degenerate

    // compute where p11 is mapped to
    Vector2 kCorner = m_kM*kQ11;  // = (a,b)

    // Compute homogeneous transform of quadrilateral
    // {(0,0),(1,0),(a,b),(0,1)} to square {(0,0),(1,0),(1,1),(0,1)}
    m_kG.x = (kCorner.y - 1.0f)/kCorner.x;
    m_kG.y = (kCorner.x - 1.0f)/kCorner.y;
    m_kD.x = 1.0f + m_kG.x;
    m_kD.y = 1.0f + m_kG.y;
}
//----------------------------------------------------------------------------
Vector2 HmQuadToSqr::Transform (const Vector2& rkP)
{
    Vector2 kProd = m_kM*(rkP - m_kT);
    Real fInvDenom = 1.0f/(1.0f + m_kG.Dot(kProd));
    Vector2 kResult = fInvDenom*kProd;
    kResult.x *= m_kD.x;
    kResult.y *= m_kD.y;
    return kResult;
}
//----------------------------------------------------------------------------
HmSqrToQuad::HmSqrToQuad (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    // translate to origin
    m_kT = rkP00;
    m_kM[0][0] = rkP10.x - rkP00.x;
    m_kM[0][1] = rkP01.x - rkP00.x;
    m_kM[1][0] = rkP10.y - rkP00.y;
    m_kM[1][1] = rkP01.y - rkP00.y;

    Matrix2 kInvM;
    bool bInverted = m_kM.Inverse(kInvM,gs_fEpsilon);
    assert( bInverted );  // else quad degenerate

    // find point which is mapped to p11
    Vector2 kCorner = kInvM*(rkP11-rkP00);  // = (a,b)

    // compute homogeneous transform of square {(0,0),(1,0),(1,1),(0,1)} to
    // quadrilateral {(0,0),(1,0),(a,b),(0,1)}
    Real fInvDenom = 1.0f/(kCorner.x + kCorner.y - 1.0f);
    m_kG.x = fInvDenom*(1.0f - kCorner.y);
    m_kG.y = fInvDenom*(1.0f - kCorner.x);
    m_kD.x = fInvDenom*kCorner.x;
    m_kD.y = fInvDenom*kCorner.y;
}
//----------------------------------------------------------------------------
Vector2 HmSqrToQuad::Transform (const Vector2& rkP)
{
    Real fInvDenom = 1.0f/(1.0f + m_kG.Dot(rkP));
    Vector2 kResult(m_kD.x*rkP.x,m_kD.y*rkP.y);
    Vector2 kProd = m_kM*kResult;
    kResult.x = fInvDenom*kProd.x + m_kT.x;
    kResult.y = fInvDenom*kProd.y + m_kT.y;
    return kResult;
}
//----------------------------------------------------------------------------
BiQuadToSqr::BiQuadToSqr (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    m_kA = rkP00;
    m_kB = rkP10 - rkP00;
    m_kC = rkP01 - rkP00;
    m_kD = rkP11 + rkP00 - rkP10 - rkP01;
    m_fBCdet = m_kB.x*m_kC.y - m_kB.y*m_kC.x;
    assert( Math::FAbs(m_fBCdet) >= gs_fEpsilon );  // else quad degenerate
    m_fCDdet = m_kC.y*m_kD.x - m_kC.x*m_kD.y;
}
//----------------------------------------------------------------------------
Vector2 BiQuadToSqr::Transform (const Vector2& rkP)
{
    Vector2 kDiff = m_kA - rkP;
    Real fABdet = kDiff.y*m_kB.x-kDiff.x*m_kB.y;
    Real fADdet = kDiff.y*m_kD.x-kDiff.x*m_kD.y;

    Real fA = m_fCDdet;
    Real fB = fADdet + m_fBCdet;
    Real fC = fABdet;

    Vector2 kResult;

    if ( Math::FAbs(fA) >= gs_fEpsilon )
    {
        // t-equation is quadratic
        Real fDiscr = Math::Sqrt(Math::FAbs(fB*fB-4.0f*fA*fC));
        kResult.y = (-fB + fDiscr)/(2.0f*fA);
        if ( kResult.y < 0.0f || kResult.y > 1.0f )
        {
            kResult.y = (-fB - fDiscr)/(2.0f*fA);
            if ( kResult.y < 0.0f || kResult.y > 1.0f )
            {
                // point p not inside quadrilateral, return invalid result
                return Vector2(-1.0f,-1.0f);
            }
        }
    }
    else
    {
        // t-equation is linear
        kResult.y = -fC/fB;
    }

    kResult.x = -(kDiff.x + kResult.y*m_kC.x)/(m_kB.x + kResult.y*m_kD.x);

    return kResult;
}
//----------------------------------------------------------------------------
BiSqrToQuad::BiSqrToQuad (const Vector2& rkP00, const Vector2& rkP10,
    const Vector2& rkP11, const Vector2& rkP01)
{
    m_kS00 = rkP00;
    m_kS10 = rkP10;
    m_kS11 = rkP11;
    m_kS01 = rkP01;
}
//----------------------------------------------------------------------------
Vector2 BiSqrToQuad::Transform (const Vector2& rkP)
{
    Vector2 kOmP(1.0f-rkP.x,1.0f-rkP.y);
    Vector2 kResult;
    kResult.x = kOmP.y*(kOmP.x*m_kS00.x + rkP.x*m_kS10.x) +
        rkP.y*(kOmP.x*m_kS01.x + rkP.x*m_kS11.x);
    kResult.y = kOmP.y*(kOmP.x*m_kS00.y + rkP.x*m_kS10.y) +
        rkP.y*(kOmP.x*m_kS01.y + rkP.x*m_kS11.y);
    return kResult;
}
//----------------------------------------------------------------------------

