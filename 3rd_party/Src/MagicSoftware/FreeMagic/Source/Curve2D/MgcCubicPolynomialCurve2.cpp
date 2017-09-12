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

#include "MgcCubicPolynomialCurve2.h"
using namespace Mgc;

//----------------------------------------------------------------------------
CubicPolynomialCurve2::CubicPolynomialCurve2 (Polynomial* pkXPoly,
    Polynomial* pkYPoly)
    :
    PolynomialCurve2(pkXPoly,pkYPoly)
{
    assert( pkXPoly && pkYPoly );
    assert( pkXPoly->GetDegree() == 3 && pkYPoly->GetDegree() == 3 );

    m_iVertexQuantity = 0;
    m_akVertex = NULL;
}
//----------------------------------------------------------------------------
CubicPolynomialCurve2::~CubicPolynomialCurve2 ()
{
    delete[] m_akVertex;
}
//----------------------------------------------------------------------------
void CubicPolynomialCurve2::Tessellate (int iLevel)
{
    // vertices V = (2^L+1)
    int iTwoPowL = (1 << iLevel);
    m_iVertexQuantity = iTwoPowL + 1;
    delete[] m_akVertex;
    m_akVertex = new Vector2[m_iVertexQuantity];

    // indices of endpoints, I[t]
    IntervalParameters kIP;
    kIP.m_iI0 = 0;
    kIP.m_iI1 = iTwoPowL;

    // vertices for subdivision
    Vector2* akX = m_akVertex;
    akX[kIP.m_iI0] = GetPosition(m_fTMin);
    akX[kIP.m_iI1] = GetPosition(m_fTMax);

    // recursive subdivision
    if ( iLevel > 0 )
    {
        kIP.m_akXuu[0] = GetSecondDerivative(m_fTMin);
        kIP.m_akXuu[1] = GetSecondDerivative(m_fTMax);

        Subdivide(--iLevel,0.25f,akX,kIP);
    }
}
//----------------------------------------------------------------------------
void CubicPolynomialCurve2::Subdivide (int iLevel, Real fDSqr, Vector2* akX,
    IntervalParameters& rkIP)
{
    // subdivision index
    int iIM = (rkIP.m_iI0 + rkIP.m_iI1) >> 1;

    // vertices
    Vector2 kXuuM = 0.5f*(rkIP.m_akXuu[0] + rkIP.m_akXuu[1]);
    akX[iIM] = 0.5f*(akX[rkIP.m_iI0] + akX[rkIP.m_iI1] - fDSqr*kXuuM);

    // recurse on two children
    if ( iLevel > 0 )
    {
        iLevel--;
        fDSqr *= 0.25f;

        IntervalParameters kSubIP;

        // subinterval [t0,tM]
        kSubIP.m_iI0 = rkIP.m_iI0;
        kSubIP.m_iI1 = iIM;
        kSubIP.m_akXuu[0] = rkIP.m_akXuu[0];
        kSubIP.m_akXuu[1] = kXuuM;
        Subdivide(iLevel,fDSqr,akX,kSubIP);

        // subinterval [tM,t1]
        kSubIP.m_iI0 = iIM;
        kSubIP.m_iI1 = rkIP.m_iI1;
        kSubIP.m_akXuu[0] = kXuuM;
        kSubIP.m_akXuu[1] = rkIP.m_akXuu[1];
        Subdivide(iLevel,fDSqr,akX,kSubIP);
    }
}
//----------------------------------------------------------------------------


