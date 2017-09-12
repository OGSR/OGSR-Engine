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

#include "MgcInterp2DQdrNonuniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DQdrNonuniform::Interp2DQdrNonuniform (int iVertexQuantity,
    Vector2* akVertex, Real* afF, Real* afFx, Real* afFy)
    :
    Delaunay2D(iVertexQuantity,akVertex)
{
    assert( afF && afFx && afFy );
    m_afF = afF;
    m_afFx = afFx;
    m_afFy = afFy;
    ProcessTriangles();
}
//----------------------------------------------------------------------------
Interp2DQdrNonuniform::Interp2DQdrNonuniform (int iVertexQuantity,
    Vector2* akVertex, Real* afF)
    :
    Delaunay2D(iVertexQuantity,akVertex)
{
    assert( afF );
    m_afF = afF;
    EstimateDerivatives();
    ProcessTriangles();
}
//----------------------------------------------------------------------------
Interp2DQdrNonuniform::Interp2DQdrNonuniform (Delaunay2D& rkNet, Real* afF,
    Real* afFx, Real* afFy)
    :
    Delaunay2D(rkNet)
{
    assert( afF && afFx && afFy );
    m_afF = afF;
    m_afFx = afFx;
    m_afFy = afFy;
    ProcessTriangles();
}
//----------------------------------------------------------------------------
Interp2DQdrNonuniform::Interp2DQdrNonuniform (Delaunay2D& rkNet, Real* afF)
    :
    Delaunay2D(rkNet)
{
    assert( afF );
    m_afF = afF;
    EstimateDerivatives();
    ProcessTriangles();
}
//----------------------------------------------------------------------------
Interp2DQdrNonuniform::~Interp2DQdrNonuniform ()
{
    delete[] m_afF;
    delete[] m_afFx;
    delete[] m_afFy;
    delete[] m_akTData;
    delete[] m_akECenter;
}
//----------------------------------------------------------------------------
void Interp2DQdrNonuniform::EstimateDerivatives ()
{
    m_afFx = new Real[m_iVertexQuantity];
    m_afFy = new Real[m_iVertexQuantity];
    Real* afFz = new Real[m_iVertexQuantity];
    memset(m_afFx,0,m_iVertexQuantity*sizeof(Real));
    memset(m_afFy,0,m_iVertexQuantity*sizeof(Real));
    memset(afFz,0,m_iVertexQuantity*sizeof(Real));

    // accumulate normals at spatial locations (averaging process)
    int i;
    for (i = 0; i < m_iTriangleQuantity; i++)
    {
        Triangle& rkTri = m_akTriangle[i];

        // get three vertices of triangle
        int j0 = rkTri.m_aiVertex[0];
        int j1 = rkTri.m_aiVertex[1];
        int j2 = rkTri.m_aiVertex[2];

        // compute normal vector of triangle (with positive z-component)
        Real fDx1 = m_akVertex[j1].x - m_akVertex[j0].x;
        Real fDy1 = m_akVertex[j1].y - m_akVertex[j0].y;
        Real fDz1 = m_afF[j1] - m_afF[j0];
        Real fDx2 = m_akVertex[j2].x - m_akVertex[j0].x;
        Real fDy2 = m_akVertex[j2].y - m_akVertex[j0].y;
        Real fDz2 = m_afF[j2] - m_afF[j0];
        Real fNx = fDy1*fDz2 - fDy2*fDz1;
        Real fNy = fDz1*fDx2 - fDz2*fDx1;
        Real fNz = fDx1*fDy2 - fDx2*fDy1;
        if ( fNz < 0.0f )
        {
            fNx = -fNx;
            fNy = -fNy;
            fNz = -fNz;
        }

        m_afFx[j0] += fNx;  m_afFy[j0] += fNy;  afFz[j0] += fNz;
        m_afFx[j1] += fNx;  m_afFy[j1] += fNy;  afFz[j1] += fNz;
        m_afFx[j2] += fNx;  m_afFy[j2] += fNy;  afFz[j2] += fNz;
    }

    // scale the normals to form (x,y,-1)
    for (i = 0; i < m_iVertexQuantity; i++)
    {
        if ( Math::FAbs(afFz[i]) > 1e-06f )
        {
            Real fInv = -1.0f/afFz[i];
            m_afFx[i] *= fInv;
            m_afFy[i] *= fInv;
        }
        else
        {
            m_afFx[i] = 0.0f;
            m_afFy[i] = 0.0f;
        }
    }

    delete[] afFz;
}
//----------------------------------------------------------------------------
void Interp2DQdrNonuniform::ProcessTriangles ()
{
    // Add degenerate triangles to boundary triangles so that interpolation
    // at the boundary can be treated in the same way as interpolation in
    // the interior.

    // add quadratic data to triangle network
    m_akTData = new TriangleData[m_iTriangleQuantity];
    m_akECenter = new Vector2[m_iExtraTriangleQuantity];

    // compute centers of inscribed circles for triangles
    int iT;
    for (iT = 0; iT < m_iTriangleQuantity; iT++)
    {
        Triangle& rkTri = m_akTriangle[iT];
        Delaunay2D::ComputeInscribedCenter(m_akVertex[rkTri.m_aiVertex[0]],
            m_akVertex[rkTri.m_aiVertex[1]],m_akVertex[rkTri.m_aiVertex[2]],
            m_akTData[iT].m_kCenter);
    }

    // compute centers of edges on convex hull
    int iE = 0;
    for (iT = 0; iT < m_iTriangleQuantity; iT++)
    {
        Triangle& rkTri = m_akTriangle[iT];
        for (int j = 0; j < 3; j++)
        {
            if ( rkTri.m_aiAdjacent[j] >= m_iTriangleQuantity )
            {
                m_akECenter[iE] = 0.5f*(
                    m_akVertex[rkTri.m_aiVertex[j]] +
                    m_akVertex[rkTri.m_aiVertex[(j+1) % 3]]);
                iE++;
            }
        }
    }

    // compute cross-edge intersections
    for (iT = 0; iT < m_iTriangleQuantity; iT++)
        ComputeCrossEdgeIntersections(iT);

    // compute Bezier coefficients
    for (iT = 0; iT < m_iTriangleQuantity; iT++)
        ComputeCoefficients(iT);
}
//----------------------------------------------------------------------------
void Interp2DQdrNonuniform::ComputeCrossEdgeIntersections (int iT)
{
    Triangle& rkTri = m_akTriangle[iT];
    const Vector2& rkV0 = m_akVertex[rkTri.m_aiVertex[0]];
    const Vector2& rkV1 = m_akVertex[rkTri.m_aiVertex[1]];
    const Vector2& rkV2 = m_akVertex[rkTri.m_aiVertex[2]];

    Vector2 akU[3];
    for (int i = 0; i < 3; i++)
    {
        int iA = rkTri.m_aiAdjacent[i];
        if ( iA < m_iTriangleQuantity )
            akU[i] = m_akTData[iA].m_kCenter;
        else
            akU[i] = m_akECenter[iA - m_iTriangleQuantity];
    }

    Real fM00, fM01, fM10, fM11, fR0, fR1, fInvDet;

    // intersection on edge <V0,V1>
    fM00 = rkV0.y - rkV1.y;
    fM01 = rkV1.x - rkV0.x;
    fM10 = m_akTData[iT].m_kCenter.y - akU[0].y;
    fM11 = akU[0].x - m_akTData[iT].m_kCenter.x;
    fR0  = fM00*rkV0.x + fM01*rkV0.y;
    fR1  = fM10*m_akTData[iT].m_kCenter.x + fM11*m_akTData[iT].m_kCenter.y;
    fInvDet = 1.0f/(fM00*fM11 - fM01*fM10);
    m_akTData[iT].m_akIntersect[0].x = (fM11*fR0-fM01*fR1)*fInvDet;
    m_akTData[iT].m_akIntersect[0].y = (fM00*fR1-fM10*fR0)*fInvDet;

    // intersection on edge <V1,V2>
    fM00 = rkV1.y - rkV2.y;
    fM01 = rkV2.x - rkV1.x;
    fM10 = m_akTData[iT].m_kCenter.y - akU[1].y;
    fM11 = akU[1].x - m_akTData[iT].m_kCenter.x;
    fR0  = fM00*rkV1.x + fM01*rkV1.y;
    fR1  = fM10*m_akTData[iT].m_kCenter.x + fM11*m_akTData[iT].m_kCenter.y;
    fInvDet = 1.0f/(fM00*fM11 - fM01*fM10);
    m_akTData[iT].m_akIntersect[1].x = (fM11*fR0-fM01*fR1)*fInvDet;
    m_akTData[iT].m_akIntersect[1].y = (fM00*fR1-fM10*fR0)*fInvDet;

    // intersection on edge <V0,V2>
    fM00 = rkV0.y - rkV2.y;
    fM01 = rkV2.x - rkV0.x;
    fM10 = m_akTData[iT].m_kCenter.y - akU[2].y;
    fM11 = akU[2].x - m_akTData[iT].m_kCenter.x;
    fR0  = fM00*rkV0.x + fM01*rkV0.y;
    fR1  = fM10*m_akTData[iT].m_kCenter.x + fM11*m_akTData[iT].m_kCenter.y;
    fInvDet = 1.0f/(fM00*fM11 - fM01*fM10);
    m_akTData[iT].m_akIntersect[2].x = (fM11*fR0-fM01*fR1)*fInvDet;
    m_akTData[iT].m_akIntersect[2].y = (fM00*fR1-fM10*fR0)*fInvDet;
}
//----------------------------------------------------------------------------
void Interp2DQdrNonuniform::ComputeCoefficients (int iT)
{
    Triangle& rkTri = m_akTriangle[iT];
    TriangleData& rkTData = m_akTData[iT];

    // get sample data at main triangle vertices
    Vector2 akV[3];
    Jet afJet[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        int j = rkTri.m_aiVertex[i];
        akV[i] = m_akVertex[j];
        afJet[i].m_fF = m_afF[j];
        afJet[i].m_fFx = m_afFx[j];
        afJet[i].m_fFy = m_afFy[j];
    }

    Vector2 akU[3];
    for (i = 0; i < 3; i++)
    {
        int iA = rkTri.m_aiAdjacent[i];
        if ( iA < m_iTriangleQuantity )
            akU[i] = m_akTData[iA].m_kCenter;
        else
            akU[i] = m_akECenter[iA - m_iTriangleQuantity];
    }

    // compute intermediate terms
    Real afCenT[3], afCen0[3], afCen1[3], afCen2[3];

    ComputeBarycenter(akV[0],akV[1],akV[2],rkTData.m_kCenter,afCenT);
    ComputeBarycenter(akV[0],akV[1],akV[2],akU[0],afCen0);
    ComputeBarycenter(akV[0],akV[1],akV[2],akU[1],afCen1);
    ComputeBarycenter(akV[0],akV[1],akV[2],akU[2],afCen2);

    Real fAlpha = (afCenT[1]*afCen1[0]-afCenT[0]*afCen1[1]) /
        (afCen1[0]-afCenT[0]);
    Real fBeta = (afCenT[2]*afCen2[1]-afCenT[1]*afCen2[2]) /
        (afCen2[1]-afCenT[1]);
    Real fGamma = (afCenT[0]*afCen0[2]-afCenT[2]*afCen0[0]) /
        (afCen0[2]-afCenT[2]);
    Real fOmAlpha = 1.0f - fAlpha, fOmBeta = 1.0f - fBeta;
    Real fOmGamma = 1.0f - fGamma;

    Real fTmp, afA[9], afB[9];

    fTmp = afCenT[0]*akV[0].x+afCenT[1]*akV[1].x+afCenT[2]*akV[2].x;
    afA[0] = 0.5f*(fTmp-akV[0].x);
    afA[1] = 0.5f*(fTmp-akV[1].x);
    afA[2] = 0.5f*(fTmp-akV[2].x);
    afA[3] = 0.5f*fBeta*(akV[2].x-akV[0].x);
    afA[4] = 0.5f*fOmGamma*(akV[1].x-akV[0].x);
    afA[5] = 0.5f*fGamma*(akV[0].x-akV[1].x);
    afA[6] = 0.5f*fOmAlpha*(akV[2].x-akV[1].x);
    afA[7] = 0.5f*fAlpha*(akV[1].x-akV[2].x);
    afA[8] = 0.5f*fOmBeta*(akV[0].x-akV[2].x);

    fTmp = afCenT[0]*akV[0].y+afCenT[1]*akV[1].y+afCenT[2]*akV[2].y;
    afB[0] = 0.5f*(fTmp-akV[0].y);
    afB[1] = 0.5f*(fTmp-akV[1].y);
    afB[2] = 0.5f*(fTmp-akV[2].y);
    afB[3] = 0.5f*fBeta*(akV[2].y-akV[0].y);
    afB[4] = 0.5f*fOmGamma*(akV[1].y-akV[0].y);
    afB[5] = 0.5f*fGamma*(akV[0].y-akV[1].y);
    afB[6] = 0.5f*fOmAlpha*(akV[2].y-akV[1].y);
    afB[7] = 0.5f*fAlpha*(akV[1].y-akV[2].y);
    afB[8] = 0.5f*fOmBeta*(akV[0].y-akV[2].y);

    // compute Bezier coefficients
    rkTData.m_afCoeff[ 2] = afJet[0].m_fF;
    rkTData.m_afCoeff[ 4] = afJet[1].m_fF;
    rkTData.m_afCoeff[ 6] = afJet[2].m_fF;

    rkTData.m_afCoeff[14] = afJet[0].m_fF + afA[0]*afJet[0].m_fFx +
        afB[0]*afJet[0].m_fFy;

    rkTData.m_afCoeff[ 7] = afJet[0].m_fF + afA[3]*afJet[0].m_fFx +
        afB[3]*afJet[0].m_fFy;

    rkTData.m_afCoeff[ 8] = afJet[0].m_fF + afA[4]*afJet[0].m_fFx +
        afB[4]*afJet[0].m_fFy;

    rkTData.m_afCoeff[16] = afJet[1].m_fF + afA[1]*afJet[1].m_fFx +
        afB[1]*afJet[1].m_fFy;

    rkTData.m_afCoeff[ 9] = afJet[1].m_fF + afA[5]*afJet[1].m_fFx +
        afB[5]*afJet[1].m_fFy;

    rkTData.m_afCoeff[10] = afJet[1].m_fF + afA[6]*afJet[1].m_fFx +
        afB[6]*afJet[1].m_fFy;

    rkTData.m_afCoeff[18] = afJet[2].m_fF + afA[2]*afJet[2].m_fFx +
        afB[2]*afJet[2].m_fFy;

    rkTData.m_afCoeff[11] = afJet[2].m_fF + afA[7]*afJet[2].m_fFx +
        afB[7]*afJet[2].m_fFy;

    rkTData.m_afCoeff[12] = afJet[2].m_fF + afA[8]*afJet[2].m_fFx +
        afB[8]*afJet[2].m_fFy;

    rkTData.m_afCoeff[ 5] = fAlpha*rkTData.m_afCoeff[10] +
        fOmAlpha*rkTData.m_afCoeff[11];

    rkTData.m_afCoeff[17] = fAlpha*rkTData.m_afCoeff[16] +
        fOmAlpha*rkTData.m_afCoeff[18];

    rkTData.m_afCoeff[ 1] = fBeta*rkTData.m_afCoeff[12] +
        fOmBeta*rkTData.m_afCoeff[ 7];

    rkTData.m_afCoeff[13] = fBeta*rkTData.m_afCoeff[18] +
        fOmBeta*rkTData.m_afCoeff[14];

    rkTData.m_afCoeff[ 3] = fGamma*rkTData.m_afCoeff[ 8] +
        fOmGamma*rkTData.m_afCoeff[ 9];

    rkTData.m_afCoeff[15] = fGamma*rkTData.m_afCoeff[14] +
        fOmGamma*rkTData.m_afCoeff[16]; 

    rkTData.m_afCoeff[ 0] = afCenT[0]*rkTData.m_afCoeff[14] + 
        afCenT[1]*rkTData.m_afCoeff[16] + afCenT[2]*rkTData.m_afCoeff[18];
}
//----------------------------------------------------------------------------
bool Interp2DQdrNonuniform::Evaluate (const Vector2& rkPoint, Real& rfF,
    Real& rfFx, Real& rfFy)
{
    // determine which triangle contains the target point
    Vector2 kV0, kV1, kV2;
    int i;
    for (i = 0; i < m_iTriangleQuantity; i++)
    {
        Triangle& rkTri = m_akTriangle[i];
        kV0 = m_akVertex[rkTri.m_aiVertex[0]];
        kV1 = m_akVertex[rkTri.m_aiVertex[1]];
        kV2 = m_akVertex[rkTri.m_aiVertex[2]];

        if ( InTriangle(kV0,kV1,kV2,rkPoint) )
            break;
    }
    if ( i == m_iTriangleQuantity )
    {
        // point is outside interpolation region
        return false;
    }

    // the input point is in this triangle
    Triangle& rkTri = m_akTriangle[i];
    TriangleData& rkTData = m_akTData[i];

    // determine which of the six subtriangles contains the target point
    Vector2 kSub0 = rkTData.m_kCenter;
    Vector2 kSub1;
    Vector2 kSub2 = rkTData.m_akIntersect[2];
    int iIndex;
    for (iIndex = 1; iIndex <= 6; iIndex++)
    {
        kSub1 = kSub2;
        if ( iIndex % 2 )
            kSub2 = m_akVertex[rkTri.m_aiVertex[iIndex/2]];
        else
            kSub2 = rkTData.m_akIntersect[iIndex/2-1];

        if ( InTriangle(kSub0,kSub1,kSub2,rkPoint) )
            break;
    }

    // This should not happen theoretically, but it can happen due to
    // numerical round-off errors.  Just in case, select an index and go
    // with it.  Probably better is to keep track of the dot products in
    // InTriangle and find the one closest to zero and use a triangle that
    // contains the edge as the one that contains the input point.
    assert( iIndex <= 6 );
    if ( iIndex > 6 )
        iIndex = 1;

    // compute barycentric coordinates with respect to subtriangle
    Real afBary[3];
    ComputeBarycenter(kSub0,kSub1,kSub2,rkPoint,afBary);

    // fetch Bezier control points
    Real afBez[6] =
    {
        rkTData.m_afCoeff[0],
        rkTData.m_afCoeff[12 + iIndex],
        rkTData.m_afCoeff[13 + (iIndex % 6)],
        rkTData.m_afCoeff[iIndex],
        rkTData.m_afCoeff[6 + iIndex],
        rkTData.m_afCoeff[1 + (iIndex % 6)]
    };

    // evaluate Bezier quadratic
    rfF = afBary[0]*(afBez[0]*afBary[0] + afBez[1]*afBary[1] +
        afBez[2]*afBary[2]) + afBary[1]*(afBez[1]*afBary[0] +
        afBez[3]*afBary[1] + afBez[4]*afBary[2]) + afBary[2]*(
        afBez[2]*afBary[0] + afBez[4]*afBary[1] + afBez[5]*afBary[2]);

    // evaluate barycentric derivatives of F
    Real fFu = 2.0f*(afBez[0]*afBary[0] + afBez[1]*afBary[1] +
        afBez[2]*afBary[2]);
    Real fFv = 2.0f*(afBez[1]*afBary[0] + afBez[3]*afBary[1] +
        afBez[4]*afBary[2]);
    Real fFw = 2.0f*(afBez[2]*afBary[0] + afBez[4]*afBary[1] +
        afBez[5]*afBary[2]);
    Real fDuw = fFu - fFw;
    Real fDvw = fFv - fFw;

    // convert back to (x,y) coordinates
    Real fM00 = kSub0.x - kSub2.x;
    Real fM10 = kSub0.y - kSub2.y;
    Real fM01 = kSub1.x - kSub2.x;
    Real fM11 = kSub1.y - kSub2.y;
    Real fInvDet = 1.0f/(fM00*fM11 - fM10*fM01);

    rfFx = fInvDet*(fM11*fDuw - fM10*fDvw);
    rfFy = fInvDet*(fM00*fDvw - fM01*fDuw);

    return true;
}
//----------------------------------------------------------------------------


