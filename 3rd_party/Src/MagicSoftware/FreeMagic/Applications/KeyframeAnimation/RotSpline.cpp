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

#include "RotSpline.h"
using namespace Mgc;

//----------------------------------------------------------------------------
RotSpline::RotSpline (int iNumKeys, RotKey* akKey)
{
    assert( iNumKeys >= 4 );

    m_iNumPolys = iNumKeys-3;
    m_akPoly = new SquadPoly[m_iNumPolys];

    // Consecutive quaterions should form an acute angle.  Changing sign on
    // a quaternion does not change the rotation it represents.
    int i;
    for (i = 1; i < iNumKeys; i++)
    {
        if ( akKey[i].Q().Dot(akKey[i-1].Q()) < 0.0f )
            akKey[i].Q() = -akKey[i].Q();
    }

    for (int i0=0,i1=1,i2=2,i3=3; i0<m_iNumPolys; i0++,i1++,i2++,i3++)
    {
        Quaternion kQ0 = akKey[i0].Q();
        Quaternion kQ1 = akKey[i1].Q();
        Quaternion kQ2 = akKey[i2].Q();
        Quaternion kQ3 = akKey[i3].Q();

        Quaternion kLog10 = (kQ0.UnitInverse()*kQ1).Log();
        Quaternion kLog21 = (kQ1.UnitInverse()*kQ2).Log();
        Quaternion kLog32 = (kQ2.UnitInverse()*kQ3).Log();

        // build multipliers at q[i1]
        float fOmT0 = 1.0f - akKey[i1].Tension();
        float fOmC0 = 1.0f - akKey[i1].Continuity();
        float fOpC0 = 1.0f + akKey[i1].Continuity();
        float fOmB0 = 1.0f - akKey[i1].Bias();
        float fOpB0 = 1.0f + akKey[i1].Bias();
        float fAdj0 = 2.0f*(akKey[i2].Time() - akKey[i1].Time()) /
            (akKey[i2].Time() - akKey[i0].Time());
        float fOut0 = 0.5f*fAdj0*fOmT0*fOpC0*fOpB0;
        float fOut1 = 0.5f*fAdj0*fOmT0*fOmC0*fOmB0;

        // build outgoing tangent at q[i1]
        Quaternion kTOut = fOut1*kLog21 + fOut0*kLog10;

        // build multipliers at q[i2]
        float fOmT1 = 1.0f - akKey[i2].Tension();
        float fOmC1 = 1.0f - akKey[i2].Continuity();
        float fOpC1 = 1.0f + akKey[i2].Continuity();
        float fOmB1 = 1.0f - akKey[i2].Bias();
        float fOpB1 = 1.0f + akKey[i2].Bias();
        float fAdj1 = 2.0f*(akKey[i2].Time() - akKey[i1].Time()) /
            (akKey[i3].Time() - akKey[i1].Time());
        float fIn0 = 0.5f*fAdj1*fOmT1*fOmC1*fOpB1;
        float fIn1 = 0.5f*fAdj1*fOmT1*fOpC1*fOmB1;

        // build incoming tangent at q[i2]
        Quaternion kTIn = fIn1*kLog32 + fIn0*kLog21;

        m_akPoly[i0].m_kP = kQ1;
        m_akPoly[i0].m_kQ = kQ2;
        m_akPoly[i0].m_kA = kQ1*((0.5f*(kTOut-kLog21)).Exp());
        m_akPoly[i0].m_kB = kQ2*((0.5f*(kLog21-kTIn)).Exp());
        m_akPoly[i0].m_fTMin = akKey[i1].Time();
        m_akPoly[i0].m_fTMax = akKey[i2].Time();
        m_akPoly[i0].m_fTInvRange = 1.0f/(akKey[i2].Time()-akKey[i1].Time());
    }
}
//----------------------------------------------------------------------------
RotSpline::~RotSpline ()
{
    delete[] m_akPoly;
}
//----------------------------------------------------------------------------
Quaternion RotSpline::Q (float fTime)
{
    // find the interpolating polynomial (clamping used, modify for looping)
    int i;
    float fU;

    if ( m_akPoly[0].m_fTMin < fTime )
    {
        if ( fTime < m_akPoly[m_iNumPolys-1].m_fTMax )
        {
            for (i = 0; i < m_iNumPolys; i++)
            {
                if ( fTime < m_akPoly[i].m_fTMax )
                    break;
            }
            fU = (fTime-m_akPoly[i].m_fTMin)*m_akPoly[i].m_fTInvRange;
        }
        else
        {
            i = m_iNumPolys-1;
            fU = 1.0f;
        }
    }
    else
    {
        i = 0;
        fU = 0.0f;
    }

    return m_akPoly[i].Q(fU);
}
//----------------------------------------------------------------------------
Quaternion RotSpline::SquadPoly::Q (float fU)
{
    Quaternion kSquad = Quaternion::Squad(fU,m_kP,m_kA,m_kB,m_kQ);
    return kSquad;
}
//----------------------------------------------------------------------------


