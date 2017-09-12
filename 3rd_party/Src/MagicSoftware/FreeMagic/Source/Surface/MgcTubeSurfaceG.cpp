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

#include "MgcMultipleCurve3.h"
#include "MgcTubeSurfaceG.h"
using namespace Mgc;

//----------------------------------------------------------------------------
TubeSurfaceG::TubeSurfaceG (MultipleCurve3* pkMedial, RadialFunction oRadial,
    bool bClosed, int iMedialSamples, int iSliceSamples,
    bool bSampleByArcLength, bool bWantNormals, const Vector3* pkUpVector)
{
    assert( pkMedial && oRadial );

    m_pkMedial = pkMedial;
    m_oRadial = oRadial;
    m_bClosed = bClosed;
    m_iVertexQuantity = 0;
    m_akVertex = NULL;
    m_akNormal = NULL;
    m_iTriangleQuantity = 0;
    m_aiConnect = NULL;

    // default tessellation
    Tessellate(iMedialSamples,iSliceSamples,bSampleByArcLength,bWantNormals,
        pkUpVector);
}
//----------------------------------------------------------------------------
TubeSurfaceG::TubeSurfaceG ()
{
    m_pkMedial = NULL;
    m_oRadial = NULL;
    m_bClosed = false;
    m_iVertexQuantity = 0;
    m_akVertex = NULL;
    m_akNormal = NULL;
    m_iTriangleQuantity = 0;
    m_aiConnect = NULL;
}
//----------------------------------------------------------------------------
TubeSurfaceG::~TubeSurfaceG ()
{
    delete m_pkMedial;
    delete[] m_akVertex;
    delete[] m_akNormal;
    delete[] m_aiConnect;
}
//----------------------------------------------------------------------------
void TubeSurfaceG::ComputeVertices ()
{
    if ( m_bClosed )
        m_iVertexQuantity = (m_iSliceSamples+1)*(m_iMedialSamples+1);
    else
        m_iVertexQuantity = (m_iSliceSamples+1)*m_iMedialSamples;

    delete[] m_akVertex;
    m_akVertex = new Vector3[m_iVertexQuantity];

    // Compute slice vertex coefficients.  The first and last coefficients
    // are duplicated to allow a closed cross section that has two different
    // pairs of texture coordinates at the shared vertex.
    Real* afSin = new Real[m_iSliceSamples + 1];
    Real* afCos = new Real[m_iSliceSamples + 1];
    int iS;
    for (iS = 0; iS < m_iSliceSamples; iS++)
    {
        Real fAngle = iS*Math::TWO_PI/m_iSliceSamples;
        afCos[iS] = Math::Cos(fAngle);
        afSin[iS] = Math::Sin(fAngle);
    }
    afSin[m_iSliceSamples] = afSin[0];
    afCos[m_iSliceSamples] = afCos[0];

    // compute vertices
    Real fTMin = m_pkMedial->GetMinTime();
    Real fTRange = m_pkMedial->GetMaxTime() - fTMin;
    Real fTotalLength;
    if ( m_bClosed )
        fTotalLength = m_pkMedial->GetTotalLength();
    else
        fTotalLength = 0.0f;

    for (int iM = 0, iV = 0; iM < m_iMedialSamples; iM++)
    {
        Real fT;
        if ( m_bClosed )
        {
            if ( m_bSampleByArcLength )
            {
                fT = m_pkMedial->GetTime(
                    iM*fTotalLength/Real(m_iMedialSamples));
            }
            else
            {
                fT = fTMin + iM*fTRange/Real(m_iMedialSamples);
            }
        }
        else
        {
            if ( m_bSampleByArcLength )
            {
                fT = m_pkMedial->GetTime(
                    iM*fTotalLength/Real(m_iMedialSamples-1));
            }
            else
            {
                fT = fTMin + iM*fTRange/Real(m_iMedialSamples-1);
            }
        }

        Real fRadius = m_oRadial(fT);

        // compute frame (position P, tangent T, normal N, binormal B)
        Vector3 kP, kT, kN, kB;
        if ( m_pkUpVector )
        {
            // Always use 'up' vector N rather than curve normal.  You must
            // constrain the curve so that T and N are never parallel.  To
            // build the frame from this, let
            //     B = Cross(T,N)/Length(Cross(T,N))
            // and replace
            //     N = Cross(B,T)/Length(Cross(B,T)).
            kP = m_pkMedial->GetPosition(fT);
            kT = m_pkMedial->GetTangent(fT);
            kB = kT.UnitCross(Vector3::UNIT_Z);
            kN = kB.UnitCross(kT);
        }
        else
        {
            // use Frenet frame to create slices
            m_pkMedial->GetFrame(fT,kP,kT,kN,kB);
        }

        // compute slice vertices, duplication at end point as noted earlier
        int iSave = iV;
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            m_akVertex[iV] = kP + fRadius*(afCos[iS]*kN + afSin[iS]*kB);
            iV++;
        }
        m_akVertex[iV] = m_akVertex[iSave];
        iV++;
    }

    if ( m_bClosed )
    {
        for (iS = 0; iS <= m_iSliceSamples; iS++)
        {
            int i1 = Index(iS,m_iMedialSamples);
            int i0 = Index(iS,0);
            m_akVertex[i1] = m_akVertex[i0];
        }
    }

    delete[] afSin;
    delete[] afCos;
}
//----------------------------------------------------------------------------
void TubeSurfaceG::ComputeNormals ()
{
    delete[] m_akNormal;
    m_akNormal = new Vector3[m_iVertexQuantity];

    int iS, iSm, iSp, iMm, iMp;
    Vector3 kDir0, kDir1;

    // interior normals (central differences)
    for (int iM = 1; iM <= m_iMedialSamples-2; iM++)
    {
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = ( iS > 0 ? iS-1 : m_iSliceSamples-1 );
            iSp = iS + 1;
            iMm = iM - 1;
            iMp = iM + 1;
            kDir0 = m_akVertex[Index(iSm,iM)] - m_akVertex[Index(iSp,iM)];
            kDir1 = m_akVertex[Index(iS,iMm)] - m_akVertex[Index(iS,iMp)];
            m_akNormal[Index(iS,iM)] = kDir0.UnitCross(kDir1);
        }
        m_akNormal[Index(m_iSliceSamples,iM)] = m_akNormal[Index(0,iM)];
    }

    // boundary normals
    if ( m_bClosed )
    {
        // central differences
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = ( iS > 0 ? iS-1 : m_iSliceSamples-1 );
            iSp = iS + 1;

            // m = 0
            kDir0 = m_akVertex[Index(iSm,0)] - m_akVertex[Index(iSp,0)];
            kDir1 = m_akVertex[Index(iS,m_iMedialSamples-1)] -
                m_akVertex[Index(iS,1)];
            m_akNormal[iS] = kDir0.UnitCross(kDir1);

            // m = max
            m_akNormal[Index(iS,m_iMedialSamples)] = m_akNormal[Index(iS,0)];
        }
        m_akNormal[Index(m_iSliceSamples,0)] = m_akNormal[Index(0,0)];
        m_akNormal[Index(m_iSliceSamples,m_iMedialSamples)] =
            m_akNormal[Index(0,m_iMedialSamples)];
    }
    else
    {
        // one-sided finite differences

        // m = 0
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = ( iS > 0 ? iS-1 : m_iSliceSamples-1 );
            iSp = iS + 1;
            kDir0 = m_akVertex[Index(iSm,0)] - m_akVertex[Index(iSp,0)];
            kDir1 = m_akVertex[Index(iS,0)] - m_akVertex[Index(iS,1)];
            m_akNormal[Index(iS,0)] = kDir0.UnitCross(kDir1);
        }
        m_akNormal[Index(m_iSliceSamples,0)] = m_akNormal[Index(0,0)];

        // m = max-1
        for (iS = 0; iS < m_iSliceSamples; iS++)
        {
            iSm = ( iS > 0 ? iS-1 : m_iSliceSamples-1 );
            iSp = iS + 1;
            kDir0 = m_akVertex[Index(iSm,m_iMedialSamples-1)] -
                m_akVertex[Index(iSp,m_iMedialSamples-1)];
            kDir1 = m_akVertex[Index(iS,m_iMedialSamples-2)] -
                m_akVertex[Index(iS,m_iMedialSamples-1)];
            m_akNormal[iS] = kDir0.UnitCross(kDir1);
        }
        m_akNormal[Index(m_iSliceSamples,m_iMedialSamples-1)] =
            m_akNormal[Index(0,m_iMedialSamples-1)];
    }
}
//----------------------------------------------------------------------------
void TubeSurfaceG::ComputeConnectivity ()
{
    m_iTriangleQuantity = 2*m_iSliceSamples*m_iMedialSamples;
    delete[] m_aiConnect;
    m_aiConnect = new int[3*m_iTriangleQuantity];

    int* piConnect = m_aiConnect;
    for (int iM = 0, iMStart = 0; iM < m_iMedialSamples; iM++)
    {
        int iI0 = iMStart;
        int iI1 = iI0 + 1;
        iMStart += m_iSliceSamples + 1;
        int iI2 = iMStart;
        int iI3 = iI2 + 1;
        for (int iS = 0; iS < m_iSliceSamples; iS++)
        {
            // TO DO.  The ordering of the vertices assumes you want to view
            // the tube from the inside.  If you want a view from the outside
            // or if you want a double-sided tube, this function must change.
            *piConnect++ = iI0;
            *piConnect++ = iI2;
            *piConnect++ = iI1;
            *piConnect++ = iI1;
            *piConnect++ = iI2;
            *piConnect++ = iI3;
            iI0++;
            iI1++;
            iI2++;
            iI3++;
        }
    }
}
//----------------------------------------------------------------------------
void TubeSurfaceG::Tessellate (int iMedialSamples, int iSliceSamples,
    bool bSampleByArcLength, bool bWantNormals, const Vector3* pkUpVector)
{
    m_iMedialSamples = iMedialSamples;
    m_iSliceSamples = iSliceSamples;
    m_bSampleByArcLength = bSampleByArcLength;
    m_bWantNormals = bWantNormals;
    m_pkUpVector = pkUpVector;

    ComputeVertices();

    if ( m_bWantNormals )
        ComputeNormals();

    ComputeConnectivity();
}
//----------------------------------------------------------------------------

